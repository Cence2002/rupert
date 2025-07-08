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

    ConcurrentQueue<Range3> box_queue_;
    ConcurrentQueue<TerminalBox> terminal_box_queue_;

    DebugExporter<Interval> debug_exporter_;

    std::vector<std::thread> processor_threads_;
    std::thread exporter_thread_;
    std::atomic<uint32_t> processed_box_count_;
    std::atomic<bool> terminated_;
    std::atomic<uint8_t> terminated_thread_count_;

    Polygon<Interval> get_projected_hole(const Range3& range3) {
        std::vector<Vector2<Interval>> all_projected_hole_vertices;
        for(const Vector3<Interval>& hole_vertex: config_.polyhedron.vertices()) {
            const std::vector<Vector2<Interval>> projected_hole_vertices = projection_hull_polygon(hole_vertex, range3.theta_interval<Interval>(), range3.phi_interval<Interval>(), config_.projection_resolution);
            for(const Vector2<Interval>& projected_hole_vertex: projected_hole_vertices) {
                const std::vector<Vector2<Interval>> rotated_projected_hole_vertices = rotation_hull_polygon(projected_hole_vertex, range3.alpha_interval<Interval>(), config_.rotation_resolution);
                for(const Vector2<Interval>& rotated_projected_hole_vertex: rotated_projected_hole_vertices) {
                    all_projected_hole_vertices.push_back(rotated_projected_hole_vertex);
                    if(config_.debug) {
                        debug_exporter_.debug_builder.box_builder.projection_builder.add_vertex(rotated_projected_hole_vertex);
                    }
                }
            }
            if(config_.debug) {
                debug_exporter_.debug_builder.box_builder.add_projection();
            }
        }
        return convex_hull(all_projected_hole_vertices);
    }

    bool is_range2_ignored(const Range3& range3, const Range2& range2) {
        const Interval box_angle_radius = Vector2<Interval>(Interval(range3.theta_interval<Interval>().len()) + Interval(range3.alpha_interval<Interval>().len()), Interval(range3.phi_interval<Interval>().len())).len() / Interval(2);
        const Interval range2_angle_radius = Vector2<Interval>(Interval(range2.theta_interval<Interval>().len()), Interval(range2.phi_interval<Interval>().len())).len() / Interval(2);
        const Interval remaining_angle = config_.epsilon - box_angle_radius - range2_angle_radius;
        if(!remaining_angle.is_positive()) {
            return false;
        }
        const Interval cos_remaining_angle = remaining_angle.cos();
        const Matrix<Interval> hole_matrix = Matrix<Interval>::projection_rotation_matrix(Interval(range3.theta_interval<Interval>().mid()), Interval(range3.phi_interval<Interval>().mid()), Interval(range3.alpha_interval<Interval>().mid()));
        const Matrix<Interval> plug_matrix = Matrix<Interval>::projection_matrix(Interval(range2.theta_interval<Interval>().mid()), Interval(range2.phi_interval<Interval>().mid()));
        for(const Matrix<Interval>& rotation: config_.polyhedron.rotations()) {
            if(Matrix<Interval>::cos_angle_between(hole_matrix * rotation, plug_matrix) > cos_remaining_angle) {
                return true;
            }
        }
        for(const Matrix<Interval>& reflection: config_.polyhedron.reflections()) {
            if(Matrix<Interval>::cos_angle_between(hole_matrix * reflection, Matrix<Interval>::reflect_z() * plug_matrix) > cos_remaining_angle) {
                return true;
            }
        }
        return false;
    }

    bool is_range2_terminal(const Range2& range2, const Polygon<Interval>& projected_hole) {
        const Interval theta(range2.theta_interval<Interval>());
        const Interval phi(range2.phi_interval<Interval>());
        bool is_terminal = false;
        for(size_t vertex_index = 0; vertex_index < config_.polyhedron.vertices().size(); vertex_index++) {
            const Vector3<Interval>& plug_vertex = config_.polyhedron.vertices()[vertex_index];
            if(config_.debug) {
                for(const Vector2<Interval>& projected_plug_vertex: projection_hull_polygon(plug_vertex, theta, phi, config_.projection_resolution)) {
                    debug_exporter_.debug_builder.box_builder.rectangle_builder.projection_builder.add_vertex(projected_plug_vertex);
                }
                debug_exporter_.debug_builder.box_builder.rectangle_builder.add_projection();
            }
            if(is_projected_vertex_outside_polygon_advanced(projected_hole, plug_vertex, theta, phi)) {
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

    bool is_box_nonterminal(const Range2& range2, const Polygon<Interval>& projected_hole) {
        const Interval theta_mid(range2.theta_interval<Interval>().mid());
        const Interval phi_mid(range2.phi_interval<Interval>().mid());
        return std::ranges::all_of(config_.polyhedron.vertices(), [&](const Vector3<Interval>& plug_vertex) {
            return is_projected_vertex_inside_polygon_trivial(projected_hole, plug_vertex, theta_mid, phi_mid);
        });
    }

    std::optional<TerminalBox> get_optional_terminal_box(const Range3& range3) {
        Polygon<Interval> projected_hole = get_projected_hole(range3);
        if(config_.debug) {
            debug_exporter_.debug_builder.box_builder.set_projection(projected_hole);
        }
        SerialQueue<Range2> range2_queue;
        range2_queue.push(Range2(Range(0, 0), Range(1, 0)));
        std::vector<Range2> range2s;
        for(uint32_t iteration = 0; config_.range2_iteration_limit == 0 || iteration < config_.range2_iteration_limit; iteration++) {
            const std::optional<Range2> optional_range2 = range2_queue.pop();
            if(!optional_range2.has_value()) {
                return std::make_optional(TerminalBox(range3, range2s));
            }

            const Range2& range2 = optional_range2.value();
            if(range2.theta_range().depth() >= 15 || range2.phi_range().depth() >= 15) {
                return std::nullopt;
            }
            if(config_.debug) {
                debug_exporter_.debug_builder.box_builder.rectangle_builder.set_rectangle(range2);
            }
            if(is_range2_ignored(range3, range2) || is_range2_terminal(range2, projected_hole)) {
                if(config_.debug) {
                    debug_exporter_.debug_builder.box_builder.add_rectangle();
                }
                range2s.push_back(range2);
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
                range2_queue.push(rectangle_part);
            }
        }
        return std::nullopt;
    }

    bool process() {
        const std::optional<Range3> optional_box = box_queue_.pop();
        if(!optional_box.has_value()) {
            return false;
        }
        const Range3& range3 = optional_box.value();
        if(range3.theta_range().depth() >= 15 || range3.phi_range().depth() >= 15 || range3.alpha_range().depth() >= 15) {
            return false;
        }
        const std::optional<TerminalBox> optional_terminal_box = get_optional_terminal_box(range3);
        if(config_.debug) {
            debug_exporter_.debug_builder.box_builder.set_box(range3);
            debug_exporter_.debug_builder.box_builder.set_terminal(optional_terminal_box.has_value());
            debug_exporter_.debug_builder.add_box();
        }
        if(optional_terminal_box.has_value()) {
            // std::cout << "Terminal Box: " << box << std::endl; // TODO: enable
            terminal_box_queue_.push(optional_terminal_box.value());
        } else {
            // std::cout << "Non Terminal Box: " << box << std::endl; // TODO: enable
            for(const Range3& box_part: range3.parts()) {
                box_queue_.push(box_part);
            }
        }
        return true;
    }

    void start_box_processor() {
        while(!terminated_) {
            if(process()) {
                if(++processed_box_count_ >= config_.range3_iteration_limit) {
                    stop();
                }
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }

        ++terminated_thread_count_;

        mpfr_free_cache2(MPFR_FREE_LOCAL_CACHE);
    }

    void start_exporter() {
        while(!terminated_) {
            if(terminal_box_queue_.size() < config_.export_size_threshold) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }
            Exporter::export_terminal_boxes(config_.working_directory() / terminal_boxes_file_name, terminal_box_queue_.pop_all());
        }
        while(terminated_thread_count_ < config_.thread_count) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        Exporter::export_terminal_boxes(config_.working_directory() / terminal_boxes_file_name, terminal_box_queue_.pop_all());
        Exporter::export_boxes(config_.working_directory() / boxes_file_name, box_queue_.pop_all());

        if(config_.debug) {
            debug_exporter_.export_debug(config_.working_directory() / debug_file_name);
        }

        mpfr_free_cache();
    }

public:
    explicit GlobalSolver(const Config<Interval>& config) : config_(config),
                                                            box_queue_(),
                                                            terminal_box_queue_(),
                                                            debug_exporter_(),
                                                            processor_threads_(),
                                                            exporter_thread_(),
                                                            processed_box_count_(0),
                                                            terminated_(false),
                                                            terminated_thread_count_(0) {
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
            box_queue_.push(Range3(
                Range(9, 0b011011010),
                Range(10, 0b1011001001),
                Range(9, 0b101101110)
            ));
        } else {
            box_queue_.push_all(boxes);
        }
    }

    void start() {
        std::ranges::generate_n(std::back_inserter(processor_threads_), config_.thread_count, [this] {
            return std::thread([this] {
                start_box_processor();
            });
        });
        exporter_thread_ = std::thread([this] {
            start_exporter();
        });

        for(std::thread& processor_thread: processor_threads_) {
            processor_thread.join();
        }
        exporter_thread_.join();
    }

    void stop() {
        terminated_ = true;
    }
};
