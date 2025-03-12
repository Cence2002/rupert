#pragma once

#include <util.hpp>

#include "interval/interval.hpp"
#include "config.hpp"
#include "box.hpp"
#include "box_queue.hpp"
#include "terminal_box_queue.hpp"
#include "debug_exporter.hpp"

template<IntervalType Interval>
struct BoxProcessor {
private:
    const Config<Interval>& config_;
    DebugExporter& debug_exporter_;
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
                    debug_exporter_.cover_builder.box_builder.projection_builder.add_vertex(rotated_projected_hole_vertex);
                }
            }
            debug_exporter_.cover_builder.box_builder.add_projection(debug_exporter_.builder);
        }
        const Interval alpha_step = Interval(box.alpha<Interval>().rad()) / config_.rotation_resolution();
        const Interval alpha_epsilon = 1 / alpha_step.cos() - 1;
        const typename Interval::Number epsilon = (alpha_epsilon / 16).max();
        return convex_hull(all_projected_hole_vertices, epsilon);
    }

    bool is_rectangle_terminal(const Rectangle& rectangle, const Polygon<Interval>& projected_hole) {
        const Interval theta(rectangle.theta<Interval>());
        const Interval phi(rectangle.phi<Interval>());
        bool is_terminal = false;
        for(size_t vertex_index = 0; vertex_index < config_.plug().vertices().size(); vertex_index++) {
            const Vertex<Interval>& plug_vertex = config_.plug().vertices()[vertex_index];
            for(const Vector<Interval>& projected_plug_vertex: projection_hull_advanced_approximate(plug_vertex, theta, phi)) {
                debug_exporter_.cover_builder.box_builder.rectangle_builder.projection_builder.add_vertex(projected_plug_vertex);
            }
            debug_exporter_.cover_builder.box_builder.rectangle_builder.add_projection(debug_exporter_.builder);
            if(projected_hole.is_projected_vertex_outside_polygon_advanced(plug_vertex, theta, phi)) {
                debug_exporter_.cover_builder.box_builder.rectangle_builder.add_last_as_out();
                is_terminal = true;
                break; // TODO: Make this optional in the config
            }
        }
        return is_terminal;
    }

    bool is_box_nonterminal(const Rectangle& rectangle, const Polygon<Interval>& projected_hole) {
        const Interval theta_mid(rectangle.theta<Interval>().mid());
        const Interval phi_mid(rectangle.phi<Interval>().mid());
        return std::ranges::all_of(config_.plug().vertices(), [&](const Vertex<Interval>& plug_vertex) {
            return projected_hole.is_projected_vertex_inside_polygon_trivial(plug_vertex, theta_mid, phi_mid);
        });
    }

    std::optional<TerminalBox> get_optional_terminal_box(const Box& box) {
        Polygon<Interval> projected_hole = get_projected_hole(box);
        debug_exporter_.cover_builder.box_builder.set_projection(debug_exporter_.builder, projected_hole);
        RectangleQueue rectangle_queue;
        std::vector<Rectangle> rectangles;
        for(uint32_t iteration = 0; config_.rectangle_iteration_limit() == 0 || iteration < config_.rectangle_iteration_limit(); iteration++) {
            const std::optional<Rectangle> optional_rectangle = rectangle_queue.pop();
            if(!optional_rectangle.has_value()) {
                debug_exporter_.cover_builder.box_builder.set_complete(true);
                return std::make_optional(TerminalBox(box, rectangles));
            }

            const Rectangle& rectangle = optional_rectangle.value();
            if(rectangle.is_invalid()) {
                return std::nullopt;
            }
            debug_exporter_.cover_builder.box_builder.rectangle_builder.set_rectangle(rectangle);
            if(is_rectangle_terminal(rectangle, projected_hole)) {
                debug_exporter_.cover_builder.box_builder.add_rectangle(debug_exporter_.builder);
                rectangles.push_back(rectangle);
                continue;
            }
            if(is_box_nonterminal(rectangle, projected_hole)) {
                debug_exporter_.cover_builder.box_builder.add_rectangle(debug_exporter_.builder);
                debug_exporter_.cover_builder.box_builder.save_in();
                return std::nullopt;
            }
            debug_exporter_.cover_builder.box_builder.add_rectangle(debug_exporter_.builder);
            for(const Rectangle& sub_rectangle: rectangle.subdivide()) {
                rectangle_queue.push(sub_rectangle);
            }
        }
        return std::nullopt;
    }

public:
    explicit BoxProcessor(
        const Config<Interval>& config,
        DebugExporter& debug_exporter,
        BoxQueue& box_queue,
        TerminalBoxQueue& terminal_box_queue
    ): config_(config),
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
        debug_exporter_.cover_builder.box_builder.set_box(box);
        const std::optional<TerminalBox> optional_terminal_box = get_optional_terminal_box(box);
        debug_exporter_.cover_builder.box_builder.set_complete(optional_terminal_box.has_value());
        if(optional_terminal_box.has_value()) {
            print("Terminal Box: ", box);
            terminal_box_queue_.push(optional_terminal_box.value());
        } else {
            // print("Nonterminal Box: ", box);
            for(const Box& sub_box: box.subdivide()) {
                box_queue_.push(sub_box);
            }
        }
        debug_exporter_.cover_builder.add_box(debug_exporter_.builder);
        return true;
    }
};
