#include "geometry/geometry.hpp"
#include "cover.hpp"
#include "test.hpp"
#include "export.hpp"
#include <thread>

Exporter exporter;

template<IntervalType Interval>
bool is_box2_terminal(const Box2& box2, const Polygon<Interval>& hole, const Polyhedron<Interval>& plug) {
    const Interval& theta = box2.theta<Interval>();
    const Interval& phi = box2.phi<Interval>();
    bool is_terminal = false;
    for(size_t vertex_index = 0; vertex_index < plug.vertices().size(); vertex_index++) {
        for(const Vector2<Interval>& projected_plug_vertex: projection_hull_combined(plug.vertices()[vertex_index], theta, phi)) {
            exporter.cover_builder.box3_builder.box2_builder.projection_builder.add_vertex(projected_plug_vertex);
        }
        exporter.cover_builder.box3_builder.box2_builder.add_projection(exporter.builder);
        if(is_projected_vertex_outside_polygon_advanced(plug.vertices()[vertex_index], theta, phi, hole)) {
            exporter.cover_builder.box3_builder.box2_builder.add_last_as_out();
            is_terminal = true;
        }
    }
    return is_terminal;
}

template<IntervalType Interval>
bool is_box3_nonterminal(const Box2& box2, const Polygon<Interval>& hole, const Polyhedron<Interval>& plug) {
    const Interval& theta_mid = Interval(box2.theta<Interval>().mid());
    const Interval& phi_mid = Interval(box2.phi<Interval>().mid());
    return std::ranges::all_of(plug.vertices(), [&](const Vector3<Interval>& plug_vertex) {
        return is_projected_vertex_inside_polygon_trivial(plug_vertex, theta_mid, phi_mid, hole);
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
Polygon<Interval> project_hole(const Box3& box3, const Polyhedron<Interval>& hole, const int resolution) {
    std::vector<Vector2<Interval>> all_projected_hole_vertices;
    for(const Vector3<Interval>& hole_vertex: hole.vertices()) {
        for(const Interval& sub_theta: split(box3.theta<Interval>(), resolution)) {
            for(const Interval& sub_phi: split(box3.phi<Interval>(), resolution)) {
                const std::vector<Vector2<Interval>> projected_hole_vertices = projection_hull_triangle(hole_vertex, sub_theta, sub_phi);
                for(const Vector2<Interval>& projected_hole_vertex: projected_hole_vertices) {
                    for(const Interval& sub_alpha: split(box3.alpha<Interval>(), resolution)) {
                        const std::vector<Vector2<Interval>> rotated_projected_hole_vertices = rotation_hull_triangle(projected_hole_vertex, sub_alpha);
                        for(const Vector2<Interval>& rotated_projected_hole_vertex: rotated_projected_hole_vertices) {
                            all_projected_hole_vertices.push_back(rotated_projected_hole_vertex);
                            exporter.cover_builder.box3_builder.projection_builder.add_vertex(rotated_projected_hole_vertex);
                        }
                    }
                }
            }
        }
        exporter.cover_builder.box3_builder.add_projection(exporter.builder);
    }
    typename Interval::Number epsilon = (1 / (Interval(box3.alpha<Interval>().rad()) / 100).cos()).max();
    return convex_hull(all_projected_hole_vertices, epsilon);
}

template<IntervalType Interval>
bool is_box3_terminal(const Box3& box3, const Polyhedron<Interval>& hole, const Polyhedron<Interval>& plug, const int iterations2, const int resolution) {
    Polygon<Interval> projected_hole = project_hole(box3, hole, resolution);
    exporter.cover_builder.box3_builder.set_projection(exporter.builder, projected_hole);
    Queue2 queue2;
    for(int iteration2 = 0; iterations2 == 0 || iteration2 < iterations2; iteration2++) {
        const std::optional<Box2> optional_box2 = queue2.pop();
        if(!optional_box2.has_value()) {
            exporter.cover_builder.box3_builder.set_complete(true);
            return true;
        }

        const Box2& box2 = optional_box2.value();
        if(box2.is_overflow()) {
            return false;
        }
        exporter.cover_builder.box3_builder.box2_builder.set_box2(box2);
        if(is_box2_terminal(box2, projected_hole, plug)) {
            exporter.cover_builder.box3_builder.add_box2(exporter.builder);
            continue;
        }
        if(is_box3_nonterminal(box2, projected_hole, plug)) {
            exporter.cover_builder.box3_builder.add_box2(exporter.builder);
            exporter.cover_builder.box3_builder.save_in();
            return false;
        }
        exporter.cover_builder.box3_builder.add_box2(exporter.builder);
        for(const Box2& sub_box2: box2.split()) {
            queue2.push(sub_box2);
        }
    }
    return false;
}

template<IntervalType Interval>
void step(Queue3& queue3, const Polyhedron<Interval>& hole, const Polyhedron<Interval>& plug,
          const int iterations2, const int resolution) {
    const std::optional<Box3> optional_box3 = queue3.pop();
    if(!optional_box3.has_value()) {
        return;
    }
    const Box3& box3 = optional_box3.value();
    if(box3.is_overflow()) {
        return;
    }
    exporter.cover_builder.box3_builder.set_box3(box3);
    const bool is_terminal = is_box3_terminal(box3, hole, plug, iterations2, resolution);
    exporter.cover_builder.box3_builder.set_complete(is_terminal);
    if(!is_terminal) {
        for(const Box3& sub_box3: box3.split()) {
            queue3.push(sub_box3);
        }
    }
    if(is_terminal) {
        print("Terminal Box: ", box3);
    } else {
        print("Nonterminal Box: ", box3);
    }
}

template<IntervalType Interval>
void solve(const Polyhedron<Interval>& hole, const Polyhedron<Interval>& plug, const int num_threads, const int iterations3, const int iterations2, const int resolution) {
    Queue3 queue3;

    std::vector<std::thread> threads;
    for(int thread = 0; thread < num_threads; thread++) {
        threads.emplace_back([&] {
            for(int iteration3 = 0; iterations3 == 0 || iteration3 < iterations3; iteration3++) {
                step(queue3, hole, plug, iterations2, resolution);
                exporter.cover_builder.add_box3(exporter.builder);
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

    exporter.cover_builder.set_description("Exported Data from C++");
    exporter.cover_builder.set_hole(exporter.builder, hole);
    exporter.cover_builder.set_plug(exporter.builder, plug);

    solve<Interval>(hole, plug, 1, 64, 10000, 3);

    exporter.save("../../web/static/cover.bin");

    return 0;
}
