#pragma once

#include "geometry/vector2.hpp"
#include "geometry/vector3.hpp"
#include "geometry/matrix.hpp"
#include "geometry/edge.hpp"
#include "geometry/polygon.hpp"
#include "geometry/polyhedron.hpp"
#include "geometry/polyhedra.hpp"
#include "geometry/transformation.hpp" // TODO move this to a more appropriate place
#include <queue>
#include <optional>

template<IntervalType Interval>
bool is_centrally_symmetric(const std::vector<Vector3<Interval>>& vertices) {
    return std::all_of(vertices.begin(), vertices.end(), [&vertices](const Vector3<Interval>& vector2) {
        return std::any_of(vertices.begin(), vertices.end(), [&vector2](const Vector3<Interval>& other_vector2) {
            return !vector2.diff(-other_vector2);
        });
    });
}

template<IntervalType Interval>
Matrix<Interval> projection_matrix(const Interval& theta, const Interval& phi) {
    return Matrix<Interval>::rotate_x(phi) * Matrix<Interval>::rotate_z(theta);
}

template<IntervalType Interval>
Matrix<Interval> projection_rotation_matrix(const Interval& theta, const Interval& phi, const Interval& alpha) {
    return Matrix<Interval>::rotate_z(alpha) * projection_matrix(theta, phi);
}

template<IntervalType Interval>
Matrix<Interval> orthonormal_basis(const Vector3<Interval>& from, const Vector3<Interval>& to, bool right_handed) {
    Vector3<Interval> x_axis = from.unit();
    Vector3<Interval> y_axis = (to - from * to.dot(x_axis)).unit();
    Vector3<Interval> z_axis = right_handed ? x_axis.cross(y_axis).unit() : y_axis.cross(x_axis).unit();
    return Matrix<Interval>(
        x_axis.x(), y_axis.x(), z_axis.x(),
        x_axis.y(), y_axis.y(), z_axis.y(),
        x_axis.z(), y_axis.z(), z_axis.z()
    );
}

template<IntervalType Interval>
std::vector<Matrix<Interval>> symmetries(
    const std::vector<Vector3<Interval>>& vertices,
    bool right_handed) {
    const Vector3<Interval> from_vector3 = vertices[0];
    const Vector3<Interval> to_vector3 = from_vector3.diff(-vertices[1]) ? vertices[1] : vertices[2];
    Matrix<Interval> basis = orthonormal_basis<Interval>(from_vector3, to_vector3, true);
    std::vector<Matrix<Interval>> symmetries;
    symmetries.reserve(vertices.size() * vertices.size());
    for(const auto& from_vertex_image: vertices) {
        for(const auto& to_vertex_image: vertices) {
            if(((from_vertex_image - to_vertex_image).len() - (from_vector3 - to_vector3).len()).is_nonzero()) {
                continue;
            }
            Matrix<Interval> image_basis = orthonormal_basis<Interval>(from_vertex_image, to_vertex_image, right_handed);
            Matrix<Interval> symmetry = Matrix<Interval>::relative_rotation(basis, image_basis);
            const bool is_symmetry = std::ranges::all_of(vertices, [&](const Vector3<Interval>& vertex) {
                const Vector3<Interval> vertex_image = symmetry * vertex;
                return std::any_of(vertices.begin(), vertices.end(), [&](const Vector3<Interval>& other_vertex) {
                                       return !vertex_image.diff(other_vertex);
                                   }
                );
            });
            if(is_symmetry) {
                symmetries.emplace_back(symmetry);
            }
        }
    }
    return symmetries;
}

template<typename Interval>
Interval max_uncertainty(const std::vector<Vector2<Interval>>& vector2s) {
    size_t max_index = 0;
    for(size_t i = 1; i < vector2s.size(); i++) {
        if(vector2s[i].len().len() > vector2s[max_index].len().len()) {
            max_index = i;
        }
    }
    return vector2s[max_index].len().len();
}

// template<typename Interval>
// std::vector<Vector<Interval>> merge_vectors(const std::vector<Vector<Interval>>& vectors) {
//     const Interval epsilon = Interval(max_uncertainty(vectors)) * Interval(static_cast<int>(vectors.size())) * Interval(2);
//     ...
// }

template<IntervalType Interval>
Polygon<Interval> convex_hull(const std::vector<Vector2<Interval>>& vector2s) {
    std::vector<Edge<Interval>> edges;

    std::vector<bool> is_duplicate(vector2s.size(), false);
    bool any_non_duplicate = false;
    for(size_t i = 1; i < vector2s.size(); i++) {
        for(size_t j = 0; j < i; j++) {
            if(!vector2s[i].diff(vector2s[j])) {
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
    std::vector<bool> visited_indices(vector2s.size(), false);

    std::optional<size_t> start_index;
    for(size_t new_start_index = 0; new_start_index < vector2s.size(); new_start_index++) {
        if(is_duplicate[new_start_index]) {
            continue;
        }
        if(!start_index.has_value() || vector2s[new_start_index].len().max() > vector2s[start_index.value()].len().max()) {
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
        for(size_t new_most_clockwise_index = 0; new_most_clockwise_index < vector2s.size(); new_most_clockwise_index++) {
            if(is_duplicate[new_most_clockwise_index] || new_most_clockwise_index == from_index) {
                continue;
            }
            if(!vector2s[from_index].diff(vector2s[new_most_clockwise_index])) {
                throw std::runtime_error("Zero length edge found");
            }
            if(!most_clockwise_index.has_value() ||
               Edge<Interval>(vector2s[from_index], vector2s[most_clockwise_index.value()]).side(vector2s[new_most_clockwise_index]) == Side::right) {
                most_clockwise_index = new_most_clockwise_index;
            }
        }
        if(!most_clockwise_index.has_value()) {
            throw std::runtime_error("No most clockwise vertex found");
        }

        std::vector<size_t> to_indices;
        const Edge<Interval> most_clockwise_edge(vector2s[from_index], vector2s[most_clockwise_index.value()]);
        for(size_t to_index = 0; to_index < vector2s.size(); to_index++) {
            if(is_duplicate[to_index] || to_index == from_index) {
                continue;
            }
            if(!vector2s[from_index].diff(vector2s[to_index])) {
                throw std::runtime_error("Zero length edge found");
            }
            if(most_clockwise_edge.side(vector2s[to_index]) == Side::left) {
                continue;
            }
            bool is_most_clockwise = true;
            const Edge<Interval> edge(vector2s[from_index], vector2s[to_index]);
            for(size_t index = 0; index < vector2s.size(); index++) {
                if(index == from_index || index == to_index) {
                    continue;
                }
                if(edge.side(vector2s[index]) == Side::right) {
                    is_most_clockwise = false;
                    break;
                }
            }
            if(is_most_clockwise) {
                to_indices.push_back(to_index);
            }
        }
        std::ranges::stable_sort(to_indices, [&](const size_t index, const size_t other_index) {
            const Interval dist = vector2s[from_index].dist(vector2s[other_index]).max();
            const Interval other_dist = vector2s[from_index].dist(vector2s[index]).max();
            return dist < other_dist;
        });
        std::vector<std::pair<size_t, size_t>> new_edge_indices;
        for(const size_t to_index: to_indices) {
            const bool is_most_clockwise = std::ranges::none_of(new_edge_indices, [&](const std::pair<size_t, size_t>& new_edge_index) {
                const size_t from_edge_index = new_edge_index.first;
                const size_t to_edge_index = new_edge_index.second;
                const Edge<Interval> new_edge(vector2s[from_edge_index], vector2s[to_edge_index]);
                return new_edge.side(vector2s[to_index]) == Side::left;
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
            edges.emplace_back(vector2s[from_edge_index], vector2s[to_edge_index]);
        }
    }
    return Polygon(edges);
}

template<IntervalType Interval>
bool is_vector_inside_polygon(const Polygon<Interval>& polygon, const Vector2<Interval>& vector2) {
    if(std::ranges::any_of(polygon.edges(), [&](const Edge<Interval>& edge) {
        return !edge.avoids(vector2);
    })) {
        return false;
    }
    if(std::ranges::any_of(polygon.edges(), [&](const Edge<Interval>& edge) {
        return edge.side(vector2) == Side::right;
    })) {
        return false;
    }
    return true;
}

template<IntervalType Interval>
bool is_projected_vertex_inside_polygon_trivial(const Polygon<Interval>& polygon, const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    return is_vector_inside_polygon(polygon, projection_trivial(vertex, theta, phi));
}

template<IntervalType Interval>
bool is_projected_vertex_inside_polygon_combined(const Polygon<Interval>& polygon, const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    return is_vector_inside_polygon(polygon, projection_combined(vertex, theta, phi));
}

template<IntervalType Interval>
bool is_vector_outside_polygon(const Polygon<Interval>& polygon, const Vector2<Interval>& vector2) {
    if(std::ranges::any_of(polygon.edges(), [&](const Edge<Interval>& edge) {
        return !edge.avoids(vector2);
    })) {
        return false;
    }
    if(std::ranges::any_of(polygon.edges(), [&](const Edge<Interval>& edge) {
        return edge.side(vector2) == Side::right;
    })) {
        return true;
    }
    return false;
}

template<IntervalType Interval>
bool is_projected_vertex_outside_polygon_trivial(const Polygon<Interval>& polygon, const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    return is_vector_outside_polygon(polygon, projection_trivial(vertex, theta, phi));
}

template<IntervalType Interval>
bool is_projected_vertex_outside_polygon_combined(const Polygon<Interval>& polygon, const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    return is_vector_outside_polygon(polygon, projection_combined(vertex, theta, phi));
}

template<IntervalType Interval>
bool is_projected_vertex_avoiding_polygon_advanced_fixed_theta(const Polygon<Interval>& polygon, const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    const Vector2<Interval> projected_vertex = projection_combined(vertex, theta, phi);
    const Edge projected_edge(
        Vector2<Interval>(projected_vertex.x(), projected_vertex.y().min()),
        Vector2<Interval>(projected_vertex.x(), projected_vertex.y().max())
    );
    return std::ranges::all_of(polygon.edges(), [&](const Edge<Interval>& edge) {
        return projected_edge.avoids(edge);
    });
}

template<IntervalType Interval>
bool is_projected_vertex_avoiding_edge_advanced_fixed_phi(const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi, const Edge<Interval>& edge) {
    const Interval translation_factor = vertex.z() * phi.sin();
    const Interval scaling_factor = phi.cos();
    const Vector2<Interval> transformed_edge_from(
        edge.from().x(),
        (edge.from().y() + translation_factor) / scaling_factor
    );
    const Vector2<Interval> transformed_edge_to(
        edge.to().x(),
        (edge.to().y() + translation_factor) / scaling_factor
    );
    const Edge<Interval> transformed_edge(transformed_edge_from, transformed_edge_to);

    const Interval radius_squared = vertex.x().sqr() + vertex.y().sqr();
    const Interval quadratic_term = transformed_edge.len().sqr();
    const Interval linear_term = Interval(2) * transformed_edge.dir().dot(transformed_edge.from());
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

    const Vector2<Interval> min_projected_vertex = projection_trivial(vertex, theta.min(), phi);
    const Vector2<Interval> max_projected_vertex = projection_trivial(vertex, theta.max(), phi);
    const Vector2<Interval> transformed_min_projected_vertex = Vector2<Interval>(
        min_projected_vertex.x(),
        (min_projected_vertex.y() + translation_factor) / scaling_factor
    );
    const Vector2<Interval> transformed_max_projected_vertex = Vector2<Interval>(
        max_projected_vertex.x(),
        (max_projected_vertex.y() + translation_factor) / scaling_factor
    );
    const Edge<Interval> transformed_projected_vertex_edge(transformed_min_projected_vertex, transformed_max_projected_vertex);
    for(const Interval& solution: solutions) {
        if(solution.is_negative() || solution > transformed_edge.len()) {
            continue;
        }
        const Vector2<Interval> intersection = transformed_edge.from() + transformed_edge.dir() * transformed_edge.len() * solution;
        if(!transformed_projected_vertex_edge.avoids(Edge<Interval>(Vector2<Interval>(Interval(0), Interval(0)), intersection))) {
            return false;
        }
    }
    return true;
}

template<IntervalType Interval>
bool is_projected_vertex_avoiding_polygon_advanced_fixed_phi(const Polygon<Interval>& polygon, const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    if(!phi.cos().is_nonzero()) {
        return is_projected_vertex_outside_polygon_combined(polygon, vertex, theta, phi);
    }
    return std::ranges::all_of(polygon.edges(), [&](const Edge<Interval>& edge) {
        return is_projected_vertex_avoiding_edge_advanced_fixed_phi(vertex, theta, phi, edge);
    });
}

template<IntervalType Interval>
bool is_projected_vertex_outside_polygon_advanced(const Polygon<Interval>& polygon, const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    if(!(theta.len() < Interval::pi() / Interval(2))) {
        return is_projected_vertex_outside_polygon_combined(polygon, vertex, theta, phi);
    }
    return is_projected_vertex_outside_polygon_combined(polygon, vertex, theta.min(), phi.min()) &&
           is_projected_vertex_outside_polygon_combined(polygon, vertex, theta.max(), phi.max()) &&
           is_projected_vertex_outside_polygon_combined(polygon, vertex, theta.min(), phi.max()) &&
           is_projected_vertex_outside_polygon_combined(polygon, vertex, theta.max(), phi.min()) &&
           is_projected_vertex_avoiding_polygon_advanced_fixed_theta(polygon, vertex, theta.min(), phi) &&
           is_projected_vertex_avoiding_polygon_advanced_fixed_theta(polygon, vertex, theta.max(), phi) &&
           is_projected_vertex_avoiding_polygon_advanced_fixed_phi(polygon, vertex, theta, phi.min()) &&
           is_projected_vertex_avoiding_polygon_advanced_fixed_phi(polygon, vertex, theta, phi.max());
}
