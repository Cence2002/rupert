#pragma once

#include "geometry/edge.hpp"
#include "geometry/polygon.hpp"
#include "geometry/polyhedron.hpp"
#include <queue>
#include <optional>

template<IntervalType Interval>
Interval harmonic_trivial(const Interval& cos_amplitude, const Interval& sin_amplitude, const Interval& angle) {
    return cos_amplitude * angle.cos() + sin_amplitude * angle.sin();
}

template<IntervalType Interval>
Interval harmonic_combined(const Interval& cos_amplitude, const Interval& sin_amplitude, const Interval& angle) {
    if(cos_amplitude.is_nonzero()) {
        const Interval amplitude = (cos_amplitude.sqr() + sin_amplitude.sqr()).sqrt();
        const Interval phase = (sin_amplitude / cos_amplitude).atan();
        const int sign = cos_amplitude.is_positive() ? 1 : -1;
        return sign * amplitude * (angle - phase).cos();
    }
    if(sin_amplitude.is_nonzero()) {
        const Interval amplitude = (cos_amplitude.sqr() + sin_amplitude.sqr()).sqrt();
        const Interval phase = -(cos_amplitude / sin_amplitude).atan();
        const int sign = sin_amplitude.is_positive() ? 1 : -1;
        return sign * amplitude * (angle - phase).sin();
    }
    return harmonic_trivial(cos_amplitude, sin_amplitude, angle);
}

template<IntervalType Interval>
std::vector<Vector2Interval<Interval>> vector2_hull(const Vector2Interval<Interval>& vector2) {
    return std::vector<Vector2Interval<Interval>>{
        Vector2Interval<Interval>(Interval(vector2.x().min()), Interval(vector2.y().min())),
        Vector2Interval<Interval>(Interval(vector2.x().min()), Interval(vector2.y().max())),
        Vector2Interval<Interval>(Interval(vector2.x().max()), Interval(vector2.y().max())),
        Vector2Interval<Interval>(Interval(vector2.x().max()), Interval(vector2.y().min()))
    };
}

// (x, y) rotated with angle alpha
// X = x * cos(alpha) - y * sin(alpha)
// Y = x * sin(alpha) + y * cos(alpha)

template<IntervalType Interval>
Vector2Interval<Interval> rotation_trivial(const Vector2Interval<Interval>& projected_vertex, const Interval& alpha) {
    return Vector2Interval<Interval>(
        harmonic_trivial(projected_vertex.x(), -projected_vertex.y(), alpha),
        harmonic_trivial(projected_vertex.y(), projected_vertex.x(), alpha)
    );
}

template<IntervalType Interval>
Vector2Interval<Interval> rotation_combined(const Vector2Interval<Interval>& projected_vertex, const Interval& alpha) {
    return Vector2Interval<Interval>(
        harmonic_combined(projected_vertex.x(), -projected_vertex.y(), alpha),
        harmonic_combined(projected_vertex.y(), projected_vertex.x(), alpha)
    );
}

template<IntervalType Interval>
std::vector<Vector2Interval<Interval>> rotation_hull_trivial(const Vector2Interval<Interval>& projected_vertex, const Interval& alpha) {
    const Vector2Interval<Interval> rotated_vertex = rotation_trivial(projected_vertex, alpha);
    return vector2_hull(rotated_vertex);
}

template<IntervalType Interval>
std::vector<Vector2Interval<Interval>> rotation_hull_combined(const Vector2Interval<Interval>& projected_vertex, const Interval& alpha) {
    const Vector2Interval<Interval> rotated_vertex = rotation_combined(projected_vertex, alpha);
    return vector2_hull(rotated_vertex);
}

template<IntervalType Interval>
std::vector<Vector2Interval<Interval>> rotation_hull_triangle(const Vector2Interval<Interval>& vector2, const Interval& alpha) {
    if(!(alpha.len() < Interval::pi() / 3)) {
        // apex of triangle is not well-defined, or it might be numerically unstable
        // default to combined
        return rotation_hull_combined(vector2, alpha);
    }

    Vector2Interval<Interval> min_rotated_vertex = rotation_trivial(vector2, Interval(alpha.min()));
    Vector2Interval<Interval> mid_rotated_vertex = rotation_trivial(vector2, Interval(alpha.mid()));
    Vector2Interval<Interval> max_rotated_vertex = rotation_trivial(vector2, Interval(alpha.max()));

    const Interval cos_alpha_rad = Interval(alpha.rad()).cos();
    Vector2Interval<Interval> mid_rotated_vertex_scaled(
        mid_rotated_vertex.x() / cos_alpha_rad,
        mid_rotated_vertex.y() / cos_alpha_rad
    );

    return std::vector<Vector2Interval<Interval>>{
        min_rotated_vertex,
        mid_rotated_vertex,
        max_rotated_vertex
    };
}

// (x, y, z) projected angle theta and phi
// X = -x * sin(theta) + y * cos(theta)
// Y = (x * cos(theta) + y * sin(theta)) * cos(phi) - z * sin(phi)

template<IntervalType Interval>
Vector2Interval<Interval> projection_trivial(const Vector3Interval<Interval>& vertex, const Interval& theta, const Interval& phi) {
    const Vector2Interval<Interval> reflected_vertex = Vector2Interval<Interval>(vertex.x(), -vertex.y());
    const Interval shifted_theta = theta + Interval::pi() / 2;
    const Vector2Interval<Interval> rotated_reflected_vertex = rotation_trivial(reflected_vertex, shifted_theta);
    return Vector2Interval<Interval>(
        rotated_reflected_vertex.x(),
        harmonic_trivial(rotated_reflected_vertex.y(), -vertex.z(), phi)
    );
}

template<IntervalType Interval>
Vector2Interval<Interval> projection_combined(const Vector3Interval<Interval>& vertex, const Interval& theta, const Interval& phi) {
    const Vector2Interval<Interval> reflected_vertex = Vector2Interval<Interval>(vertex.x(), -vertex.y());
    const Interval shifted_theta = theta + Interval::pi() / 2;
    const Vector2Interval<Interval> rotated_reflected_vertex = rotation_combined(reflected_vertex, shifted_theta);
    return Vector2Interval<Interval>(
        rotated_reflected_vertex.x(),
        harmonic_combined(rotated_reflected_vertex.y(), -vertex.z(), phi)
    );
}

template<IntervalType Interval>
std::vector<Vector2Interval<Interval>> projection_hull_trivial(const Vector3Interval<Interval>& vertex, const Interval& theta, const Interval& phi) {
    const Vector2Interval<Interval> projected_vertex = projection_trivial(vertex, theta, phi);
    return vector2_hull(projected_vertex);
}

template<IntervalType Interval>
std::vector<Vector2Interval<Interval>> projection_hull_combined(const Vector3Interval<Interval>& vertex, const Interval& theta, const Interval& phi) {
    const Vector2Interval<Interval> projected_vertex = projection_combined(vertex, theta, phi);
    return vector2_hull(projected_vertex);
}

template<IntervalType Interval>
std::vector<Vector2Interval<Interval>> projection_hull_triangle(const Vector3Interval<Interval>& vertex, const Interval& theta, const Interval& phi) {
    if(!(theta.len() < Interval::pi() / 3)) {
        // apex of triangle (in rotation_hull_triangle) is not well-defined, or it might be numerically unstable
        // default to combined
        return projection_hull_combined(vertex, theta, phi);
    }

    const Vector2Interval<Interval> reflected_vertex = Vector2Interval<Interval>(vertex.x(), -vertex.y());
    const Interval shifted_theta = theta + Interval::pi() / 2;

    std::vector<Vector2Interval<Interval>> rotation_hull = rotation_hull_triangle(reflected_vertex, shifted_theta);
    if(rotation_hull.size() != 3) {
        throw std::runtime_error("rotation_hull_triangle must return 3 vertices");
    }

    const Vector2Interval<Interval> min_rotated_reflected_vertex = rotation_hull[0];
    const Vector2Interval<Interval> mid_rotated_reflected_vertex = rotation_hull[1];
    const Vector2Interval<Interval> max_rotated_reflected_vertex = rotation_hull[2];

    const Vector2Interval<Interval> scaled_min_rotated_reflected_vertex(
        min_rotated_reflected_vertex.x(),
        harmonic_combined(min_rotated_reflected_vertex.y(), -vertex.z(), phi)
    );
    const Vector2Interval<Interval> scaled_mid_rotated_reflected_vertex(
        mid_rotated_reflected_vertex.x(),
        harmonic_combined(mid_rotated_reflected_vertex.y(), -vertex.z(), phi)
    );
    const Vector2Interval<Interval> scaled_max_rotated_reflected_vertex(
        max_rotated_reflected_vertex.x(),
        harmonic_combined(max_rotated_reflected_vertex.y(), -vertex.z(), phi)
    );

    return std::vector<Vector2Interval<Interval>>{
        Vector2Interval<Interval>(scaled_min_rotated_reflected_vertex.x(), Interval(scaled_min_rotated_reflected_vertex.y().min())),
        Vector2Interval<Interval>(scaled_min_rotated_reflected_vertex.x(), Interval(scaled_min_rotated_reflected_vertex.y().max())),
        Vector2Interval<Interval>(scaled_mid_rotated_reflected_vertex.x(), Interval(scaled_mid_rotated_reflected_vertex.y().min())),
        Vector2Interval<Interval>(scaled_mid_rotated_reflected_vertex.x(), Interval(scaled_mid_rotated_reflected_vertex.y().max())),
        Vector2Interval<Interval>(scaled_max_rotated_reflected_vertex.x(), Interval(scaled_max_rotated_reflected_vertex.y().min())),
        Vector2Interval<Interval>(scaled_max_rotated_reflected_vertex.x(), Interval(scaled_max_rotated_reflected_vertex.y().max()))
    };
}

template<IntervalType Interval>
std::vector<Vector2Interval<Interval>> projection_hull_advanced_approximate(const Vector3Interval<Interval>& vertex, const Interval& theta, const Interval& phi, const int resolution = 30) {
    if(!(theta.len() < Interval::pi() / 3)) {
        return projection_hull_combined(vertex, theta, phi);
    }
    std::vector<Vector2Interval<Interval>> projected_vertices;
    for(int i = 0; i <= resolution; i++) {
        const Interval theta_i = Interval(theta.min()) + Interval(theta.len()) * i / resolution;
        const Vector2Interval<Interval> projected_vertex = projection_combined(vertex, theta_i, Interval(phi.min()));
        projected_vertices.push_back(projected_vertex);
    }
    for(int i = 0; i <= resolution; i++) {
        const Interval phi_i = Interval(phi.min()) + Interval(phi.len()) * i / resolution;
        const Vector2Interval<Interval> projected_vertex = projection_combined(vertex, Interval(theta.max()), phi_i);
        projected_vertices.push_back(projected_vertex);
    }
    for(int i = 0; i <= resolution; i++) {
        const Interval theta_i = Interval(theta.max()) - Interval(theta.len()) * i / resolution;
        const Vector2Interval<Interval> projected_vertex = projection_combined(vertex, theta_i, Interval(phi.max()));
        projected_vertices.push_back(projected_vertex);
    }
    for(int i = 0; i <= resolution; i++) {
        const Interval phi_i = Interval(phi.max()) - Interval(phi.len()) * i / resolution;
        const Vector2Interval<Interval> projected_vertex = projection_combined(vertex, Interval(theta.min()), phi_i);
        projected_vertices.push_back(projected_vertex);
    }
    return projected_vertices;
}

template<IntervalType Interval>
bool is_vector2_inside_polygon(const Vector2Interval<Interval>& vector2, const Polygon<Interval>& polygon) {
    bool all_counter_clockwise = true;
    for(const Edge<Interval>& edge: polygon.edges()) {
        const Orientation orientation = edge.orientation(vector2);
        if(orientation == Orientation::COLLINEAR) {
            return false;
        }
        const bool counter_clockwise = orientation == Orientation::COUNTERCLOCKWISE;
        all_counter_clockwise &= counter_clockwise;
    }
    return all_counter_clockwise;
}

template<IntervalType Interval>
bool is_projected_vertex_inside_polygon_trivial(const Vector3Interval<Interval>& vertex, const Interval& theta, const Interval& phi, const Polygon<Interval>& polygon) {
    const Vector2Interval<Interval> projected_vertex = projection_trivial(vertex, theta, phi);
    return is_vector2_inside_polygon(projected_vertex, polygon);
}

template<IntervalType Interval>
bool is_projected_vertex_inside_polygon_combined(const Vector3Interval<Interval>& vertex, const Interval& theta, const Interval& phi, const Polygon<Interval>& polygon) {
    const Vector2Interval<Interval> projected_vertex = projection_combined(vertex, theta, phi);
    return is_vector2_inside_polygon(projected_vertex, polygon);
}

template<IntervalType Interval>
bool is_vector2_outside_polygon(const Vector2Interval<Interval>& vector2, const Polygon<Interval>& polygon) {
    bool any_clockwise = false;
    for(const Edge<Interval>& edge: polygon.edges()) {
        const Orientation orientation = edge.orientation(vector2);
        if(!edge.avoids(vector2)) {
            return false;
        }
        const bool clockwise = orientation == Orientation::CLOCKWISE;
        any_clockwise |= clockwise;
    }
    return any_clockwise;
}

template<IntervalType Interval>
bool is_projected_vertex_outside_polygon_trivial(const Vector3Interval<Interval>& vertex, const Interval& theta, const Interval& phi, const Polygon<Interval>& polygon) {
    const Vector2Interval<Interval> projected_vertex = projection_trivial(vertex, theta, phi);
    return is_vector2_outside_polygon(projected_vertex, polygon);
}

template<IntervalType Interval>
bool is_projected_vertex_outside_polygon_combined(const Vector3Interval<Interval>& vertex, const Interval& theta, const Interval& phi, const Polygon<Interval>& polygon) {
    const Vector2Interval<Interval> projected_vertex = projection_combined(vertex, theta, phi);
    return is_vector2_outside_polygon(projected_vertex, polygon);
}

template<IntervalType Interval>
bool is_projected_vertex_avoiding_polygon_advanced_fixed_theta(const Vector3Interval<Interval>& vertex, const typename Interval::Number& theta, const Interval& phi, const Polygon<Interval>& polygon) {
    const Vector2Interval<Interval> projected_vertex = projection_combined(vertex, Interval(theta), phi);
    const Edge projected_edge(
        Vector2Interval<Interval>(projected_vertex.x(), Interval(projected_vertex.y().min())),
        Vector2Interval<Interval>(projected_vertex.x(), Interval(projected_vertex.y().max()))
    );
    for(const Edge<Interval>& edge: polygon.edges()) {
        if(!projected_edge.avoids(edge)) {
            return false;
        }
    }
    return true;
}

template<IntervalType Interval>
bool is_projected_vertex_avoiding_edge_advanced_fixed_phi(const Vector3Interval<Interval>& vertex, const Interval& theta, const typename Interval::Number& phi, const Edge<Interval>& edge) {
    const Interval transformation_addition = vertex.z() * Interval(phi).sin();
    const Interval transformation_division = Interval(phi).cos();
    const Edge<Interval> transformed_edge(
        Vector2Interval<Interval>(
            edge.from().x(),
            (edge.from().y() + transformation_addition) / transformation_division
        ),
        Vector2Interval<Interval>(
            edge.to().x(),
            (edge.to().y() + transformation_addition) / transformation_division
        )
    );
    const Vector2Interval<Interval> transformed_edge_direction = transformed_edge.direction();

    const Interval radius_squared = vertex.x() * vertex.x() + vertex.y() * vertex.y();
    const Interval double_quadratic_term = 2 * transformed_edge_direction.len_sqr();
    const Interval linear_term = 2 * Vector2Interval<Interval>::dot(transformed_edge_direction, transformed_edge.from());
    const Interval constant_term = transformed_edge.from().len_sqr() - radius_squared;
    const Interval discriminant = linear_term.sqr() - 2 * double_quadratic_term * constant_term;
    if(!discriminant.is_positive()) {
        return true;
    }
    const Interval sqrt_discriminant = discriminant.sqrt();
    std::array<Interval, 2> solutions = {
        (-linear_term + sqrt_discriminant) / double_quadratic_term,
        (-linear_term - sqrt_discriminant) / double_quadratic_term
    };

    const Vector2Interval<Interval> min_projected_vertex = projection_trivial(vertex, Interval(theta.min()), Interval(phi));
    const Vector2Interval<Interval> max_projected_vertex = projection_trivial(vertex, Interval(theta.max()), Interval(phi));
    const Vector2Interval<Interval> transformed_max_projected_vertex = Vector2Interval<Interval>(
        max_projected_vertex.x(),
        (max_projected_vertex.y() + transformation_addition) / transformation_division
    );
    const Edge<Interval> projected_vertex_edge(
        Vector2Interval<Interval>(
            min_projected_vertex.x(),
            (min_projected_vertex.y() + transformation_addition) / transformation_division
        ),
        Vector2Interval<Interval>(
            max_projected_vertex.x(),
            (max_projected_vertex.y() + transformation_addition) / transformation_division
        )
    );
    for(const Interval& solution: solutions) {
        if(solution.is_negative() || solution > 1) {
            continue;
        }
        const Vector2Interval<Interval> intersection(
            transformed_edge.from().x() + solution * transformed_edge_direction.x(),
            transformed_edge.from().y() + solution * transformed_edge_direction.y()
        );
        if(!projected_vertex_edge.avoids(Edge<Interval>(Vector2Interval<Interval>(Interval(0), Interval(0)), intersection))) {
            return false;
        }
    }
    return true;
}

template<IntervalType Interval>
bool is_projected_vertex_avoiding_polygon_advanced_fixed_phi(const Vector3Interval<Interval>& vertex, const Interval& theta, const typename Interval::Number& phi, const Polygon<Interval>& polygon) {
    if(!Interval(phi).cos().is_nonzero()) {
        // degenerate case, division by zero would occur in the inverse transformation
        // default to combined
        return is_projected_vertex_outside_polygon_combined(vertex, theta, Interval(phi), polygon);
    }
    for(const Edge<Interval>& edge: polygon.edges()) {
        if(!is_projected_vertex_avoiding_edge_advanced_fixed_phi(vertex, theta, phi, edge)) {
            return false;
        }
    }
    return true;
}

template<IntervalType Interval>
bool is_projected_vertex_outside_polygon_advanced(const Vector3Interval<Interval>& vertex, const Interval& theta, const Interval& phi, const Polygon<Interval>& polygon) {
    if(!(theta.len() < Interval::pi() / 3)) {
        // projected vertex might surround the polygon, meaning it avoids, but is not outside
        // default to combined
        return is_projected_vertex_outside_polygon_combined(vertex, theta, phi, polygon);
    }
    return is_projected_vertex_outside_polygon_combined(vertex, Interval(theta.mid()), Interval(phi.mid()), polygon) &&
           is_projected_vertex_avoiding_polygon_advanced_fixed_theta(vertex, theta.min(), phi, polygon) &&
           is_projected_vertex_avoiding_polygon_advanced_fixed_theta(vertex, theta.max(), phi, polygon) &&
           is_projected_vertex_avoiding_polygon_advanced_fixed_phi(vertex, theta, phi.min(), polygon) &&
           is_projected_vertex_avoiding_polygon_advanced_fixed_phi(vertex, theta, phi.max(), polygon);
}

template<IntervalType Interval>
Polygon<Interval> convex_hull(const std::vector<Vector2Interval<Interval>>& vertices, const typename Interval::Number& epsilon) {
    std::vector<Edge<Interval>> edges;

    std::optional<size_t> start_index;
    for(size_t new_start_index = 0; new_start_index < vertices.size(); new_start_index++) {
        if(!start_index.has_value() || vertices[new_start_index].x().max() > vertices[start_index.value()].x().max()) {
            start_index = new_start_index;
        }
    }

    std::vector<bool> visited_indices(vertices.size(), false);
    std::queue<std::pair<size_t, Vector2Interval<Interval>>> queue({{start_index.value(), vertices[start_index.value()] * Interval(epsilon)}});
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
            const Vector2Interval<Interval> to_vertex = vertices[to_index];
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
            const Vector2Interval<Interval> to_vertex = vertices[to_index] * Interval(epsilon);
            edges.emplace_back(from_vertex, to_vertex);
            if(!visited_indices[to_index]) {
                queue.push({to_index, vertices[to_index] * Interval(epsilon)});
            }
        }
    }
    return Polygon(edges);
}
