#include "geometry/geometry.hpp"
#include "cover.hpp"
#include "test.hpp"
#include <thread>

template<IntervalType Interval>
bool is_box2_terminal(const Box2& box2, const Polyhedron<Interval>& plug, const Polygon<Interval>& hole) {
    return std::ranges::any_of(plug.vertices(), [&](const Vector3<Interval>& plug_vertex) {
        const Vector2<Interval> projected_plug_vertex = plug_vertex.project(Interval(box2.phi<Interval>().mid()), Interval(box2.theta<Interval>().mid()));
        return hole.is_outside(projected_plug_vertex);
    });
}

template<IntervalType Interval>
bool is_box3_nonterminal(const Box2& box2, const Polyhedron<Interval>& plug, const Polygon<Interval>& hole) {
    return std::ranges::all_of(plug.vertices(), [&](const Vector3<Interval>& plug_vertex) {
        const Vector2<Interval> projected_plug_vertex = plug_vertex.project(Interval(box2.phi<Interval>().mid()), Interval(box2.theta<Interval>().mid()));
        return hole.is_inside(projected_plug_vertex);
    });
}

template<IntervalType Interval>
std::vector<Interval> split(const Interval& interval, const int parts) {
    std::vector<Interval> sub_intervals;
    for(int i = 0; i < parts; i++) {
        const Interval sub_interval = Interval(i, i + 1) * interval.len() / parts + interval.min();
        sub_intervals.push_back(sub_interval);
    }
    return sub_intervals;
}

template<IntervalType Interval>
Polygon<Interval> project_hole(const Box3& box3, const Polyhedron<Interval>& hole, const int resolution) {
    std::vector<Vector2<Interval>> projected_hole_vertices;
    for(const Interval& sub_phi: split(box3.phi<Interval>(), resolution)) {
        for(const Interval& sub_theta: split(box3.theta<Interval>(), resolution)) {
            for(const Interval& sub_alpha: split(box3.alpha<Interval>(), resolution)) {
                for(const Vector3<Interval>& hole_vertex: hole.vertices()) {
                    const Vector2<Interval> projected_hole_vertex = hole_vertex.project(sub_phi, sub_theta).rotate(sub_alpha);
                    projected_hole_vertices.emplace_back(Interval(projected_hole_vertex.x().min()), Interval(projected_hole_vertex.y().min()));
                    projected_hole_vertices.emplace_back(Interval(projected_hole_vertex.x().min()), Interval(projected_hole_vertex.y().max()));
                    projected_hole_vertices.emplace_back(Interval(projected_hole_vertex.x().max()), Interval(projected_hole_vertex.y().min()));
                    projected_hole_vertices.emplace_back(Interval(projected_hole_vertex.x().max()), Interval(projected_hole_vertex.y().max()));
                }
            }
        }
    }
    return convex_hull(projected_hole_vertices);
}

template<IntervalType Interval>
bool is_box3_terminal(const Box3& box3, const Polyhedron<Interval>& plug, const Polyhedron<Interval>& hole,
                      const int iterations, const int resolution) {
    Polygon<Interval> projected_hole = project_hole(box3, hole, resolution);
    Queue2 queue2;
    for(int t = 0; t < iterations; t++) {
        const std::optional<Box2> optional_box2 = queue2.pop();
        if(!optional_box2.has_value()) {
            return true;
        }
        const Box2& box2 = optional_box2.value();
        if(is_box3_nonterminal(box2, plug, projected_hole)) {
            return false;
        }
        if(is_box2_terminal(box2, plug, projected_hole)) {
            continue;
        }
        for(const Box2& sub_box2: box2.split()) {
            queue2.push(sub_box2);
        }
    }
    return queue2.empty();
}

template<IntervalType Interval>
void step(Queue3& queue3, const Polyhedron<Interval>& plug, const Polyhedron<Interval>& hole,
          const int iterations, const int resolution) {
    const std::optional<Box3> optional_box3 = queue3.pop();
    if(!optional_box3.has_value()) {
        return;
    }
    const Box3& box3 = optional_box3.value();
    const bool is_terminal = is_box3_terminal(box3, plug, hole, iterations, resolution);
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
void solve(const int num_threads = 10, const int iterations = 10000, const int resolution = 5) {
    const Polyhedron<Interval> plug = Polyhedron<Interval>::cube();
    const Polyhedron<Interval> hole = Polyhedron<Interval>::cube();

    Queue3 queue3;

    std::vector<std::thread> threads;
    for(int t = 0; t < num_threads; t++) {
        threads.emplace_back([&] {
            while(true) {
                step(queue3, plug, hole, iterations, resolution);
                break;
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
    solve<FloatInterval>(10, 2, 2);
    return 0;
}
