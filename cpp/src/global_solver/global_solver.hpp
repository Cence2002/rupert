#pragma once

#include "global_solver/config.hpp"
#include "global_solver/io.hpp"
#include "global_solver/helpers.hpp"
#include "queue/queues.hpp"
#include <thread>

const std::string polyhedra_file_name = "polyhedra.bin";
const std::string terminal_boxes_file_name = "terminal_boxes.bin";
const std::string boxes_file_name = "boxes.bin";
const std::string debug_file_name = "debug.bin";

template<IntervalType Interval>
struct GlobalSolver {
private:
    const Config<Interval>& config_;
    std::atomic<bool> interrupted_;

    ConcurrentPriorityQueue<Range3> hole_orientation_queue_;
    std::vector<std::thread> threads_;
    std::atomic<uint8_t> terminated_threads_;

    ConcurrentQueue<Elimination> export_queue_;
    std::thread export_thread_;

    DebugExporter<Interval> debug_exporter_;

    Polygon<Interval> get_hole_projection(const Range3& hole_orientation) {
        std::vector<Vector2<Interval>> vectors;
        for(const Vector3<Interval>& vertex: config_.polyhedron.vertices()) {
            for(const Vector2<Interval>& projected_hole_vertex: projected_orientation_hull(vertex, hole_orientation.theta<Interval>(), hole_orientation.phi<Interval>(), config_.projection_resolution)) {
                for(const Vector2<Interval>& rotated_projected_hole_vertex: rotation_hull(projected_hole_vertex, hole_orientation.alpha<Interval>(), config_.rotation_resolution)) {
                    vectors.push_back(rotated_projected_hole_vertex);
                    if(config_.debug) {
                        debug_exporter_.debug_builder.box_builder.projection_builder.add_vertex(rotated_projected_hole_vertex);
                    }
                }
            }
            if(config_.debug) {
                debug_exporter_.debug_builder.box_builder.add_projection();
            }
        }
        return convex_hull(deduplicate_vectors(vectors));
    }

    bool is_plug_orientation_ignored(const Range3& hole_orientation, const Range2& plug_orientation) {
        const Interval box_angle_radius = Vector2<Interval>(Interval(hole_orientation.theta<Interval>().len()) + Interval(hole_orientation.alpha<Interval>().len()), Interval(hole_orientation.phi<Interval>().len())).len() / Interval(2);
        const Interval range2_angle_radius = Vector2<Interval>(Interval(plug_orientation.theta<Interval>().len()), Interval(plug_orientation.phi<Interval>().len())).len() / Interval(2);
        const Interval remaining_angle = config_.epsilon - box_angle_radius - range2_angle_radius;
        if(!remaining_angle.pos()) {
            return false;
        }
        const Interval cos_remaining_angle = remaining_angle.cos();
        const Matrix<Interval> hole_matrix = Matrix<Interval>::rotation_z(hole_orientation.alpha<Interval>().mid()) * Matrix<Interval>::orientation(hole_orientation.theta<Interval>().mid(), hole_orientation.phi<Interval>().mid());
        const Matrix<Interval> plug_matrix = Matrix<Interval>::orientation(plug_orientation.theta<Interval>().mid(), plug_orientation.phi<Interval>().mid());
        for(const Matrix<Interval>& rotation: config_.polyhedron.rotations()) {
            if(Matrix<Interval>::relative_rotation(hole_matrix * rotation, plug_matrix).cos_angle() > cos_remaining_angle) {
                return true;
            }
        }
        for(const Matrix<Interval>& reflection: config_.polyhedron.reflections()) {
            if(Matrix<Interval>::relative_rotation(hole_matrix * reflection, Matrix<Interval>::reflection_z() * plug_matrix).cos_angle() > cos_remaining_angle) {
                return true;
            }
        }
        return false;
    }

    bool is_range2_terminal(const Range2& plug_orientation, const Polygon<Interval>& projected_hole) {
        const Interval theta = plug_orientation.theta<Interval>();
        const Interval phi = plug_orientation.phi<Interval>();
        bool is_terminal = false;
        for(size_t vertex_index = 0; vertex_index < config_.polyhedron.vertices().size(); vertex_index++) {
            const Vector3<Interval>& plug_vertex = config_.polyhedron.vertices()[vertex_index];
            if(config_.debug) {
                for(const Vector2<Interval>& projected_plug_vertex: projected_orientation_hull(plug_vertex, theta, phi, config_.projection_resolution)) {
                    debug_exporter_.debug_builder.box_builder.rectangle_builder.projection_builder.add_vertex(projected_plug_vertex);
                }
                debug_exporter_.debug_builder.box_builder.rectangle_builder.add_projection();
            }
            if(projected_oriented_vector_avoids_polygon(projected_hole, plug_vertex, theta, phi)) {
                if(config_.debug) {
                    debug_exporter_.debug_builder.box_builder.rectangle_builder.add_last_as_out_index();
                }
                is_terminal = true;
                if(!config_.debug) {
                    break;
                }
            }
        }
        return is_terminal;
    }

    bool is_box_nonterminal(const Range2& plug_orientation, const Polygon<Interval>& projected_hole) {
        const Interval theta_mid = plug_orientation.theta<Interval>().mid();
        const Interval phi_mid = plug_orientation.phi<Interval>().mid();
        return std::ranges::all_of(config_.polyhedron.vertices(), [&](const Vector3<Interval>& plug_vertex) {
            return projected_hole.inside(trivial_projected_orientation(plug_vertex, theta_mid, phi_mid));
        });
    }

    std::optional<Elimination> get_optional_terminal_box(const Range3& hole_orientation) {
        Polygon<Interval> projected_hole = get_hole_projection(hole_orientation);
        if(config_.debug) {
            debug_exporter_.debug_builder.box_builder.set_projection(projected_hole);
        }
        SerialQueue<Range2> plug_orientation_queue;
        plug_orientation_queue.push(Range2(Range(0, 0), Range(1, 0)));
        std::vector<Range2> eliminated_plug_orientations;
        while(plug_orientation_queue.size() > 0) {
            const std::optional<Range2> optional_range2 = plug_orientation_queue.pop();
            if(!optional_range2.has_value()) {
                return std::make_optional(Elimination(hole_orientation, eliminated_plug_orientations));
            }

            const Range2& range2 = optional_range2.value();
            if(range2.theta_range().depth() >= 15 || range2.phi_range().depth() >= 15) {
                return std::nullopt;
            }
            if(config_.debug) {
                debug_exporter_.debug_builder.box_builder.rectangle_builder.set_rectangle(range2);
            }
            if(is_plug_orientation_ignored(hole_orientation, range2) || is_range2_terminal(range2, projected_hole)) {
                if(config_.debug) {
                    debug_exporter_.debug_builder.box_builder.add_rectangle();
                }
                eliminated_plug_orientations.push_back(range2);
                continue;
            }
            if(is_box_nonterminal(range2, projected_hole)) {
                if(config_.debug) {
                    debug_exporter_.debug_builder.box_builder.add_rectangle();
                    debug_exporter_.debug_builder.box_builder.set_last_as_in_index();
                }
                return std::nullopt;
            }
            if(config_.debug) {
                debug_exporter_.debug_builder.box_builder.add_rectangle();
            }
            for(const Range2& rectangle_part: range2.parts()) {
                plug_orientation_queue.push(rectangle_part);
            }
        }
        return std::nullopt;
    }

    bool process() {
        const std::optional<Range3> optional_box = hole_orientation_queue_.pop();
        if(!optional_box.has_value()) {
            return false;
        }
        const Range3& range3 = optional_box.value();
        if(range3.theta_range().depth() >= 15 || range3.phi_range().depth() >= 15 || range3.alpha_range().depth() >= 15) {
            return false;
        }
        const std::optional<Elimination> optional_terminal_box = get_optional_terminal_box(range3);
        if(config_.debug) {
            debug_exporter_.debug_builder.box_builder.set_box(range3);
            debug_exporter_.debug_builder.box_builder.set_terminal(optional_terminal_box.has_value());
            debug_exporter_.debug_builder.add_box();
        }
        if(optional_terminal_box.has_value()) {
            // std::cout << "Terminal Box: " << box << std::endl; // TODO: enable
            export_queue_.push(optional_terminal_box.value());
        } else {
            // std::cout << "Non Terminal Box: " << box << std::endl; // TODO: enable
            for(const Range3& box_part: range3.parts()) {
                hole_orientation_queue_.push(box_part);
            }
        }
        return true;
    }

    void start_box_processor() {
        while(!interrupted_) {
            if(!process()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }
        mpfr_free_cache2(MPFR_FREE_LOCAL_CACHE);
        ++terminated_threads_;
    }

    void start_exporter() {
        while(!interrupted_) {
            if(export_queue_.size() < config_.export_threshold) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }
            Exporter::export_terminal_boxes(config_.working_directory() / terminal_boxes_file_name, export_queue_.pop_all());
        }
        while(terminated_threads_ < config_.threads) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        Exporter::export_terminal_boxes(config_.working_directory() / terminal_boxes_file_name, export_queue_.pop_all());
        Exporter::export_boxes(config_.working_directory() / boxes_file_name, hole_orientation_queue_.pop_all());

        if(config_.debug) {
            debug_exporter_.export_debug(config_.working_directory() / debug_file_name);
        }

        mpfr_free_cache();
    }

public:
    explicit GlobalSolver(const Config<Interval>& config) : config_(config),
                                                            interrupted_(false),
                                                            hole_orientation_queue_(),
                                                            threads_(),
                                                            terminated_threads_(0),
                                                            export_queue_(),
                                                            export_thread_(),
                                                            debug_exporter_() {
        if(config_.debug) {
            debug_exporter_.debug_builder.set_hole(config_.polyhedron);
            debug_exporter_.debug_builder.set_plug(config_.polyhedron);
        }
    }

    void init() {
        if(config_.restart) {
            Importer::restart(config_.working_directory());
        }
        Exporter::create_working_directory(config_.working_directory());
        Exporter::export_polyhedra(config_.working_directory() / polyhedra_file_name, config_.polyhedron);
        const std::vector<Range3> boxes = Importer::import_boxes(config_.working_directory() / boxes_file_name);
        if(boxes.empty()) {
            // box_queue_.push(Box()); // TODO: Use this instead of the magic numbers
            hole_orientation_queue_.push(Range3(
                Range(9, 0b011011010),
                Range(10, 0b1011001001),
                Range(9, 0b101101110)
            ));
        } else {
            hole_orientation_queue_.push_all(boxes);
        }
    }

    void start() {
        std::ranges::generate_n(std::back_inserter(threads_), config_.threads, [this] {
            return std::thread([this] {
                start_box_processor();
            });
        });
        export_thread_ = std::thread([this] {
            start_exporter();
        });

        for(std::thread& thread: threads_) {
            thread.join();
        }
        export_thread_.join();
    }

    void stop() {
        interrupted_ = true;
    }
};
