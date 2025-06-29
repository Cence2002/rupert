#pragma once

#include "pipeline/debug_exporter.hpp"
#include "pipeline/terminal_box.hpp"

template<IntervalType Interval>
struct BoxProcessor {
private:
    const Config<Interval>& config_;
    DebugExporter<Interval>& debug_exporter_;
    ConcurrentQueue<Range3>& range3_queue_;
    ConcurrentQueue<TerminalBox>& terminal_box_queue_;

    Polygon<Interval> get_projected_hole(const Range3& box) {
        std::vector<Vector2<Interval>> all_projected_hole_vertices;
        for(const Vector3<Interval>& hole_vertex: config_.hole().vertices()) {
            const std::vector<Vector2<Interval>> projected_hole_vertices = projection_hull_polygon(hole_vertex, box.theta_interval<Interval>(), box.phi_interval<Interval>(), config_.projection_resolution());
            for(const Vector2<Interval>& projected_hole_vertex: projected_hole_vertices) {
                const std::vector<Vector2<Interval>> rotated_projected_hole_vertices = rotation_hull_polygon(projected_hole_vertex, box.alpha_interval<Interval>(), config_.rotation_resolution());
                for(const Vector2<Interval>& rotated_projected_hole_vertex: rotated_projected_hole_vertices) {
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

    bool is_range2_ignored(const Range3& box, const Range2& range2) {
        const Interval box_angle_radius = Vector2<Interval>(Interval(box.theta_interval<Interval>().len()) + Interval(box.alpha_interval<Interval>().len()), Interval(box.phi_interval<Interval>().len())).len() / Interval(2);
        const Interval range2_angle_radius = Vector2<Interval>(Interval(range2.theta_interval<Interval>().len()), Interval(range2.phi_interval<Interval>().len())).len() / Interval(2);
        const Interval remaining_angle = config_.epsilon() - box_angle_radius - range2_angle_radius;
        if(!remaining_angle.is_positive()) {
            return false;
        }
        const Interval cos_remaining_angle = remaining_angle.cos();
        const Matrix<Interval> hole_matrix = projection_rotation_matrix(Interval(box.theta_interval<Interval>().mid()), Interval(box.phi_interval<Interval>().mid()), Interval(box.alpha_interval<Interval>().mid()));
        const Matrix<Interval> plug_matrix = projection_matrix(Interval(range2.theta_interval<Interval>().mid()), Interval(range2.phi_interval<Interval>().mid()));
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

    bool is_range2_terminal(const Range2& range2, const Polygon<Interval>& projected_hole) {
        const Interval theta(range2.theta_interval<Interval>());
        const Interval phi(range2.phi_interval<Interval>());
        bool is_terminal = false;
        for(size_t vertex_index = 0; vertex_index < config_.plug().vertices().size(); vertex_index++) {
            const Vector3<Interval>& plug_vertex = config_.plug().vertices()[vertex_index];
            if(config_.debug_enabled()) {
                for(const Vector2<Interval>& projected_plug_vertex: projection_hull_polygon(plug_vertex, theta, phi, config_.projection_resolution())) {
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

    bool is_box_nonterminal(const Range2& range2, const Polygon<Interval>& projected_hole) {
        const Interval theta_mid(range2.theta_interval<Interval>().mid());
        const Interval phi_mid(range2.phi_interval<Interval>().mid());
        return std::ranges::all_of(config_.plug().vertices(), [&](const Vector3<Interval>& plug_vertex) {
            return is_projected_vertex_inside_polygon_trivial(projected_hole, plug_vertex, theta_mid, phi_mid);
        });
    }

    std::optional<TerminalBox> get_optional_terminal_box(const Range3& box) {
        Polygon<Interval> projected_hole = get_projected_hole(box);
        if(config_.debug_enabled()) {
            debug_exporter_.debug_builder.box_builder.set_projection(projected_hole);
        }
        Queue<Range2> range2_queue;
        range2_queue.push(Range2());
        std::vector<Range2> range2s;
        for(uint32_t iteration = 0; config_.range2_iteration_limit() == 0 || iteration < config_.range2_iteration_limit(); iteration++) {
            const std::optional<Range2> optional_range2 = range2_queue.pop();
            if(!optional_range2.has_value()) {
                return std::make_optional(TerminalBox(box, range2s));
            }

            const Range2& range2 = optional_range2.value();
            if(!range2.is_valid()) {
                return std::nullopt;
            }
            if(config_.debug_enabled()) {
                debug_exporter_.debug_builder.box_builder.rectangle_builder.set_rectangle(range2);
            }
            if(is_range2_ignored(box, range2) || is_range2_terminal(range2, projected_hole)) {
                if(config_.debug_enabled()) {
                    debug_exporter_.debug_builder.box_builder.add_rectangle();
                }
                range2s.push_back(range2);
                continue;
            }
            if(is_box_nonterminal(range2, projected_hole)) {
                if(config_.debug_enabled()) {
                    debug_exporter_.debug_builder.box_builder.add_rectangle();
                    debug_exporter_.debug_builder.box_builder.set_last_as_in_index();
                }
                return std::nullopt;
            }
            if(config_.debug_enabled()) {
                debug_exporter_.debug_builder.box_builder.add_rectangle();
            }
            for(const Range2& rectangle_part: range2.parts()) {
                range2_queue.push(rectangle_part);
            }
        }
        return std::nullopt;
    }

public:
    explicit BoxProcessor(
        const Config<Interval>& config,
        DebugExporter<Interval>& debug_exporter,
        ConcurrentQueue<Range3>& range3_queue,
        ConcurrentQueue<TerminalBox>& terminal_box_queue
    ) : config_(config),
        debug_exporter_(debug_exporter),
        range3_queue_(range3_queue),
        terminal_box_queue_(terminal_box_queue) {}

    bool process() {
        const std::optional<Range3> optional_box = range3_queue_.pop();
        if(!optional_box.has_value()) {
            return false;
        }
        const Range3& box = optional_box.value();
        if(!box.is_valid()) {
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
            for(const Range3& box_part: box.parts()) {
                range3_queue_.push(box_part);
            }
        }
        return true;
    }
};
