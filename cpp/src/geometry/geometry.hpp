#pragma once

#include "geometry/edge.hpp"
#include "geometry/polygon.hpp"
#include "geometry/polyhedron.hpp"
#include <queue>

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
