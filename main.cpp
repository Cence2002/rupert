#include "util.hpp"
#include "interval.hpp"
#include "vector.hpp"
#include "cover.hpp"
#include "polyhedron.hpp"
#include "test.hpp"
#include <thread>

template<IntervalType I>
bool is_box2_terminal(const Box2& box2, const std::vector<Vector3<I>>& plug, const Polygon<I>& projected_hole) {
    return std::ranges::any_of(plug, [&](const Vector3<I>& plug_vertex) {
        const Vector2<I> projected_plug_vertex = plug_vertex.project(box2.theta<I>(), box2.phi<I>());
        return projected_hole.is_outside(projected_plug_vertex);
    });
}

template<IntervalType I>
bool is_box3_nonterminal(const Box2& box2, const std::vector<Vector3<I>>& plug, const Polygon<I>& projected_hole) {
    return std::ranges::all_of(plug, [&](const Vector3<I>& plug_vertex) {
        const Vector2<I> projected_plug_vertex = plug_vertex.project(box2.theta<I>().mid(), box2.phi<I>().mid());
        return projected_hole.is_inside(projected_plug_vertex);
    });
}

template<IntervalType I>
std::vector<I> split(const I& interval, const int parts) {
    std::vector<I> sub_intervals;
    for(int i = 0; i < parts; i++) {
        const I sub_interval = I(i, i + 1) * interval.len() / parts + interval.min();
        sub_intervals.push_back(sub_interval);
    }
    return sub_intervals;
}

template<IntervalType I>
Polygon<I> project_hole(const Box3& box3, const std::vector<Vector3<I>>& hole, const int resolution) {
    std::vector<Vector2<I>> projected_hole_vertices;
    for(const I& sub_theta: split(box3.theta<I>(), resolution)) {
        for(const I& sub_phi: split(box3.phi<I>(), resolution)) {
            for(const I& sub_alpha: split(box3.alpha<I>(), resolution)) {
                for(const Vector3<I>& hole_vertex: hole) {
                    const Vector2<I> projected_hole_vertex = hole_vertex.project(sub_theta, sub_phi).rotate(sub_alpha);
                    projected_hole_vertices.emplace_back(projected_hole_vertex.x().min(), projected_hole_vertex.y().min());
                    projected_hole_vertices.emplace_back(projected_hole_vertex.x().min(), projected_hole_vertex.y().max());
                    projected_hole_vertices.emplace_back(projected_hole_vertex.x().max(), projected_hole_vertex.y().min());
                    projected_hole_vertices.emplace_back(projected_hole_vertex.x().max(), projected_hole_vertex.y().max());
                }
            }
        }
    }
    return Polygon<I>::convex_hull(projected_hole_vertices);
}

template<IntervalType I>
bool is_box3_terminal(const Box3& box3, const std::vector<Vector3<I>>& plug, const std::vector<Vector3<I>>& hole,
                      const int iterations, const int resolution) {
    Polygon<I> projected_hole = project_hole(box3, hole, resolution);
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

template<IntervalType I>
void step(Queue3& queue3, const std::vector<Vector3<I>>& plug, const std::vector<Vector3<I>>& hole,
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
        print("Terminal Box:", box3);
    } else {
        print("Nonterminal Box:", box3);
    }
}

template<IntervalType I>
void solve(const int num_threads = 10, const int iterations = 10000, const int resolution = 5) {
    std::vector<Vector3<I>> plug = Polyhedron<I>::rhombicosidodecahedron();
    std::vector<Vector3<I>> hole = Polyhedron<I>::rhombicosidodecahedron();

    Queue3 queue3;
    if(num_threads == 1) {
        while(true) {
            step(queue3, plug, hole, iterations, resolution);
            break;
        }
    } else {
        std::vector<std::thread> threads;
        for(int t = 0; t < num_threads; t++) {
            threads.emplace_back([&] {
                while(true) {
                    step(queue3, plug, hole, iterations, resolution);
                }
            });
        }
        for(auto& thread: threads) {
            thread.join();
        }
    }
}

int main() {
    tests();
    solve<PreciseInterval>(10, 2, 2);

    mpfi_t a{}, b{};
    print(a->left._mpfr_d == nullptr);
    mpfi_init(a);
    print(a->left._mpfr_d == nullptr);
    mpfi_set_si(a, 1);
    print(a->left._mpfr_d == nullptr);
    mpfi_swap(a, b);
    print(a->left._mpfr_d == nullptr);
    print(b->left._mpfr_d == nullptr);
    mpfi_clear(b);
    print(b->left._mpfr_d == nullptr);

    // // default constructor
    // PreciseInterval a0;
    // // val constructor
    // PreciseInterval a1(1);
    // // min max constructor
    // PreciseInterval a2(1, 2);
    // // copy constructor
    // PreciseInterval a3 = a2;
    // // move constructor
    // a0 = a1;
    // // move constructor
    // PreciseInterval a4 = std::move(a2);
    // // move assignment
    // a3 = std::move(a4);
    // // destructor

    return 0;
}
