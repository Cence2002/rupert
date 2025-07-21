#pragma once

#include "global_solver/config.hpp"
#include "global_solver/exporter.hpp"
#include "global_solver/helpers.hpp"
#include "queue/queues.hpp"
#include <thread>
#include <latch>

const std::string polyhedron_file_name = "polyhedra.bin";
const std::string pruned_hole_orientations_file_name = "pruned_hole_orientations.bin";
const std::string unpruned_hole_orientations_file_name = "unpruned_hole_orientations.bin";

/*
HO = Hole Orientation
PO = Plug Orientation

HOs, prunedHOs, unprunedHOs = [full], [], []
for HO in HOs:
    POs, prunedPOs, unprunedPOs = [full], [], []
    for PO in POs:
        if PO fits in HO: export(HO, PO) and terminate
        if |PO, HO| < threshold: continue
        if PO is prunable: add PO to prunedPOs
        elif |PO| < threshold: add PO and all POs to unprunedPOs
        else: add pieces of PO to POs
    if unprunedPOs is empty: add HO and prunedPOs to prunedHOs
    elif |HO| < threshold: add HO and unprunedPOs to unprunedHOs
    else: add pieces of HO to HOs
export(prunedHOs, unprunedHOs)
*/

template<IntervalType Interval>
class GlobalSolver {
    const Config<Interval>& config_;

    ConcurrentPriorityQueue<Range3> hole_orientations_{};
    std::vector<std::thread> threads_{};
    std::atomic<bool> interrupted_{false};
    std::atomic<bool> rupert_{false};

    ConcurrentQueue<CombinedOrientation> pruned_hole_orientations_{};
    ConcurrentQueue<CombinedOrientation> unpruned_hole_orientations_{};
    std::latch exporter_latch_;

    Polygon<Interval> oriented_hole_projection(const Range3& hole_orientation) {
        std::vector<Vector2<Interval>> all_vectors;
        for(const Vector3<Interval>& vertex: config_.polyhedron.vertices()) {
            for(const Vector2<Interval>& vectors: projected_orientation_hull(vertex, hole_orientation.theta<Interval>(), hole_orientation.phi<Interval>(), config_.projection_resolution)) {
                for(const Vector2<Interval>& vector: rotation_hull(vectors, hole_orientation.alpha<Interval>(), config_.rotation_resolution)) {
                    all_vectors.push_back(vector);
                }
            }
        }
        return convex_hull(deduplicate_vectors(all_vectors));
    }

    bool skip_plug_orientation(const Range3& hole_orientation, const Range2& plug_orientation) {
        const Interval hole_orientation_angle_radius = Vector2<Interval>(hole_orientation.theta<Interval>().rad() + hole_orientation.alpha<Interval>().rad(), hole_orientation.phi<Interval>().rad()).len();
        const Interval plug_orientation_angle_radius = Vector2<Interval>(plug_orientation.theta<Interval>().rad(), plug_orientation.phi<Interval>().rad()).len();
        const Interval remaining_angle = config_.epsilon - hole_orientation_angle_radius - plug_orientation_angle_radius;
        if(!remaining_angle.pos()) {
            return false;
        }
        const Interval cos_remaining_angle = remaining_angle.cos();
        const Matrix<Interval> hole_matrix = Matrix<Interval>::rotation_z(hole_orientation.alpha<Interval>().mid()) * Matrix<Interval>::orientation(hole_orientation.theta<Interval>().mid(), hole_orientation.phi<Interval>().mid());
        const Matrix<Interval> plug_matrix = Matrix<Interval>::orientation(plug_orientation.theta<Interval>().mid(), plug_orientation.phi<Interval>().mid());
        return std::ranges::any_of(config_.polyhedron.rotations(), [&](const Matrix<Interval>& rotation) {
                   return Matrix<Interval>::relative_rotation(hole_matrix * rotation, plug_matrix).cos_angle() > cos_remaining_angle;
               }) ||
               std::ranges::any_of(config_.polyhedron.reflections(), [&](const Matrix<Interval>& reflection) {
                   return Matrix<Interval>::relative_rotation(hole_matrix * reflection, Matrix<Interval>::reflection_z() * plug_matrix).cos_angle() > cos_remaining_angle;
               });
    }

    bool check_plug_orientation(const Range2& plug_orientation, const Polygon<Interval>& projected_oriented_hole) {
        bool eliminated = false;
        for(const Vector3<Interval>& vertex: config_.polyhedron.vertices()) {
            if(projected_oriented_vector_avoids_polygon(projected_oriented_hole, vertex, plug_orientation.theta<Interval>(), plug_orientation.phi<Interval>())) {
                eliminated = true;
            }
        }
        return eliminated;
    }

    std::pair<std::vector<Range2>, std::vector<Range2>> check_hole_orientation(const Range3& hole_orientation) {
        const Polygon<Interval> projected_oriented_hole = oriented_hole_projection(hole_orientation);
        SerialQueue<Range2> plug_orientations;
        plug_orientations.add(Range2(Range(1, 0), Range(1, 0)));
        plug_orientations.add(Range2(Range(1, 1), Range(1, 0)));
        std::vector<Range2> eliminated_plug_orientations;
        std::vector<Range2> terminal_plug_orientations;
        while(plug_orientations.size() > 0) {
            const std::optional<Range2> optional_plug_orientation = plug_orientations.fetch();
            if(!optional_plug_orientation.has_value()) {
                throw std::runtime_error("plug_orientations.size() > 0 but none popped");
            }

            const Range2& plug_orientation = optional_plug_orientation.value();
            if(skip_plug_orientation(hole_orientation, plug_orientation) || check_plug_orientation(plug_orientation, projected_oriented_hole)) {
                eliminated_plug_orientations.push_back(plug_orientation);
                plug_orientations.ack();
                continue;
            }
            if(std::ranges::all_of(config_.polyhedron.vertices(), [&](const Vector3<Interval>& vertex) {
                return projected_oriented_hole.inside(trivial_projected_orientation(vertex, plug_orientation.theta<Interval>().mid(), plug_orientation.phi<Interval>().mid()));
            })) {
                return std::make_pair(std::vector<Range2>{}, std::vector<Range2>{});
            }
            if(plug_orientation.terminal()) {
                terminal_plug_orientations.push_back(plug_orientation);
                plug_orientations.ack();
                continue;
            }
            for(const Range2& rectangle_part: plug_orientation.parts()) {
                plug_orientations.add(rectangle_part);
            }
            plug_orientations.ack();
        }
        return std::make_pair(eliminated_plug_orientations, terminal_plug_orientations);
    }

    void process_hole_orientation(const Range3& hole_orientation) {
        const auto [eliminated_plug_orientations, terminal_plug_orientations] = check_hole_orientation(hole_orientation);
        const bool hole_orientation_eliminated = eliminated_plug_orientations.size() > 0 && terminal_plug_orientations.size() == 0;
        if(hole_orientation_eliminated) {
            std::cout << "Eliminated hole orientation: " << hole_orientation << std::endl;
            pruned_hole_orientations_.add(CombinedOrientation(hole_orientation, eliminated_plug_orientations));
            return;
        }
        // TODO: refine termination criterion to use epsilon (potentially a new one)
        if(hole_orientation.terminal()) {
            // TODO: Export terminal_plug_orientations and deduplicate orientation pairs
            throw std::runtime_error("Range overflow");
        }
        for(const Range3& box_part: hole_orientation.parts()) {
            hole_orientations_.add(box_part);
        }
    }

    void start_box_processor() {
        while(!interrupted_) {
            const std::optional<Range3> optional_hole_orientation = hole_orientations_.fetch();
            if(optional_hole_orientation.has_value()) {
                process_hole_orientation(optional_hole_orientation.value());
                hole_orientations_.ack();
            } else {
                if(hole_orientations_.size() == 0) {
                    interrupt();
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        mpfr_free_cache2(MPFR_FREE_LOCAL_CACHE);
        exporter_latch_.count_down();
    }

public:
    explicit GlobalSolver(const Config<Interval>& config) : config_(config), exporter_latch_(config.threads) {}

    void run() {
        hole_orientations_.add(Range3(
            Range(9, 0b011011010),
            Range(10, 0b1011001001),
            Range(9, 0b101101110)
        ));
        Exporter::create_empty_working_directory(config_.working_directory());
        Exporter::export_polyhedron(config_.working_directory() / polyhedron_file_name, config_.polyhedron);
        std::ranges::generate_n(std::back_inserter(threads_), config_.threads, [this] {
            return std::thread([this] {
                start_box_processor();
            });
        });
        for(std::thread& thread: threads_) {
            thread.join();
        }
        exporter_latch_.wait();
        if(!rupert_) {
            Exporter::export_combined_orientations(config_.working_directory() / pruned_hole_orientations_file_name, pruned_hole_orientations_.pop_all());
            Exporter::export_combined_orientations(config_.working_directory() / unpruned_hole_orientations_file_name, unpruned_hole_orientations_.pop_all());
        }
        mpfr_free_cache();
    }

    void interrupt() {
        interrupted_ = true;
    }
};
