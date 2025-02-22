#include "geometry/geometry.hpp"
#include "cover.hpp"
#include "test.hpp"
#include "export.hpp"
#include <thread>

Exporter exporter;

template<IntervalType Interval>
bool is_box2_terminal(const Box2& box2, const Polygon<Interval>& hole, const Polyhedron<Interval>& plug) {
    // return std::ranges::any_of(plug.vertices(), [&](const Vector3<Interval>& plug_vertex) {
    //     const Vector2<Interval> projected_plug_vertex = plug_vertex.project(Interval(box2.phi<Interval>()), Interval(box2.theta<Interval>()));
    //     return hole.is_outside(projected_plug_vertex);
    // });
    bool is_terminal = false;
    for(int vertex_index = 0; vertex_index < plug.vertices().size(); vertex_index++) {
        const IntervalVector2<Interval> projected_plug_vertex = projection(plug.vertices()[vertex_index], Interval(box2.phi<Interval>()), Interval(box2.theta<Interval>()));
        for(const IntervalVector2<Interval>& projected_plug_vertex: projection_hull_trivial(plug.vertices()[vertex_index], box2.phi<Interval>(), box2.theta<Interval>())) {
            exporter.cover_builder.box3_builder.box2_builder.projection_builder.add_vertex(projected_plug_vertex);
        }
        exporter.cover_builder.box3_builder.box2_builder.add_projection(exporter.builder);
        if(hole.is_outside(projected_plug_vertex)) {
            exporter.cover_builder.box3_builder.box2_builder.add_out(vertex_index);
            is_terminal = true;
        }
    }
    return is_terminal;
}

template<IntervalType Interval>
bool is_box3_nonterminal(const Box2& box2, const Polygon<Interval>& hole, const Polyhedron<Interval>& plug) {
    return std::ranges::all_of(plug.vertices(), [&](const IntervalVector3<Interval>& plug_vertex) {
        const IntervalVector2<Interval> projected_plug_vertex = projection(plug_vertex, Interval(box2.phi<Interval>().mid()), Interval(box2.theta<Interval>().mid()));
        return hole.is_inside(projected_plug_vertex);
    });
}

template<IntervalType Interval>
std::vector<Interval> split(const Interval& interval, const int parts) {
    std::vector<Interval> sub_intervals;
    for(int part_index = 0; part_index < parts; part_index++) {
        // TODO make valid (min rounds down, so we need smarter calculation of parts)
        const Interval sub_interval = interval.min() + Interval(part_index, part_index + 1) * interval.len() / parts;
        sub_intervals.push_back(sub_interval);
    }
    return sub_intervals;
}

template<IntervalType Interval>
Polygon<Interval> project_hole(const Box3& box3, const Polyhedron<Interval>& hole, const int resolution) {
    std::vector<IntervalVector2<Interval>> all_projected_hole_vertices;
    for(const IntervalVector3<Interval>& hole_vertex: hole.vertices()) {
        for(const Interval& sub_phi: split(box3.phi<Interval>(), resolution)) {
            for(const Interval& sub_theta: split(box3.theta<Interval>(), resolution)) {
                const std::vector<IntervalVector2<Interval>> projected_hole_vertices = projection_hull_trivial(hole_vertex, sub_phi, sub_theta);
                for(const IntervalVector2<Interval>& projected_hole_vertex: projected_hole_vertices) {
                    for(const Interval& sub_alpha: split(box3.alpha<Interval>(), resolution)) {
                        const std::vector<IntervalVector2<Interval>> rotated_projected_hole_vertices = rotation_hull_triangle(projected_hole_vertex, sub_alpha);
                        for(const IntervalVector2<Interval>& rotated_projected_hole_vertex: rotated_projected_hole_vertices) {
                            all_projected_hole_vertices.push_back(rotated_projected_hole_vertex);
                            exporter.cover_builder.box3_builder.projection_builder.add_vertex(rotated_projected_hole_vertex);
                        }
                    }
                }
            }
        }
        exporter.cover_builder.box3_builder.add_projection(exporter.builder);
    }
    return convex_hull(all_projected_hole_vertices);
}

template<IntervalType Interval>
bool is_box3_terminal(const Box3& box3, const Polyhedron<Interval>& hole, const Polyhedron<Interval>& plug,
                      const int iterations2, const int resolution) {
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
void solve(const Polyhedron<Interval>& hole, const Polyhedron<Interval>& plug,
           const int num_threads = 10, const int iterations3 = 1000, const int iterations2 = 10000, const int resolution = 5) {
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
    for(auto& thread: threads) {
        thread.join();
    }
    mpfr_free_cache();
}

int main() {
    tests();

    using Interval = FloatInterval;
    const Polyhedron<Interval> hole = Polyhedron<Interval>(std::vector<IntervalVector3<Interval>>(
        {
            IntervalVector3<Interval>(Interval(1), Interval(1), Interval(0)),
            IntervalVector3<Interval>(Interval(1), Interval(-1), Interval(0)),
            IntervalVector3<Interval>(Interval(-1), Interval(-1), Interval(0)),
            IntervalVector3<Interval>(Interval(-1), Interval(1), Interval(0)),
            IntervalVector3<Interval>(Interval(0), Interval(0), Interval(1) / 2),
            IntervalVector3<Interval>(Interval(0), Interval(0), -Interval(1) / 2),
        }
    ));

    const Polyhedron<Interval> plug = Polyhedron<Interval>::cube();

    exporter.cover_builder.set_description("Exported Data from C++");
    exporter.cover_builder.set_hole(exporter.builder, hole);
    exporter.cover_builder.set_plug(exporter.builder, plug);

    solve<Interval>(hole, plug, 1, 30, 10000, 3);

    exporter.save("../../web/static/test.bin");

    return 0;
}
