#pragma once

#include "global_solver/config.hpp"
#include "global_solver/exporter.hpp"
#include "global_solver/helpers.hpp"
#include "queue/queues.hpp"
#include <thread>
#include <latch>

const std::string polyhedron_file_name = "polyhedron.bin";
const std::string pruned_hole_orientations_file_name = "pruned_hole_orientations.bin";
const std::string unpruned_hole_orientations_file_name = "unpruned_hole_orientations.bin";

/*
HO = Hole Orientation
PO = Plug Orientation

HOs, prunedHOs, unprunedHOs = [full], [], []
for HO in HOs:
    POs, prunedPOs, unprunedPOs = [full], [], []
    for PO in POs:
        if PO sample fits in HO sample: export(HO, PO) and terminate
        if PO sample fits in HO: set HOprunable to false, break
        if |PO, HO| < threshold: continue
        if PO is prunable: add PO to prunedPOs
        elif |PO| < threshold: add PO and all POs to unprunedPOs
        else: add pieces of PO to POs
    if HOprunable: add HO and prunedPOs to prunedHOs
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

    bool plug_orientation_sample_inside_hole_orientation_sample(const Range3& hole_orientation, const Range2& plug_orientation) const {
        const Interval hole_theta = hole_orientation.theta_mid<Interval>();
        const Interval hole_phi = hole_orientation.phi_mid<Interval>();
        const Interval hole_alpha = hole_orientation.alpha_mid<Interval>();
        const Vector3<Interval> direction = Vector3<Interval>(
            hole_theta.sin() * hole_phi.sin(),
            hole_theta.cos() * hole_phi.sin(),
            hole_phi.cos()
        );
        const Bitset normal_mask = config_.polyhedron.get_normal_mask(direction);
        const auto outline_iterator = std::ranges::find_if(config_.polyhedron.outlines(), [&](const Outline& candidate_outline) {
            return candidate_outline.normal_mask == normal_mask;
        });
        if(outline_iterator == config_.polyhedron.outlines().end()) {
            return false;
        }
        const Outline& outline = *outline_iterator;
        const Matrix<Interval> hole_matrix = Matrix<Interval>::rotation_z(hole_alpha) * Matrix<Interval>::orientation(hole_theta, hole_phi);
        std::vector<Vector2<Interval>> projected_vertices;
        for(const size_t vertex_index: outline.vertex_indices) {
            const Vector3<Interval> vertex = config_.polyhedron.vertices()[vertex_index];
            const Vector3<Interval> projected_vertex = hole_matrix * vertex;
            projected_vertices.emplace_back(
                projected_vertex.x(),
                projected_vertex.y()
            );
        }
        std::vector<Edge<Interval>> projected_edges;
        for(size_t index = 0; index < projected_vertices.size(); ++index) {
            const size_t next_index = (index + 1) % projected_vertices.size();
            projected_edges.emplace_back(projected_vertices[index], projected_vertices[next_index]);
        }
        const Polygon<Interval> projected_hole = Polygon<Interval>(projected_edges);
        return plug_orientation_sample_inside_hole_orientation(projected_hole, plug_orientation);
    }

    bool plug_orientation_sample_inside_hole_orientation(const Polygon<Interval>& projected_hole, const Range2& plug_orientation) const {
        const Interval plug_theta = plug_orientation.theta_mid<Interval>();
        const Interval plug_phi = plug_orientation.phi_mid<Interval>();
        return std::ranges::all_of(config_.polyhedron.vertices(), [&](const Vector3<Interval>& vertex) {
            return projected_hole.inside(trivial_orientation(vertex, plug_theta, plug_phi));
        });
    }

    bool plug_orientation_skippable(const Range3& hole_orientation, const Range2& plug_orientation) const {
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
                   return cos_remaining_angle < Matrix<Interval>::relative_rotation(plug_matrix, hole_matrix * rotation).cos_angle();
               }) ||
               std::ranges::any_of(config_.polyhedron.reflections(), [&](const Matrix<Interval>& reflection) {
                   return cos_remaining_angle < Matrix<Interval>::relative_rotation(plug_matrix, Matrix<Interval>::reflection_z() * hole_matrix * reflection).cos_angle();
               });
    }

    bool plug_orientation_prunable(const Range2& plug_orientation, const Polygon<Interval>& projected_hole) const {
        return std::ranges::any_of(config_.polyhedron.vertices(), [&](const Vector3<Interval>& vertex) {
            return projected_oriented_vector_avoids_polygon(projected_hole, vertex, plug_orientation.theta<Interval>(), plug_orientation.phi<Interval>());
        });
    }

    std::tuple<bool, std::vector<Range2>, std::vector<Range2>> process_plug_orientations(const Range3& hole_orientation) {
        const Polygon<Interval> projected_hole = project_polyhedron(config_.polyhedron, hole_orientation, config_.projection_resolution, config_.rotation_resolution);
        SerialQueue<Range2> plug_orientations;
        plug_orientations.add(Range2(Range(0, 0), Range(1, 0)));
        std::vector<Range2> pruned_plug_orientations;
        std::vector<Range2> unpruned_plug_orientations;
        while(plug_orientations.size() > 0) {
            const std::optional<Range2> optional_plug_orientation = plug_orientations.fetch();
            if(!optional_plug_orientation.has_value()) {
                throw std::runtime_error("plug_orientations is empty");
            }
            const Range2& plug_orientation = optional_plug_orientation.value();
            if(plug_orientation_sample_inside_hole_orientation_sample(hole_orientation, plug_orientation)) {
                std::cout << "Rupert passage found for hole orientation: " << hole_orientation << " and plug orientation: " << plug_orientation << std::endl;
                rupert_ = true;
                interrupt();
                return std::make_tuple(false, std::vector<Range2>{}, std::vector<Range2>{});
            }
            if(plug_orientation_sample_inside_hole_orientation(projected_hole, plug_orientation)) {
                return std::make_tuple(false, std::vector<Range2>{}, std::vector<Range2>{});
            }
            if(plug_orientation_skippable(hole_orientation, plug_orientation)) {
                plug_orientations.ack();
                continue;
            }
            if(plug_orientation_prunable(plug_orientation, projected_hole)) {
                pruned_plug_orientations.push_back(plug_orientation);
                plug_orientations.ack();
                continue;
            }
            if(plug_orientation.terminal()) {
                // TODO: use a better threshold
                unpruned_plug_orientations.push_back(plug_orientation);
                plug_orientations.ack();
                continue;
            }
            for(const Range2& rectangle_part: plug_orientation.parts()) {
                plug_orientations.add(rectangle_part);
            }
            plug_orientations.ack();
        }
        return std::make_tuple(unpruned_plug_orientations.empty(), pruned_plug_orientations, unpruned_plug_orientations);
    }

    void process_hole_orientation(const Range3& hole_orientation) {
        const auto [hole_orientation_prunable, pruned_plug_orientations, unpruned_plug_orientations] = process_plug_orientations(hole_orientation);
        if(hole_orientation_prunable) {
            std::cout << "Hole orientation is prunable: " << hole_orientation << std::endl;
            pruned_hole_orientations_.add(CombinedOrientation(hole_orientation, pruned_plug_orientations));
            return;
        }
        if(hole_orientation.terminal()) {
            // TODO: use a better threshold
            std::cout << "Hole orientation is unprunable: " << hole_orientation << std::endl;
            unpruned_hole_orientations_.add(CombinedOrientation(hole_orientation, unpruned_plug_orientations));
            return;
        }
        for(const Range3& hole_orientation_part: hole_orientation.parts()) {
            hole_orientations_.add(hole_orientation_part);
        }
    }

    void processor_hole_orientations() {
        while(!interrupted_ && hole_orientations_.size() > 0) {
            const std::optional<Range3> optional_hole_orientation = hole_orientations_.fetch();
            if(optional_hole_orientation.has_value()) {
                process_hole_orientation(optional_hole_orientation.value());
                hole_orientations_.ack();
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        mpfr_free_cache2(MPFR_FREE_LOCAL_CACHE);
        exporter_latch_.count_down();
    }

public:
    explicit GlobalSolver(const Config<Interval>& config) : config_(config), exporter_latch_(config.threads) {}

    void run() {
        // hole_orientations_.add(Range3(
        //     Range(9, 0b011011010),
        //     Range(10, 0b1011001001),
        //     Range(9, 0b101101110)
        // ));
        hole_orientations_.add(Range3(
            Range(0, 0),
            Range(1, 0),
            Range(1, 0)
        ));
        Exporter::create_empty_working_directory(config_.working_directory());
        Exporter::export_polyhedron(config_.working_directory() / polyhedron_file_name, config_.polyhedron);
        std::ranges::generate_n(std::back_inserter(threads_), config_.threads, [this] {
            return std::thread([this] {
                processor_hole_orientations();
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
