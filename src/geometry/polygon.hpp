#pragma once

#include "geometry/line.hpp"
#include <vector>
#include <queue>

template<IntervalType I>
struct Polygon {
private:
    std::vector<Line<I>> edges_;

public:
    explicit Polygon() = default;

    Polygon(const Polygon& polygon) = default;

    Polygon& operator=(const Polygon& polygon) = default;

    explicit Polygon(const std::vector<Line<I>>& edges) : edges_(edges) {}

    static Polygon convex_hull(const std::vector<Vector2<I>>& vertices) {
        std::vector<Line<I>> edges;

        std::vector<bool> visited(vertices.size(), false);

        size_t max_index = 0;
        for(size_t better_max_index = 1; better_max_index < vertices.size(); better_max_index++) {
            const I better_max = vertices[better_max_index].x();
            if(better_max > vertices[max_index].x()) {
                max_index = better_max_index;
            }
        }
        std::queue<size_t> queue({max_index});

        while(!queue.empty()) {
            const size_t from_index = queue.front();
            queue.pop();
            if(visited[from_index]) {
                continue;
            }
            visited[from_index] = true;
            const Vector2<I> from = vertices[from_index];
            size_t most_clockwise_index_sample = (from_index + 1) % vertices.size();
            for(size_t clockwise_index = 0; clockwise_index < vertices.size(); clockwise_index++) {
                if(clockwise_index == from_index) {
                    continue;
                }
                const Vector2<I> to = vertices[clockwise_index];
                if(Line<I>(from, vertices[most_clockwise_index_sample]).orientation(to) == Orientation::CLOCKWISE) {
                    most_clockwise_index_sample = clockwise_index;
                }
            }
            std::vector<size_t> most_clockwise_indices;
            for(size_t clockwise_index = 0; clockwise_index < vertices.size(); clockwise_index++) {
                if(from_index == clockwise_index) {
                    continue;
                }
                const Vector2<I> clockwise = vertices[clockwise_index];
                if(Line<I>(from, vertices[most_clockwise_index_sample]).orientation(clockwise) == Orientation::COUNTERCLOCKWISE) {
                    continue;
                }
                bool most_clockwise = true;
                for(size_t clockwise_clockwise_index = 0; clockwise_clockwise_index < vertices.size(); clockwise_clockwise_index++) {
                    if(clockwise_clockwise_index == from_index || clockwise_clockwise_index == clockwise_index) {
                        continue;
                    }
                    const Vector2<I> clockwise_clockwise = vertices[clockwise_clockwise_index];
                    if(Line<I>(from, clockwise).orientation(clockwise_clockwise) == Orientation::CLOCKWISE) {
                        most_clockwise = false;
                        break;
                    }
                }
                if(most_clockwise) {
                    most_clockwise_indices.push_back(clockwise_index);
                }
            }
            std::ranges::sort(most_clockwise_indices, [&](const size_t a, const size_t b) {
                return (vertices[a] - from).len().min() < (vertices[b] - from).len().min();
            });
            for(const size_t most_clockwise_index: most_clockwise_indices) {
                edges.push_back(Line<I>(vertices[from_index], vertices[most_clockwise_index]));
                if(!visited[most_clockwise_index]) {
                    queue.push(most_clockwise_index);
                }
            }
        }
        return Polygon(edges);
    }

    bool is_inside(const Vector2<I>& v) const {
        bool all_counter_clockwise = true;
        for(const Line<I>& edge: edges_) {
            const Orientation orientation = edge.orientation(v);
            if(orientation == Orientation::COLLINEAR) {
                return false;
            }
            all_counter_clockwise &= orientation == Orientation::COUNTERCLOCKWISE;
        }
        return all_counter_clockwise;
    }

    bool is_outside(const Vector2<I>& v) const {
        bool any_clockwise = false;
        for(const Line<I>& edge: edges_) {
            const Orientation orientation = edge.orientation(v);
            if(orientation == Orientation::COLLINEAR) {
                return false;
            }
            any_clockwise |= orientation == Orientation::CLOCKWISE;
        }
        return any_clockwise;
    }

    friend std::ostream& operator<<(std::ostream& os, const Polygon& polygon) {
        for(const Line<I>& edge: polygon.edges_) {
            os << edge << std::endl;
        }
        return os;
    }
};
