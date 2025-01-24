#include "util.hpp"
#include "interval.hpp"
#include "vector.hpp"
#include "cover.hpp"
#include "polyhedron.hpp"
#include "test.hpp"
#include <thread>

template<IntervalType I>
I to_interval(const Range &range) {
    return I(range.min, range.max);
}

template<IntervalType I>
bool is_box2_terminal(const Box2 &box2, const std::vector<Vector3<I>> &plug, const std::vector<Vector2<I>> &projected_hole) {
    const I theta = to_interval<BoostInterval>(box2.theta);
    const I phi = to_interval<BoostInterval>(box2.phi);
    for(const Vector3<I> &plug_vertex: plug) {
        const Vector2<I> projected_plug_vertex = plug_vertex.project(theta, phi);
        if(projected_plug_vertex.outside_advanced(projected_hole)) {
            return true;
        }
    }
    return false;
}

template<IntervalType I>
bool is_box3_nonterminal(const Box2 &box2, const std::vector<Vector3<I>> &plug, const std::vector<Vector2<I>> &projected_hole) {
    const I theta(box2.theta.mid());
    const I phi(box2.phi.mid());
    for(const Vector3<I> &plug_vertex: plug) {
        const Vector2<I> projected_plug_vertex = plug_vertex.project(theta, phi);
        if(!projected_plug_vertex.inside_advanced(projected_hole)) {
            return false;
        }
    }
    return true;
}

template<IntervalType I>
std::vector<I> split(const I &interval, const int parts) {
    std::vector<double> cutoffs;
    cutoffs.push_back(interval.min());
    const double step = interval.len() / parts;
    for(int i = 1; i < parts; i++) {
        const double cutoff = interval.min() + i * step;
        if(interval.has(cutoff)) {
            cutoffs.push_back(cutoff);
        }
    }
    cutoffs.push_back(interval.max());
    std::vector<I> sub_intervals;
    for(size_t i = 0; i < cutoffs.size() - 1; i++) {
        sub_intervals.emplace_back(cutoffs[i], cutoffs[i + 1]);
    }
    return sub_intervals;
}

template<IntervalType I>
std::vector<Vector2<I>> project_hole(const Box3 &box3, const std::vector<Vector3<I>> &hole, const int resolution) {
    const I theta = to_interval<BoostInterval>(box3.theta);
    const I phi = to_interval<BoostInterval>(box3.phi);
    const I alpha = to_interval<BoostInterval>(box3.alpha);
    std::vector<std::pair<double, double>> projected_hole_vertices;
    for(const I &sub_theta: split(theta, resolution)) {
        for(const I &sub_phi: split(phi, resolution)) {
            for(const I &sub_alpha: split(alpha, resolution)) {
                for(const Vector3<I> &hole_vertex: hole) {
                    const Vector2<I> projected_hole_vertex = hole_vertex.project(sub_theta, sub_phi).rotate(sub_alpha);
                    projected_hole_vertices.emplace_back(projected_hole_vertex.x.min(), projected_hole_vertex.y.min());
                    projected_hole_vertices.emplace_back(projected_hole_vertex.x.min(), projected_hole_vertex.y.max());
                    projected_hole_vertices.emplace_back(projected_hole_vertex.x.max(), projected_hole_vertex.y.min());
                    projected_hole_vertices.emplace_back(projected_hole_vertex.x.max(), projected_hole_vertex.y.max());
                }
            }
        }
    }
    std::vector<Vector2<I>> projected_hole_hull_vertices;
    for(const auto &[x, y]: convex_hull(projected_hole_vertices)) {
        projected_hole_hull_vertices.emplace_back(Vector2<I>(x, y));
    }
    return projected_hole_hull_vertices;
}

template<IntervalType I>
bool is_box3_terminal(const Box3 &box3, const std::vector<Vector3<I>> &plug, const std::vector<Vector3<I>> &hole, const int iterations, const int resolution) {
    std::vector<Vector2<I>> projected_hole = project_hole(box3, hole, resolution);
    Queue2 queue2;
    for(int t = 0; t < iterations; t++) {
        const std::optional<Box2> optional_box2 = queue2.pop();
        if(!optional_box2.has_value()) {
            return true;
        }
        const Box2 &box2 = optional_box2.value();
        if(is_box3_nonterminal(box2, plug, projected_hole)) {
            return false;
        }
        if(is_box2_terminal(box2, plug, projected_hole)) {
            continue;
        }
        for(const Box2 &sub_box2: box2.split()) {
            queue2.push(sub_box2);
        }
    }
    return queue2.empty();
}

template<IntervalType I>
void step(Queue3 &queue3, const std::vector<Vector3<I>> &plug, const std::vector<Vector3<I>> &hole, const int iterations, const int resolution) {
    const std::optional<Box3> optional_box3 = queue3.pop();
    if(!optional_box3.has_value()) {
        return;
    }
    const Box3 &box3 = optional_box3.value();
    const bool is_terminal = is_box3_terminal(box3, plug, hole, iterations, resolution);
    if(!is_terminal) {
        for(const Box3 &sub_box3: box3.split()) {
            queue3.push(sub_box3);
        }
    }
    if(is_terminal) {
        print("Terminal Box:", box3);
    } else {
        // print("Nonterminal Box:", box3);
    }
}

int main() {
    tests();

    std::vector<Vector3<BoostInterval>> plug = Polyhedron<BoostInterval>::rhombicosidodecahedron();
    std::vector<Vector3<BoostInterval>> hole = Polyhedron<BoostInterval>::rhombicosidodecahedron();

    constexpr int num_threads = 10;

    Queue3 queue3;
    std::vector<std::thread> threads;
    for(int t = 0; t < num_threads; t++) {
        threads.emplace_back([&queue3, &plug, &hole]() {
            while(true) {
                step(queue3, plug, hole, 10000, 10);
            }
        });
    }
    for(auto &thread: threads) {
        thread.join();
    }

    return 0;
}
