#pragma once

#include "geometry/edge.hpp"
#include "geometry/vertex.hpp"
#include <vector>
#include <queue>
#include <optional>

template<IntervalType Interval>
struct Polygon {
private:
    std::vector<Edge<Interval>> edges_;

public:
    explicit Polygon(const std::vector<Edge<Interval>>& edges) : edges_(edges) {}

    std::vector<Edge<Interval>> edges() const {
        return edges_;
    }

    bool is_vector_inside_polygon(const Vector<Interval>& vector) const {
        bool all_counter_clockwise = true;
        for(const Edge<Interval>& edge: edges()) {
            const Orientation orientation = edge.orientation(vector);
            if(orientation == Orientation::collinear) {
                return false;
            }
            const bool counter_clockwise = orientation == Orientation::counterclockwise;
            all_counter_clockwise &= counter_clockwise;
        }
        return all_counter_clockwise;
    }

    bool is_projected_vertex_inside_polygon_trivial(const Vertex<Interval>& vertex, const Interval& theta, const Interval& phi) const {
        const Vector<Interval> projected_vertex = projection_trivial(vertex, theta, phi);
        return is_vector_inside_polygon(projected_vertex);
    }

    bool is_projected_vertex_inside_polygon_combined(const Vertex<Interval>& vertex, const Interval& theta, const Interval& phi) const {
        const Vector<Interval> projected_vertex = projection_combined(vertex, theta, phi);
        return is_vector_inside_polygon(projected_vertex);
    }

    bool is_vector_outside_polygon(const Vector<Interval>& vector) const {
        bool any_clockwise = false;
        for(const Edge<Interval>& edge: edges()) {
            const Orientation orientation = edge.orientation(vector);
            if(!edge.avoids(vector)) {
                return false;
            }
            const bool clockwise = orientation == Orientation::clockwise;
            any_clockwise |= clockwise;
        }
        return any_clockwise;
    }

    bool is_projected_vertex_outside_polygon_trivial(const Vertex<Interval>& vertex, const Interval& theta, const Interval& phi) const {
        const Vector<Interval> projected_vertex = projection_trivial(vertex, theta, phi);
        return is_vector_outside_polygon(projected_vertex);
    }

    bool is_projected_vertex_outside_polygon_combined(const Vertex<Interval>& vertex, const Interval& theta, const Interval& phi) const {
        const Vector<Interval> projected_vertex = projection_combined(vertex, theta, phi);
        return is_vector_outside_polygon(projected_vertex);
    }

    bool is_projected_vertex_avoiding_polygon_advanced_fixed_theta(const Vertex<Interval>& vertex, const typename Interval::Number& theta, const Interval& phi) const {
        const Vector<Interval> projected_vertex = projection_combined(vertex, Interval(theta), phi);
        const Edge projected_edge(
            Vector<Interval>(projected_vertex.x(), Interval(projected_vertex.y().min())),
            Vector<Interval>(projected_vertex.x(), Interval(projected_vertex.y().max()))
        );
        return std::ranges::all_of(edges(), [&](const Edge<Interval>& edge) {
            return projected_edge.avoids(edge);
        });
    }

    bool is_projected_vertex_avoiding_edge_advanced_fixed_phi(const Vertex<Interval>& vertex, const Interval& theta, const typename Interval::Number& phi, const Edge<Interval>& edge) const {
        const Interval translation_factor = vertex.z() * Interval(phi).sin();
        const Interval scaling_factor = Interval(phi).cos();
        const Vector<Interval> transformed_edge_from(
            edge.from().x(),
            (edge.from().y() + translation_factor) / scaling_factor
        );
        const Vector<Interval> transformed_edge_to(
            edge.to().x(),
            (edge.to().y() + translation_factor) / scaling_factor
        );
        const Edge<Interval> transformed_edge(transformed_edge_from, transformed_edge_to);

        const Interval radius_squared = vertex.x() * vertex.x() + vertex.y() * vertex.y();
        const Interval double_quadratic_term = 2 * transformed_edge.direction().len_sqr();
        const Interval linear_term = 2 * Vector<Interval>::dot(transformed_edge.direction(), transformed_edge.from());
        const Interval constant_term = transformed_edge.from().len_sqr() - radius_squared;
        const Interval discriminant = linear_term.sqr() - 2 * double_quadratic_term * constant_term;
        if(!discriminant.is_positive()) {
            return true;
        }
        const Interval sqrt_discriminant = discriminant.sqrt();
        const std::array<Interval, 2> solutions = {
            (-linear_term + sqrt_discriminant) / double_quadratic_term,
            (-linear_term - sqrt_discriminant) / double_quadratic_term
        };

        const Vector<Interval> min_projected_vertex = projection_trivial(vertex, Interval(theta.min()), Interval(phi));
        const Vector<Interval> max_projected_vertex = projection_trivial(vertex, Interval(theta.max()), Interval(phi));
        const Vector<Interval> transformed_min_projected_vertex = Vector<Interval>(
            min_projected_vertex.x(),
            (min_projected_vertex.y() + translation_factor) / scaling_factor
        );
        const Vector<Interval> transformed_max_projected_vertex = Vector<Interval>(
            max_projected_vertex.x(),
            (max_projected_vertex.y() + translation_factor) / scaling_factor
        );
        const Edge<Interval> projected_vertex_edge(transformed_min_projected_vertex, transformed_max_projected_vertex);
        for(const Interval& solution: solutions) {
            if(solution.is_negative() || solution > 1) {
                continue;
            }
            const Vector<Interval> intersection = transformed_edge.from() + transformed_edge.direction() * solution;
            if(!projected_vertex_edge.avoids(Edge<Interval>(Vector<Interval>(Interval(0), Interval(0)), intersection))) {
                return false;
            }
        }
        return true;
    }

    bool is_projected_vertex_avoiding_polygon_advanced_fixed_phi(const Vertex<Interval>& vertex, const Interval& theta, const typename Interval::Number& phi) const {
        if(!Interval(phi).cos().is_nonzero()) {
            // degenerate case, division by zero would occur in the inverse transformation
            // default to combined
            return is_projected_vertex_outside_polygon_combined(vertex, theta, Interval(phi));
        }
        return std::ranges::all_of(edges(), [&](const Edge<Interval>& edge) {
            return is_projected_vertex_avoiding_edge_advanced_fixed_phi(vertex, theta, phi, edge);
        });
    }

    bool is_projected_vertex_outside_polygon_advanced(const Vertex<Interval>& vertex, const Interval& theta, const Interval& phi) const {
        if(!(theta.len() < Interval::pi() / 3)) {
            // projected vertex might surround the polygon, meaning it avoids, but is not outside
            // default to combined
            return is_projected_vertex_outside_polygon_combined(vertex, theta, phi);
        }
        return is_projected_vertex_outside_polygon_combined(vertex, Interval(theta.min()), Interval(phi.min())) &&
               is_projected_vertex_avoiding_polygon_advanced_fixed_theta(vertex, theta.min(), phi) &&
               is_projected_vertex_avoiding_polygon_advanced_fixed_theta(vertex, theta.max(), phi) &&
               is_projected_vertex_avoiding_polygon_advanced_fixed_phi(vertex, theta, phi.min()) &&
               is_projected_vertex_avoiding_polygon_advanced_fixed_phi(vertex, theta, phi.max());
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Polygon& polygon) {
        for(const Edge<Interval>& edge: polygon.edges_) {
            ostream << edge << "\n";
        }
        return ostream;
    }
};

template<IntervalType Interval>
Polygon<Interval> convex_hull(const std::vector<Vector<Interval>>& vertices, const typename Interval::Number& epsilon) {
    const Interval epsilon_interval = 1 + Interval(epsilon);
    std::vector<Edge<Interval>> edges;

    std::queue<std::pair<size_t, Vector<Interval>>> queue;
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
            if(Edge<Interval>(from_vertex, vertices[most_clockwise_index.value()]).orientation(vertices[new_most_clockwise_index]) == Orientation::clockwise) {
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
            const Vector<Interval> to_vertex = vertices[to_index];
            if(!(to_vertex - from_vertex).len_sqr().is_positive()) {
                throw std::runtime_error("Zero length edge found");
            }
            if(most_clockwise_edge.orientation(to_vertex) == Orientation::counterclockwise) {
                continue;
            }
            bool is_most_clockwise = true;
            const Edge<Interval> edge(from_vertex, to_vertex);
            for(size_t index = 0; index < vertices.size(); index++) {
                if(index == from_index || index == to_index) {
                    continue;
                }
                if(edge.orientation(vertices[index]) == Orientation::clockwise) {
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
            const Vector<Interval> to_vertex = vertices[to_index] * epsilon_interval;
            const bool is_most_clockwise = std::ranges::none_of(new_edges, [&](const Edge<Interval>& new_edge) {
                return new_edge.orientation(to_vertex) == Orientation::counterclockwise;
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
