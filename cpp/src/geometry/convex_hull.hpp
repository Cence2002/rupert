#pragma once

#include "geometry/edge.hpp"
#include "geometry/polygon.hpp"
#include <queue>
#include <optional>

template<IntervalType Interval>
Polygon<Interval> convex_hull(const std::vector<Vector2<Interval>>& vertices, const typename Interval::Number& epsilon) {
    std::vector<Edge<Interval>> edges;

    std::optional<size_t> start_index;
    for(size_t new_start_index = 0; new_start_index < vertices.size(); new_start_index++) {
        if(!start_index.has_value() || vertices[new_start_index].x().max() > vertices[start_index.value()].x().max()) {
            start_index = new_start_index;
        }
    }

    std::vector<bool> visited_indices(vertices.size(), false);
    std::queue<std::pair<size_t, Vector2<Interval>>> queue({{start_index.value(), vertices[start_index.value()] * Interval(epsilon)}});
    while(!queue.empty()) {
        const auto [from_index, from_vertex] = queue.front();
        queue.pop();
        if(visited_indices[from_index]) {
            continue;
        }
        visited_indices[from_index] = true;

        std::optional<size_t> most_clockwise_index;
        for(size_t new_most_clockwise_index = 0; new_most_clockwise_index < vertices.size(); new_most_clockwise_index++) {
            if(!most_clockwise_index.has_value() && (vertices[new_most_clockwise_index] - from_vertex).len().is_positive()) {
                most_clockwise_index = new_most_clockwise_index;
            }
            if(most_clockwise_index.has_value() && Edge<Interval>(from_vertex, vertices[most_clockwise_index.value()]).orientation(vertices[new_most_clockwise_index]) == Orientation::CLOCKWISE) {
                most_clockwise_index = new_most_clockwise_index;
            }
        }
        if(!most_clockwise_index.has_value()) {
            throw std::runtime_error("No most clockwise index found");
        }

        std::vector<size_t> to_indices;
        for(size_t to_index = 0; to_index < vertices.size(); to_index++) {
            if(!(vertices[to_index] - from_vertex).len().is_positive()) {
                print(vertices[to_index], from_vertex);
                throw std::runtime_error("Zero length edge found");
            }
            const Vector2<Interval> to_vertex = vertices[to_index];
            if(Edge<Interval>(from_vertex, vertices[most_clockwise_index.value()]).orientation(to_vertex) == Orientation::COUNTERCLOCKWISE) {
                continue;
            }
            bool is_most_clockwise = true;
            for(size_t index = 0; index < vertices.size(); index++) {
                if(index == from_index || index == to_index) {
                    continue;
                }
                if(Edge<Interval>(from_vertex, to_vertex).orientation(vertices[index]) == Orientation::CLOCKWISE) {
                    is_most_clockwise = false;
                    break;
                }
            }
            if(is_most_clockwise) {
                to_indices.push_back(to_index);
            }
        }
        std::ranges::stable_sort(to_indices, [&](const size_t index, const size_t other_index) {
            return (vertices[index] - from_vertex).len().max() < (vertices[other_index] - from_vertex).len().max();
        });
        for(const size_t to_index: to_indices) {
            const Vector2<Interval> to_vertex = vertices[to_index] * Interval(epsilon);
            edges.emplace_back(from_vertex, to_vertex);
            if(!visited_indices[to_index]) {
                queue.push({to_index, vertices[to_index] * Interval(epsilon)});
            }
        }
    }
    return Polygon(edges);
}
