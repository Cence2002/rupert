#pragma once

#include "pipeline/debug_exporter.hpp"

template<IntervalType Interval>
struct BoxProcessor {
private:
    const Config<Interval>& config_;
    DebugExporter<Interval>& debug_exporter_;
    BoxQueue& box_queue_;
    TerminalBoxQueue& terminal_box_queue_;

    Polygon<Interval> get_projected_hole(const Box& box) {
        std::vector<Vector<Interval>> all_projected_hole_vertices;
        for(const Vertex<Interval>& hole_vertex: config_.hole().vertices()) {
            const std::vector<Vector<Interval>> projected_hole_vertices = projection_hull_polygon(hole_vertex, box.theta<Interval>(), box.phi<Interval>(), config_.projection_resolution());
            for(const Vector<Interval>& projected_hole_vertex: projected_hole_vertices) {
                const std::vector<Vector<Interval>> rotated_projected_hole_vertices = rotation_hull_polygon(projected_hole_vertex, box.alpha<Interval>(), config_.rotation_resolution());
                for(const Vector<Interval>& rotated_projected_hole_vertex: rotated_projected_hole_vertices) {
                    all_projected_hole_vertices.push_back(rotated_projected_hole_vertex);
                    if(config_.debug_enabled()) {
                        debug_exporter_.debug_builder.box_builder.projection_builder.add_vertex(rotated_projected_hole_vertex);
                    }
                }
            }
            if(config_.debug_enabled()) {
                debug_exporter_.debug_builder.box_builder.add_projection();
            }
        }
        return convex_hull(all_projected_hole_vertices);
    }

    bool is_rectangle_ignored(const Box& box, const Rectangle& rectangle) {
        const Interval box_angle_radius = Vector<Interval>(Interval(box.theta<Interval>().len()) + Interval(box.alpha<Interval>().len()), Interval(box.phi<Interval>().len())).len() / Interval(2);
        const Interval rectangle_angle_radius = Vector<Interval>(Interval(rectangle.theta<Interval>().len()), Interval(rectangle.phi<Interval>().len())).len() / Interval(2);
        const Interval remaining_angle = config_.epsilon() - box_angle_radius - rectangle_angle_radius;
        if(!remaining_angle.is_positive()) {
            return false;
        }
        const Interval cos_remaining_angle = remaining_angle.cos();
        const Matrix<Interval> hole_matrix = projection_rotation_matrix(Interval(box.theta<Interval>().mid()), Interval(box.phi<Interval>().mid()), Interval(box.alpha<Interval>().mid()));
        const Matrix<Interval> plug_matrix = projection_matrix(Interval(rectangle.theta<Interval>().mid()), Interval(rectangle.phi<Interval>().mid()));
        for(const Matrix<Interval>& rotation: config_.plug().rotations()) {
            if(Matrix<Interval>::cos_angle_between(hole_matrix * rotation, plug_matrix) > cos_remaining_angle) {
                return true;
            }
        }
        for(const Matrix<Interval>& reflection: config_.plug().reflections()) {
            if(Matrix<Interval>::cos_angle_between(hole_matrix * reflection, Matrix<Interval>::reflect_z() * plug_matrix) > cos_remaining_angle) {
                return true;
            }
        }
        return false;
    }

    bool is_rectangle_terminal(const Rectangle& rectangle, const Polygon<Interval>& projected_hole) {
        const Interval theta(rectangle.theta<Interval>());
        const Interval phi(rectangle.phi<Interval>());
        bool is_terminal = false;
        for(size_t vertex_index = 0; vertex_index < config_.plug().vertices().size(); vertex_index++) {
            const Vertex<Interval>& plug_vertex = config_.plug().vertices()[vertex_index];
            if(config_.debug_enabled()) {
                for(const Vector<Interval>& projected_plug_vertex: projection_hull_polygon(plug_vertex, theta, phi, config_.projection_resolution())) {
                    debug_exporter_.debug_builder.box_builder.rectangle_builder.projection_builder.add_vertex(projected_plug_vertex);
                }
                debug_exporter_.debug_builder.box_builder.rectangle_builder.add_projection();
            }
            if(is_projected_vertex_outside_polygon_advanced(projected_hole, plug_vertex, theta, phi)) {
                if(config_.debug_enabled()) {
                    debug_exporter_.debug_builder.box_builder.rectangle_builder.add_last_as_out_index();
                }
                is_terminal = true;
                if(!config_.debug_enabled()) {
                    break;
                }
            }
        }
        return is_terminal;
    }

    bool is_box_nonterminal(const Rectangle& rectangle, const Polygon<Interval>& projected_hole) {
        const Interval theta_mid(rectangle.theta<Interval>().mid());
        const Interval phi_mid(rectangle.phi<Interval>().mid());
        return std::ranges::all_of(config_.plug().vertices(), [&](const Vertex<Interval>& plug_vertex) {
            return is_projected_vertex_inside_polygon_trivial(projected_hole, plug_vertex, theta_mid, phi_mid);
        });
    }

    std::optional<TerminalBox> get_optional_terminal_box(const Box& box) {
        Polygon<Interval> projected_hole = get_projected_hole(box);
        if(config_.debug_enabled()) {
            debug_exporter_.debug_builder.box_builder.set_projection(projected_hole);
        }
        RectangleQueue rectangle_queue;
        std::vector<Rectangle> rectangles;
        for(uint32_t iteration = 0; config_.rectangle_iteration_limit() == 0 || iteration < config_.rectangle_iteration_limit(); iteration++) {
            const std::optional<Rectangle> optional_rectangle = rectangle_queue.pop();
            if(!optional_rectangle.has_value()) {
                return std::make_optional(TerminalBox(box, rectangles));
            }

            const Rectangle& rectangle = optional_rectangle.value();
            if(rectangle.is_invalid()) {
                return std::nullopt;
            }
            if(config_.debug_enabled()) {
                debug_exporter_.debug_builder.box_builder.rectangle_builder.set_rectangle(rectangle);
            }
            if(is_rectangle_ignored(box, rectangle) || is_rectangle_terminal(rectangle, projected_hole)) {
                if(config_.debug_enabled()) {
                    debug_exporter_.debug_builder.box_builder.add_rectangle();
                }
                rectangles.push_back(rectangle);
                continue;
            }
            if(is_box_nonterminal(rectangle, projected_hole)) {
                if(config_.debug_enabled()) {
                    debug_exporter_.debug_builder.box_builder.add_rectangle();
                    debug_exporter_.debug_builder.box_builder.set_last_as_in_index();
                }
                return std::nullopt;
            }
            if(config_.debug_enabled()) {
                debug_exporter_.debug_builder.box_builder.add_rectangle();
            }
            for(const Rectangle& rectangle_part: rectangle.parts()) {
                rectangle_queue.push(rectangle_part);
            }
        }
        return std::nullopt;
    }

public:
    explicit BoxProcessor(
        const Config<Interval>& config,
        DebugExporter<Interval>& debug_exporter,
        BoxQueue& box_queue,
        TerminalBoxQueue& terminal_box_queue
    ) : config_(config),
        debug_exporter_(debug_exporter),
        box_queue_(box_queue),
        terminal_box_queue_(terminal_box_queue) {}

    bool process() {
        const std::optional<Box> optional_box = box_queue_.pop();
        if(!optional_box.has_value()) {
            return false;
        }
        const Box& box = optional_box.value();
        if(box.is_invalid()) {
            return false;
        }
        const std::optional<TerminalBox> optional_terminal_box = get_optional_terminal_box(box);
        if(config_.debug_enabled()) {
            debug_exporter_.debug_builder.box_builder.set_box(box);
            debug_exporter_.debug_builder.box_builder.set_terminal(optional_terminal_box.has_value());
            debug_exporter_.debug_builder.add_box();
        }
        if(optional_terminal_box.has_value()) {
            // std::cout << "Terminal Box: " << box << std::endl; // TODO: enable
            terminal_box_queue_.push(optional_terminal_box.value());
        } else {
            // std::cout << "Non Terminal Box: " << box << std::endl; // TODO: enable
            for(const Box& box_part: box.parts()) {
                box_queue_.push(box_part);
            }
        }
        return true;
    }
};
