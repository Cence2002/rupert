#include "geometry/geometry.hpp"
#include "cover.hpp"
#include "export.hpp"
#include "test.hpp"
#include <thread>

Exporter exporter;

template<IntervalType Interval>
bool is_rectangle_terminal(const Rectangle& rectangle, const Polygon<Interval>& hole, const Polyhedron<Interval>& plug) {
    const Interval& theta = rectangle.theta<Interval>();
    const Interval& phi = rectangle.phi<Interval>();
    bool is_terminal = false;
    for(size_t vertex_index = 0; vertex_index < plug.vertices().size(); vertex_index++) {
        for(const Vector<Interval>& projected_plug_vertex: projection_hull_advanced_approximate(plug.vertices()[vertex_index], theta, phi)) {
            exporter.cover_builder.box_builder.rectangle_builder.projection_builder.add_vertex(projected_plug_vertex);
        }
        exporter.cover_builder.box_builder.rectangle_builder.add_projection(exporter.builder);
        if(hole.is_projected_vertex_outside_polygon_advanced(plug.vertices()[vertex_index], theta, phi)) {
            exporter.cover_builder.box_builder.rectangle_builder.add_last_as_out();
            is_terminal = true;
        }
    }
    return is_terminal;
}

template<IntervalType Interval>
bool is_box_nonterminal(const Rectangle& rectangle, const Polygon<Interval>& hole, const Polyhedron<Interval>& plug) {
    const Interval& theta_mid = Interval(rectangle.theta<Interval>().mid());
    const Interval& phi_mid = Interval(rectangle.phi<Interval>().mid());
    return std::ranges::all_of(plug.vertices(), [&](const Vertex<Interval>& plug_vertex) {
        return hole.is_projected_vertex_inside_polygon_trivial(plug_vertex, theta_mid, phi_mid);
    });
}

template<IntervalType Interval>
std::vector<Interval> split(const Interval& interval, const int parts) {
    std::vector<Interval> sub_intervals;
    for(int part_index = 0; part_index < parts; part_index++) {
        const Interval sub_interval = interval.min() + Interval(part_index, part_index + 1) / parts * interval.len();
        sub_intervals.push_back(sub_interval);
    }
    return sub_intervals;
}

template<IntervalType Interval>
Polygon<Interval> project_hole(const Box& box, const Polyhedron<Interval>& hole, const int projection_resolution, const int rotation_resolution) {
    std::vector<Vector<Interval>> all_projected_hole_vertices;
    for(const Vertex<Interval>& hole_vertex: hole.vertices()) {
        const std::vector<Vector<Interval>> projected_hole_vertices = projection_hull_polygon(hole_vertex, box.theta<Interval>(), box.phi<Interval>(), projection_resolution);
        for(const Vector<Interval>& projected_hole_vertex: projected_hole_vertices) {
            const std::vector<Vector<Interval>> rotated_projected_hole_vertices = rotation_hull_polygon(projected_hole_vertex, box.alpha<Interval>(), rotation_resolution);
            for(const Vector<Interval>& rotated_projected_hole_vertex: rotated_projected_hole_vertices) {
                all_projected_hole_vertices.push_back(rotated_projected_hole_vertex);
                exporter.cover_builder.box_builder.projection_builder.add_vertex(rotated_projected_hole_vertex);
            }
        }
        exporter.cover_builder.box_builder.add_projection(exporter.builder);
    }
    const Interval alpha_step = Interval(box.alpha<Interval>().rad()) / rotation_resolution;
    const Interval alpha_epsilon = 1 / alpha_step.cos() - 1;
    const typename Interval::Number epsilon = (alpha_epsilon / 16).max();
    return convex_hull(all_projected_hole_vertices, epsilon);
}

template<IntervalType Interval>
bool is_box_terminal(const Box& box, const Polyhedron<Interval>& hole, const Polyhedron<Interval>& plug, const int iterations2, const int projection_resolution, const int rotation_resolution) {
    Polygon<Interval> projected_hole = project_hole(box, hole, projection_resolution, rotation_resolution);
    exporter.cover_builder.box_builder.set_projection(exporter.builder, projected_hole);
    Queue2 queue2;
    for(int iteration2 = 0; iterations2 == 0 || iteration2 < iterations2; iteration2++) {
        const std::optional<Rectangle> optional_rectangle = queue2.pop();
        if(!optional_rectangle.has_value()) {
            exporter.cover_builder.box_builder.set_complete(true);
            return true;
        }

        const Rectangle& rectangle = optional_rectangle.value();
        if(rectangle.is_overflow()) {
            return false;
        }
        exporter.cover_builder.box_builder.rectangle_builder.set_rectangle(rectangle);
        if(is_rectangle_terminal(rectangle, projected_hole, plug)) {
            exporter.cover_builder.box_builder.add_rectangle(exporter.builder);
            continue;
        }
        if(is_box_nonterminal(rectangle, projected_hole, plug)) {
            exporter.cover_builder.box_builder.add_rectangle(exporter.builder);
            exporter.cover_builder.box_builder.save_in();
            return false;
        }
        exporter.cover_builder.box_builder.add_rectangle(exporter.builder);
        for(const Rectangle& sub_rectangle: rectangle.split()) {
            queue2.push(sub_rectangle);
        }
    }
    return false;
}

template<IntervalType Interval>
void step(Queue3& queue3, const Polyhedron<Interval>& hole, const Polyhedron<Interval>& plug, const int iterations2, const int projection_resolution, const int rotation_resolution) {
    const std::optional<Box> optional_box = queue3.pop();
    if(!optional_box.has_value()) {
        return;
    }
    const Box& box = optional_box.value();
    if(box.is_overflow()) {
        return;
    }
    exporter.cover_builder.box_builder.set_box(box);
    const bool is_terminal = is_box_terminal(box, hole, plug, iterations2, projection_resolution, rotation_resolution);
    exporter.cover_builder.box_builder.set_complete(is_terminal);
    if(!is_terminal) {
        for(const Box& sub_box: box.split()) {
            queue3.push(sub_box);
        }
    }
    if(is_terminal) {
        print("Terminal Box: ", box);
    } else {
        print("Nonterminal Box: ", box);
    }
}

template<IntervalType Interval>
void solve(const Polyhedron<Interval>& hole, const Polyhedron<Interval>& plug, const int num_threads, const int iterations3, const int iterations2, const int projection_resolution, const int rotation_resolution) {
    Queue3 queue3;

    std::vector<std::thread> threads;
    const int iterations3_per_thread = iterations3 / num_threads;
    for(int thread = 0; thread < num_threads; thread++) {
        threads.emplace_back([&] {
            for(int iteration3 = 0; iterations3_per_thread == 0 || iteration3 < iterations3_per_thread; iteration3++) {
                step(queue3, hole, plug, iterations2, projection_resolution, rotation_resolution);
                exporter.cover_builder.add_box(exporter.builder);
            }
            mpfr_free_cache2(MPFR_FREE_LOCAL_CACHE);
        });
    }
    for(std::thread& thread: threads) {
        thread.join();
    }
    mpfr_free_cache();
}

int main() {
    tests();

    using Interval = FloatInterval;

    const Polyhedron<Interval> hole = Polyhedron<Interval>::rhombicosidodecahedron();
    const Polyhedron<Interval> plug = Polyhedron<Interval>::rhombicosidodecahedron();

    exporter.cover_builder.set_description("Exported Cover");
    exporter.cover_builder.set_hole(exporter.builder, hole);
    exporter.cover_builder.set_plug(exporter.builder, plug);

    solve<Interval>(hole, plug, 1, 64, 10000, 2, 2);

    exporter.save("../../web/static/cover.bin");

    return 0;
}
