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

    const std::vector<Edge<Interval>>& edges() const {
        return edges_;
    }

    static bool is_centrally_symmetric(const std::vector<Vertex<Interval>>& vertices) {
        return std::all_of(vertices.begin(), vertices.end(), [&vertices](const Vertex<Interval>& vertex) {
            return std::any_of(vertices.begin(), vertices.end(), [&vertex](const Vertex<Interval>& other_vertex) {
                return !vertex.diff(-other_vertex);
            });
        });
    }

    bool is_vector_inside_polygon(const Vector<Interval>& vector) const {
        if(std::ranges::any_of(edges_, [&](const Edge<Interval>& edge) {
            return !edge.avoids(vector);
        })) {
            return false;
        }
        if(std::ranges::any_of(edges_, [&](const Edge<Interval>& edge) {
            return edge.orientation(vector) == Orientation::negative;
        })) {
            return false;
        }
        return true;
    }

    bool is_projected_vertex_inside_polygon_trivial(const Vertex<Interval>& vertex, const Interval& theta, const Interval& phi) const {
        return is_vector_inside_polygon(projection_trivial(vertex, theta, phi));
    }

    bool is_projected_vertex_inside_polygon_combined(const Vertex<Interval>& vertex, const Interval& theta, const Interval& phi) const {
        return is_vector_inside_polygon(projection_combined(vertex, theta, phi));
    }

    bool is_vector_outside_polygon(const Vector<Interval>& vector) const {
        if(std::ranges::any_of(edges_, [&](const Edge<Interval>& edge) {
            return !edge.avoids(vector);
        })) {
            return false;
        }
        if(std::ranges::any_of(edges_, [&](const Edge<Interval>& edge) {
            return edge.orientation(vector) == Orientation::negative;
        })) {
            return true;
        }
        return false;
    }

    bool is_projected_vertex_outside_polygon_trivial(const Vertex<Interval>& vertex, const Interval& theta, const Interval& phi) const {
        return is_vector_outside_polygon(projection_trivial(vertex, theta, phi));
    }

    bool is_projected_vertex_outside_polygon_combined(const Vertex<Interval>& vertex, const Interval& theta, const Interval& phi) const {
        return is_vector_outside_polygon(projection_combined(vertex, theta, phi));
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

        const Interval radius_squared = vertex.x().sqr() + vertex.y().sqr();
        const Interval quadratic_term = transformed_edge.len().sqr();
        const Interval linear_term = Interval(2) * transformed_edge.dir().dot(transformed_edge.from()); // TODO maybe: *transformed_edge.len()
        const Interval constant_term = transformed_edge.from().len().sqr() - radius_squared;
        const Interval discriminant = linear_term.sqr() - Interval(4) * quadratic_term * constant_term;
        if(!discriminant.is_positive()) {
            return true;
        }
        const Interval sqrt_discriminant = discriminant.sqrt();
        const std::array<Interval, 2> solutions = {
            (-linear_term + sqrt_discriminant) / (Interval(2) * quadratic_term),
            (-linear_term - sqrt_discriminant) / (Interval(2) * quadratic_term)
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
        const Edge<Interval> transformed_projected_vertex_edge(transformed_min_projected_vertex, transformed_max_projected_vertex);
        for(const Interval& solution: solutions) {
            if(solution.is_negative() || solution > transformed_edge.len()) {
                continue;
            }
            const Vector<Interval> intersection = transformed_edge.from() + transformed_edge.dir() * transformed_edge.len() * solution;
            if(!transformed_projected_vertex_edge.avoids(Edge<Interval>(Vector<Interval>(Interval(0), Interval(0)), intersection))) {
                return false;
            }
        }
        return true;
    }

    bool is_projected_vertex_avoiding_polygon_advanced_fixed_phi(const Vertex<Interval>& vertex, const Interval& theta, const typename Interval::Number& phi) const {
        if(!Interval(phi).cos().is_nonzero()) {
            return is_projected_vertex_outside_polygon_combined(vertex, theta, Interval(phi));
        }
        return std::ranges::all_of(edges(), [&](const Edge<Interval>& edge) {
            return is_projected_vertex_avoiding_edge_advanced_fixed_phi(vertex, theta, phi, edge);
        });
    }

    bool is_projected_vertex_outside_polygon_advanced(const Vertex<Interval>& vertex, const Interval& theta, const Interval& phi) const {
        if(!(Interval(theta.len()) < Interval::pi() / Interval(2))) {
            return is_projected_vertex_outside_polygon_combined(vertex, theta, phi);
        }
        return is_projected_vertex_outside_polygon_combined(vertex, Interval(theta.min()), Interval(phi.min())) &&
               is_projected_vertex_outside_polygon_combined(vertex, Interval(theta.max()), Interval(phi.max())) &&
               is_projected_vertex_outside_polygon_combined(vertex, Interval(theta.min()), Interval(phi.max())) &&
               is_projected_vertex_outside_polygon_combined(vertex, Interval(theta.max()), Interval(phi.min())) &&
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

template<typename Interval>
typename Interval::Number max_uncertainty(const std::vector<Vector<Interval>>& vectors) {
    size_t best_index = 0;
    for(size_t i = 1; i < vectors.size(); i++) {
        if(vectors[i].len().len() > vectors[best_index].len().len()) {
            best_index = i;
        }
    }
    return vectors[best_index].len().len();
}

// template<typename Interval>
// std::vector<Vector<Interval>> merge_vectors(const std::vector<Vector<Interval>>& vectors) {
//     const Interval epsilon = Interval(max_uncertainty(vectors)) * Interval(static_cast<int>(vectors.size())) * Interval(2);
//     ...
// }

template<IntervalType Interval>
Polygon<Interval> convex_hull(const std::vector<Vector<Interval>>& vectors) {
    std::vector<Edge<Interval>> edges;

    std::vector<bool> is_duplicate(vectors.size(), false);
    bool any_non_duplicate = false;
    for(size_t i = 1; i < vectors.size(); i++) {
        for(size_t j = 0; j < i; j++) {
            if(!vectors[i].diff(vectors[j])) {
                is_duplicate[i] = true;
                break;
            }
        }
        if(!is_duplicate[i]) {
            any_non_duplicate = true;
        }
    }
    if(!any_non_duplicate) {
        throw std::runtime_error("All vectors are duplicates");
    }

    std::queue<size_t> queue;
    std::vector<bool> visited_indices(vectors.size(), false);

    std::optional<size_t> start_index;
    for(size_t new_start_index = 0; new_start_index < vectors.size(); new_start_index++) {
        if(is_duplicate[new_start_index]) {
            continue;
        }
        if(!start_index.has_value() || vectors[new_start_index].len().max() > vectors[start_index.value()].len().max()) {
            start_index = new_start_index;
        }
    }
    queue.emplace(start_index.value());

    while(!queue.empty()) {
        const size_t from_index = queue.front();
        queue.pop();
        if(visited_indices[from_index]) {
            continue;
        }
        visited_indices[from_index] = true;

        std::optional<size_t> most_clockwise_index;
        for(size_t new_most_clockwise_index = 0; new_most_clockwise_index < vectors.size(); new_most_clockwise_index++) {
            if(is_duplicate[new_most_clockwise_index] || new_most_clockwise_index == from_index) {
                continue;
            }
            if(!vectors[from_index].diff(vectors[new_most_clockwise_index])) {
                throw std::runtime_error("Zero length edge found");
            }
            if(!most_clockwise_index.has_value() ||
               Edge<Interval>(vectors[from_index], vectors[most_clockwise_index.value()]).orientation(vectors[new_most_clockwise_index]) == Orientation::negative) {
                most_clockwise_index = new_most_clockwise_index;
            }
        }
        if(!most_clockwise_index.has_value()) {
            throw std::runtime_error("No most clockwise vertex found");
        }

        std::vector<size_t> to_indices;
        const Edge<Interval> most_clockwise_edge(vectors[from_index], vectors[most_clockwise_index.value()]);
        for(size_t to_index = 0; to_index < vectors.size(); to_index++) {
            if(is_duplicate[to_index] || to_index == from_index) {
                continue;
            }
            if(!vectors[from_index].diff(vectors[to_index])) {
                throw std::runtime_error("Zero length edge found");
            }
            if(most_clockwise_edge.orientation(vectors[to_index]) == Orientation::positive) {
                continue;
            }
            bool is_most_clockwise = true;
            const Edge<Interval> edge(vectors[from_index], vectors[to_index]);
            for(size_t index = 0; index < vectors.size(); index++) {
                if(index == from_index || index == to_index) {
                    continue;
                }
                if(edge.orientation(vectors[index]) == Orientation::negative) {
                    is_most_clockwise = false;
                    break;
                }
            }
            if(is_most_clockwise) {
                to_indices.push_back(to_index);
            }
        }
        std::ranges::stable_sort(to_indices, [&](const size_t index, const size_t other_index) {
            const typename Interval::Number dist = vectors[from_index].dist(vectors[other_index]).max();
            const typename Interval::Number other_dist = vectors[from_index].dist(vectors[index]).max();
            return dist < other_dist;
        });
        std::vector<std::pair<size_t, size_t>> new_edge_indices;
        for(const size_t to_index: to_indices) {
            const bool is_most_clockwise = std::ranges::none_of(new_edge_indices, [&](const std::pair<size_t, size_t>& new_edge_index) {
                const size_t from_edge_index = new_edge_index.first;
                const size_t to_edge_index = new_edge_index.second;
                const Edge<Interval> new_edge(vectors[from_edge_index], vectors[to_edge_index]);
                return new_edge.orientation(vectors[to_index]) == Orientation::positive;
            });
            if(!is_most_clockwise) {
                continue;
            }
            new_edge_indices.emplace_back(from_index, to_index);
            if(!visited_indices[to_index]) {
                queue.emplace(to_index);
            }
        }
        for(const auto& [from_edge_index, to_edge_index]: new_edge_indices) {
            edges.emplace_back(vectors[from_edge_index], vectors[to_edge_index]);
        }
    }
    return Polygon(edges);
}
