#pragma once

#include "geometry/edge.hpp"
#include "geometry/polygon.hpp"
#include <queue>
#include <optional>

template<IntervalType Interval>
Polygon<Interval> convex_hull(const std::vector<Vector2<Interval>>& vertices, const typename Interval::Number& epsilon) {
    const Interval epsilon_interval = 1 + Interval(epsilon);
    std::vector<Edge<Interval>> edges;

    std::queue<std::pair<size_t, Vector2<Interval>>> queue;
    std::vector<bool> visited_indices(vertices.size(), false);

    std::optional<size_t> start_index;
    for(size_t new_start_index = 0; new_start_index < vertices.size(); new_start_index++) {
        if(!start_index.has_value() || vertices[new_start_index].len_sqr() > vertices[start_index.value()].len_sqr()) {
            start_index = new_start_index;
        }
    }
    queue.emplace(start_index.value(), vertices[start_index.value()] * epsilon_interval);

    while(!queue.empty()) {
        const auto [from_index, from_vertex] = queue.front();
        queue.pop();
        if(visited_indices[from_index]) {
            continue;
        }
        visited_indices[from_index] = true;

        std::optional<size_t> most_clockwise_index;
        for(size_t new_most_clockwise_index = 0; new_most_clockwise_index < vertices.size(); new_most_clockwise_index++) {
            if(new_most_clockwise_index == from_index) {
                continue;
            }
            if(!(vertices[new_most_clockwise_index] - from_vertex).len_sqr().is_positive()) {
                throw std::runtime_error("Zero length edge found");
            }
            if(!most_clockwise_index.has_value()) {
                most_clockwise_index = new_most_clockwise_index;
                continue;
            }
            if(Edge<Interval>(from_vertex, vertices[most_clockwise_index.value()]).orientation(vertices[new_most_clockwise_index]) == Orientation::CLOCKWISE) {
                most_clockwise_index = new_most_clockwise_index;
            }
        }
        if(!most_clockwise_index.has_value()) {
            throw std::runtime_error("Vertices are too close to each other");
        }

        std::vector<size_t> to_indices;
        const Edge<Interval> most_clockwise_edge(from_vertex, vertices[most_clockwise_index.value()]);
        for(size_t to_index = 0; to_index < vertices.size(); to_index++) {
            if(to_index == from_index) {
                continue;
            }
            const Vector2<Interval> to_vertex = vertices[to_index];
            if(!(to_vertex - from_vertex).len_sqr().is_positive()) {
                throw std::runtime_error("Zero length edge found");
            }
            if(most_clockwise_edge.orientation(to_vertex) == Orientation::COUNTERCLOCKWISE) {
                continue;
            }
            bool is_most_clockwise = true;
            const Edge<Interval> edge(from_vertex, to_vertex);
            for(size_t index = 0; index < vertices.size(); index++) {
                if(index == from_index || index == to_index) {
                    continue;
                }
                if(edge.orientation(vertices[index]) == Orientation::CLOCKWISE) {
                    is_most_clockwise = false;
                    break;
                }
            }
            if(is_most_clockwise) {
                to_indices.push_back(to_index);
            }
        }
        std::ranges::stable_sort(to_indices, [&](const size_t index, const size_t other_index) {
            return (vertices[index] - from_vertex).len_sqr().min() < (vertices[other_index] - from_vertex).len_sqr().min();
        });
        std::vector<Edge<Interval>> new_edges;
        for(const size_t to_index: to_indices) {
            const Vector2<Interval> to_vertex = vertices[to_index] * epsilon_interval;
            const bool is_most_clockwise = std::ranges::none_of(new_edges, [&](const Edge<Interval>& new_edge) {
                return new_edge.orientation(to_vertex) == Orientation::COUNTERCLOCKWISE;
            });
            if(!is_most_clockwise) {
                continue;
            }
            new_edges.emplace_back(from_vertex, to_vertex);
            if(!visited_indices[to_index]) {
                queue.emplace(to_index, to_vertex);
            }
        }
        edges.insert(edges.end(), new_edges.begin(), new_edges.end());
    }
    return Polygon(edges);
}
