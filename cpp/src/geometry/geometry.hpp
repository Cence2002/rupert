#pragma once

#include "geometry/edge.hpp"
#include "geometry/polygon.hpp"
#include "geometry/polyhedron.hpp"
#include <queue>

template<IntervalType Interval>
std::vector<IntervalVector2<Interval>> intervalvector2_hull(const IntervalVector2<Interval>& intervalvector2) {
    return std::vector<IntervalVector2<Interval>>{
            IntervalVector2<Interval>(Interval(intervalvector2.x().min()), Interval(intervalvector2.y().min())),
            IntervalVector2<Interval>(Interval(intervalvector2.x().min()), Interval(intervalvector2.y().max())),
            IntervalVector2<Interval>(Interval(intervalvector2.x().max()), Interval(intervalvector2.y().min())),
            IntervalVector2<Interval>(Interval(intervalvector2.x().max()), Interval(intervalvector2.y().max()))
        };
}

template<IntervalType Interval>
IntervalVector2<Interval> rotation(const IntervalVector2<Interval>& projected_vertex, const Interval& alpha) {
    const Interval cos_alpha = alpha.cos();
    const Interval sin_alpha = alpha.sin();
    return IntervalVector2<Interval>(
        projected_vertex.x() * cos_alpha - projected_vertex.y() * sin_alpha,
        projected_vertex.x() * sin_alpha + projected_vertex.y() * cos_alpha
    );
}

template<IntervalType Interval>
std::vector<IntervalVector2<Interval>> rotation_hull_trivial(const IntervalVector2<Interval>& projected_vertex, const Interval& alpha) {
    const IntervalVector2<Interval> rotated_vertex = rotation(projected_vertex, alpha);
    return intervalvector2_hull(rotated_vertex);
}

template<IntervalType Interval>
std::vector<IntervalVector2<Interval>> rotation_hull_triangle(const IntervalVector2<Interval>& intervalvector2, const Interval& alpha) {
    if(!(alpha.len() < Interval::pi() / 2)) {
        throw std::invalid_argument("rotation_hull_triangle only supports alpha < pi/2");
    }

    const Interval cos_alpha_min = Interval(alpha.min()).cos();
    const Interval cos_alpha_max = Interval(alpha.max()).cos();
    const Interval cos_alpha_mid = Interval(alpha.mid()).cos();

    const Interval sin_alpha_min = Interval(alpha.min()).sin();
    const Interval sin_alpha_max = Interval(alpha.max()).sin();
    const Interval sin_alpha_mid = Interval(alpha.mid()).sin();

    const Interval cos_alpha_rad = Interval(alpha.rad()).cos();

    return std::vector<IntervalVector2<Interval>>{
            IntervalVector2(
                intervalvector2.x() * cos_alpha_min - intervalvector2.y() * sin_alpha_min,
                intervalvector2.x() * sin_alpha_min + intervalvector2.y() * cos_alpha_min
            ),
            IntervalVector2(
                intervalvector2.x() * cos_alpha_max - intervalvector2.y() * sin_alpha_max,
                intervalvector2.x() * sin_alpha_max + intervalvector2.y() * cos_alpha_max
            ),
            IntervalVector2(
                (intervalvector2.x() * cos_alpha_mid - intervalvector2.y() * sin_alpha_mid) / cos_alpha_rad,
                (intervalvector2.x() * sin_alpha_mid + intervalvector2.y() * cos_alpha_mid) / cos_alpha_rad
            )
        };
}

template<IntervalType Interval>
IntervalVector2<Interval> projection(const IntervalVector3<Interval>& vertex, const Interval& phi, const Interval& theta) {
    const Interval& cos_phi = phi.cos();
    const Interval& sin_phi = phi.sin();
    const Interval& cos_theta = theta.cos();
    const Interval& sin_theta = theta.sin();
    return IntervalVector2<Interval>(
        -vertex.x() * sin_phi + vertex.y() * cos_phi,
        (vertex.x() * cos_phi + vertex.y() * sin_phi) * cos_theta - vertex.z() * sin_theta
    );
}

template<IntervalType Interval>
std::vector<IntervalVector2<Interval>> projection_hull_trivial(const IntervalVector3<Interval>& vertex, const Interval& phi, const Interval& theta) {
    const IntervalVector2<Interval> projected_vertex = projection(vertex, phi, theta);
    return intervalvector2_hull(projected_vertex);
}

template<IntervalType Interval>
Polygon<Interval> convex_hull(const std::vector<IntervalVector2<Interval>>& vertices) {
    std::vector<Edge<Interval>> edges;

    std::vector<bool> visited(vertices.size(), false);

    size_t most_max_x_index = 0;
    for(size_t max_x_index = 1; max_x_index < vertices.size(); max_x_index++) {
        if(vertices[max_x_index].x().max() > vertices[most_max_x_index].x().max()) {
            most_max_x_index = max_x_index;
        }
    }

    std::queue<size_t> queue({most_max_x_index});
    while(!queue.empty()) {
        const size_t from_index = queue.front();
        queue.pop();
        if(visited[from_index]) {
            continue;
        }
        visited[from_index] = true;

        const IntervalVector2<Interval> from = vertices[from_index];
        size_t most_clockwise_index = -1;
        for(size_t clockwise_index = 0; clockwise_index < vertices.size(); clockwise_index++) {
            if(most_clockwise_index == -1 && (vertices[clockwise_index] - from).len().is_pos()) {
                most_clockwise_index = clockwise_index;
            }
            if(Edge<Interval>(from, vertices[most_clockwise_index] - from).orientation(vertices[clockwise_index]) == Orientation::CLOCKWISE) {
                most_clockwise_index = clockwise_index;
            }
        }

        std::vector<size_t> to_indices;
        for(size_t to_index = 0; to_index < vertices.size(); to_index++) {
            if(!(vertices[to_index] - from).len().is_pos()) {
                continue;
            }
            const IntervalVector2<Interval> to = vertices[to_index];
            if(Edge<Interval>(from, vertices[most_clockwise_index] - from).orientation(to) == Orientation::COUNTERCLOCKWISE) {
                continue;
            }
            bool most_clockwise = true;
            for(size_t clockwise_index = 0; clockwise_index < vertices.size(); clockwise_index++) {
                if(clockwise_index == from_index || clockwise_index == to_index) {
                    continue;
                }
                if(Edge<Interval>(from, to - from).orientation(vertices[clockwise_index]) == Orientation::CLOCKWISE) {
                    most_clockwise = false;
                    break;
                }
            }
            if(most_clockwise) {
                to_indices.push_back(to_index);
            }
        }
        std::ranges::sort(to_indices, [&](const size_t a, const size_t b) {
            return (vertices[a] - from).len().mid() < (vertices[b] - from).len().mid();
        });
        for(const size_t to_index: to_indices) {
            edges.emplace_back(vertices[from_index], vertices[to_index] - vertices[from_index]);
            if(!visited[to_index]) {
                queue.push(to_index);
            }
        }
    }
    return Polygon(edges);
}
