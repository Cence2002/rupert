#pragma once

#include "global_solver/config.hpp"
#include "global_solver/io.hpp"
#include "global_solver/helpers.hpp"
#include "queue/queues.hpp"
#include <thread>
#include <latch>

const std::string polyhedron_file_name = "polyhedra.bin";
const std::string eliminated_hole_orientations_file_name = "terminal_boxes.bin";
const std::string hole_orientations_file_name = "boxes.bin";
const std::string debug_file_name = "debug.bin";

template<IntervalType Interval>
class GlobalSolver {
    const Config<Interval>& config_;

    ConcurrentPriorityQueue<Range3> hole_orientations_{};
    std::atomic<size_t> pending_hole_orientations_{0};
    std::vector<std::thread> threads_{};
    std::atomic<bool> interrupted_;

    ConcurrentQueue<EliminatedHoleOrientation> eliminated_hole_orientations_{};
    std::thread exporter_thread_{};
    std::latch exporter_latch_;

    DebugExporter<Interval> debug_exporter_{};

    Polygon<Interval> oriented_hole_projection(const Range3& hole_orientation) {
        std::vector<Vector2<Interval>> all_vectors;
        for(const Vector3<Interval>& vertex: config_.polyhedron.vertices()) {
            for(const Vector2<Interval>& vectors: projected_orientation_hull(vertex, hole_orientation.theta<Interval>(), hole_orientation.phi<Interval>(), config_.projection_resolution)) {
                for(const Vector2<Interval>& vector: rotation_hull(vectors, hole_orientation.alpha<Interval>(), config_.rotation_resolution)) {
                    all_vectors.push_back(vector);
                    if(config_.debug) {
                        debug_exporter_.debug_builder.box_builder.projection_builder.add_vertex(vector);
                    }
                }
            }
            if(config_.debug) {
                debug_exporter_.debug_builder.box_builder.add_projection();
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
            if(config_.debug) {
                for(const Vector2<Interval>& projected_plug_vertex: projected_orientation_hull(vertex, plug_orientation.theta<Interval>(), plug_orientation.phi<Interval>(), config_.projection_resolution)) {
                    debug_exporter_.debug_builder.box_builder.rectangle_builder.projection_builder.add_vertex(projected_plug_vertex);
                }
                debug_exporter_.debug_builder.box_builder.rectangle_builder.add_projection();
            }
            if(projected_oriented_vector_avoids_polygon(projected_oriented_hole, vertex, plug_orientation.theta<Interval>(), plug_orientation.phi<Interval>())) {
                if(config_.debug) {
                    debug_exporter_.debug_builder.box_builder.rectangle_builder.add_last_as_out_index();
                }
                eliminated = true;
                if(!config_.debug) {
                    break;
                }
            }
        }
        return eliminated;
    }

    std::pair<std::vector<Range2>, std::vector<Range2>> check_hole_orientation(const Range3& hole_orientation) {
        Polygon<Interval> projected_oriented_hole = oriented_hole_projection(hole_orientation);
        if(config_.debug) {
            debug_exporter_.debug_builder.box_builder.set_projection(projected_oriented_hole);
        }
        SerialQueue<Range2> plug_orientations;
        plug_orientations.push(Range2(Range(0, 0), Range(0, 0)));
        std::vector<Range2> eliminated_plug_orientations;
        std::vector<Range2> terminal_plug_orientations;
        while(plug_orientations.size() > 0) {
            const std::optional<Range2> optional_plug_orientation = plug_orientations.pop();
            if(!optional_plug_orientation.has_value()) {
                throw std::runtime_error("plug_orientations.size() > 0 but none popped");
            }

            const Range2& plug_orientation = optional_plug_orientation.value();
            if(config_.debug) {
                debug_exporter_.debug_builder.box_builder.rectangle_builder.set_rectangle(plug_orientation);
            }
            if(skip_plug_orientation(hole_orientation, plug_orientation) || check_plug_orientation(plug_orientation, projected_oriented_hole)) {
                if(config_.debug) {
                    debug_exporter_.debug_builder.box_builder.add_rectangle();
                }
                eliminated_plug_orientations.push_back(plug_orientation);
                continue;
            }
            if(std::ranges::all_of(config_.polyhedron.vertices(), [&](const Vector3<Interval>& vertex) {
                return projected_oriented_hole.inside(trivial_projected_orientation(vertex, plug_orientation.theta<Interval>().mid(), plug_orientation.phi<Interval>().mid()));
            })) {
                if(config_.debug) {
                    debug_exporter_.debug_builder.box_builder.add_rectangle();
                    debug_exporter_.debug_builder.box_builder.set_last_as_in_index();
                }
                return std::make_pair(std::vector<Range2>{}, std::vector<Range2>{});
            }
            if(config_.debug) {
                debug_exporter_.debug_builder.box_builder.add_rectangle();
            }
            if(plug_orientation.terminal()) {
                terminal_plug_orientations.push_back(plug_orientation);
                continue;
            }
            for(const Range2& rectangle_part: plug_orientation.parts()) {
                plug_orientations.push(rectangle_part);
            }
        }
        return std::make_pair(eliminated_plug_orientations, terminal_plug_orientations);
    }

    void process_hole_orientation(const Range3& hole_orientation) {
        const auto [eliminated_plug_orientations, terminal_plug_orientations] = check_hole_orientation(hole_orientation);
        const bool hole_orientation_eliminated = eliminated_plug_orientations.size() > 0 && terminal_plug_orientations.size() == 0;
        if(config_.debug) {
            debug_exporter_.debug_builder.box_builder.set_box(hole_orientation);
            debug_exporter_.debug_builder.box_builder.set_terminal(hole_orientation_eliminated);
            debug_exporter_.debug_builder.add_box();
        }
        if(hole_orientation_eliminated) {
            std::cout << "Eliminated hole orientation: " << hole_orientation << std::endl;
            eliminated_hole_orientations_.push(EliminatedHoleOrientation(hole_orientation, eliminated_plug_orientations));
            return;
        }
        // TODO: refine termination criterion to use epsilon (potentially a new one)
        if(hole_orientation.terminal()) {
            // TODO: Export terminal_plug_orientations and deduplicate orientation pairs
            throw std::runtime_error("Range overflow");
        }
        for(const Range3& box_part: hole_orientation.parts()) {
            hole_orientations_.push(box_part);
        }
    }

    void start_box_processor() {
        while(!interrupted_) {
            const size_t pending_hole_orientations_before = pending_hole_orientations_.fetch_add(1);
            const std::optional<Range3> optional_hole_orientation = hole_orientations_.pop();
            if(optional_hole_orientation.has_value()) {
                process_hole_orientation(optional_hole_orientation.value());
                pending_hole_orientations_.fetch_sub(1);
            } else {
                if(pending_hole_orientations_before == 0) {
                    pending_hole_orientations_.fetch_sub(1);
                    interrupt();
                    continue;
                }
                pending_hole_orientations_.fetch_sub(1);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        mpfr_free_cache2(MPFR_FREE_LOCAL_CACHE);
        exporter_latch_.count_down();
    }

    void start_exporter() {
        while(!interrupted_) {
            if(eliminated_hole_orientations_.size() > config_.export_threshold) {
                Exporter::export_terminal_boxes(config_.working_directory() / eliminated_hole_orientations_file_name, eliminated_hole_orientations_.pop_all());
            }
            std::this_thread::yield();
        }
        exporter_latch_.wait();

        Exporter::export_terminal_boxes(config_.working_directory() / eliminated_hole_orientations_file_name, eliminated_hole_orientations_.pop_all());
        Exporter::export_boxes(config_.working_directory() / hole_orientations_file_name, hole_orientations_.pop_all());

        if(config_.debug) {
            debug_exporter_.export_debug(config_.working_directory() / debug_file_name);
        }

        mpfr_free_cache();
    }

public:
    explicit GlobalSolver(const Config<Interval>& config) : config_(config),
                                                            interrupted_(false),
                                                            exporter_latch_(config.threads) {
        if(config_.debug) {
            debug_exporter_.debug_builder.set_hole(config_.polyhedron);
            debug_exporter_.debug_builder.set_plug(config_.polyhedron);
        }
    }

    void setup() {
        if(config_.restart) {
            Importer::restart(config_.working_directory());
            // TODO: Find starting point (or subset)
            hole_orientations_.push(Range3(
                Range(9, 0b011011010),
                Range(10, 0b1011001001),
                Range(9, 0b101101110)
            ));
        } else {
            hole_orientations_.push_all(Importer::import_boxes(config_.working_directory() / hole_orientations_file_name));
        }
        Exporter::create_working_directory(config_.working_directory());
        Exporter::export_polyhedra(config_.working_directory() / polyhedron_file_name, config_.polyhedron);
    }

    void run() {
        std::ranges::generate_n(std::back_inserter(threads_), config_.threads, [this] {
            return std::thread([this] {
                start_box_processor();
            });
        });
        exporter_thread_ = std::thread([this] {
            start_exporter();
        });

        for(std::thread& thread: threads_) {
            thread.join();
        }
        exporter_thread_.join();
    }

    void interrupt() {
        interrupted_ = true;
    }
};
