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
        const Interval amplitude = (cos_amplitude * cos_amplitude + sin_amplitude * sin_amplitude).sqrt();
        const Interval phase = (sin_amplitude / cos_amplitude).atan();
        const int sign = cos_amplitude.is_pos() ? 1 : -1;
        return sign * amplitude * (angle - phase).cos();
    }
    if(sin_amplitude.is_nonzero()) {
        const Interval amplitude = (cos_amplitude * cos_amplitude + sin_amplitude * sin_amplitude).sqrt();
        const Interval phase = -(cos_amplitude / sin_amplitude).atan();
        const int sign = sin_amplitude.is_pos() ? 1 : -1;
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

template<IntervalType Interval>
Vector2Interval<Interval> rotation_trivial(const Vector2Interval<Interval>& projected_vertex, const Interval& alpha) {
    // X = x * cos(alpha) - y * sin(alpha)
    // Y = x * sin(alpha) + y * cos(alpha)
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
    if(!(alpha.len() < Interval::pi() / 4)) {
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

template<IntervalType Interval>
Vector2Interval<Interval> projection_trivial(const Vector3Interval<Interval>& vertex, const Interval& theta, const Interval& phi) {
    // X = -x * sin(phi) + y * cos(phi)
    // Y = (x * cos(phi) + y * sin(phi)) * cos(theta) - z * sin(theta)
    const Vector2Interval<Interval> reflected_vertex = Vector2Interval<Interval>(vertex.x(), -vertex.y());
    const Interval shifted_phi = phi + Interval::pi() / 2;
    const Vector2Interval<Interval> rotated_reflected_vertex = rotation_trivial(reflected_vertex, shifted_phi);
    return Vector2Interval<Interval>(
        rotated_reflected_vertex.x(),
        harmonic_trivial(rotated_reflected_vertex.y(), -vertex.z(), theta)
    );
}

template<IntervalType Interval>
Vector2Interval<Interval> projection_combined(const Vector3Interval<Interval>& vertex, const Interval& theta, const Interval& phi) {
    const Vector2Interval<Interval> reflected_vertex = Vector2Interval<Interval>(vertex.x(), -vertex.y());
    const Interval shifted_phi = phi + Interval::pi() / 2;
    const Vector2Interval<Interval> rotated_reflected_vertex = rotation_combined(reflected_vertex, shifted_phi);
    return Vector2Interval<Interval>(
        rotated_reflected_vertex.x(),
        harmonic_combined(rotated_reflected_vertex.y(), -vertex.z(), theta)
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
    if(!(phi.len() < Interval::pi() / 4)) {
        // apex of triangle (in rotation_hull_triangle) is not well-defined, or it might be numerically unstable
        // default to combined
        return projection_hull_combined(vertex, theta, phi);
    }

    const Vector2Interval<Interval> reflected_vertex = Vector2Interval<Interval>(vertex.x(), -vertex.y());
    const Interval shifted_phi = phi + Interval::pi() / 2;

    std::vector<Vector2Interval<Interval>> rotation_hull = rotation_hull_triangle(reflected_vertex, shifted_phi);
    if(rotation_hull.size() != 3) {
        throw std::runtime_error("rotation_hull_triangle must return 3 vertices");
    }

    const Vector2Interval<Interval> min_rotated_reflected_vertex = rotation_hull[0];
    const Vector2Interval<Interval> mid_rotated_reflected_vertex = rotation_hull[1];
    const Vector2Interval<Interval> max_rotated_reflected_vertex = rotation_hull[2];

    const Vector2Interval<Interval> scaled_min_rotated_reflected_vertex(
        min_rotated_reflected_vertex.x(),
        harmonic_combined(min_rotated_reflected_vertex.y(), -vertex.z(), theta)
    );
    const Vector2Interval<Interval> scaled_mid_rotated_reflected_vertex(
        mid_rotated_reflected_vertex.x(),
        harmonic_combined(mid_rotated_reflected_vertex.y(), -vertex.z(), theta)
    );
    const Vector2Interval<Interval> scaled_max_rotated_reflected_vertex(
        max_rotated_reflected_vertex.x(),
        harmonic_combined(max_rotated_reflected_vertex.y(), -vertex.z(), theta)
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
bool is_projected_vertex_avoiding_polygon_advanced_fixed_phi(const Vector3Interval<Interval>& vertex, const Interval& theta, const typename Interval::Number& phi, const Polygon<Interval>& polygon) {
    const Vector2Interval<Interval> projected_vertex = projection_combined(vertex, theta, Interval(phi));
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
bool is_projected_vertex_avoiding_edge_advanced_fixed_theta(const Vector3Interval<Interval>& vertex, const typename Interval::Number& theta, const Interval& phi, const Edge<Interval>& edge) {
    const Interval transformation_addition_scale = vertex.z() * Interval(theta).sin();
    const Interval transformation_division_scale = Interval(theta).cos();
    const Edge<Interval> transformed_edge(
        Vector2Interval<Interval>(
            edge.from().x(),
            (edge.from().y() + transformation_addition_scale) / transformation_division_scale
        ),
        Vector2Interval<Interval>(
            edge.to().x(),
            (edge.to().y() + transformation_addition_scale) / transformation_division_scale
        )
    );
    const Vector2Interval<Interval> edge_direction = transformed_edge.direction();

    const Interval radius_squared = vertex.x() * vertex.x() + vertex.y() * vertex.y();
    const Interval quadratic_term = edge_direction.len_sqr();
    const Interval linear_term = 2 * Vector2Interval<Interval>::dot(edge_direction, transformed_edge.from());
    const Interval constant_term = transformed_edge.from().len_sqr() - radius_squared;
    const Interval discriminant = linear_term * linear_term - 4 * quadratic_term * constant_term;
    if(discriminant.is_neg()) {
        return true;
    }
    const Interval sqrt_discriminant = discriminant.sqrt();
    std::array<Interval, 2> solutions = {
        (-linear_term + sqrt_discriminant) / (2 * quadratic_term),
        (-linear_term - sqrt_discriminant) / (2 * quadratic_term)
    };

    const Vector2Interval<Interval> min_projected_vertex = projection_trivial(vertex, Interval(theta), Interval(phi.min()));
    const Vector2Interval<Interval> max_projected_vertex = projection_trivial(vertex, Interval(theta), Interval(phi.max()));
    const Vector2Interval<Interval> transformed_max_projected_vertex = Vector2Interval<Interval>(
        max_projected_vertex.x(),
        (max_projected_vertex.y() + transformation_addition_scale) / transformation_division_scale
    );
    const Edge<Interval> projected_vertex_edge(
        Vector2Interval<Interval>(
            min_projected_vertex.x(),
            (min_projected_vertex.y() + transformation_addition_scale) / transformation_division_scale
        ),
        Vector2Interval<Interval>(
            max_projected_vertex.x(),
            (max_projected_vertex.y() + transformation_addition_scale) / transformation_division_scale
        )
    );
    for(const Interval& solution: solutions) {
        if(solution.is_neg() || solution > 1) {
            continue;
        }
        const Vector2Interval<Interval> intersection(
            transformed_edge.from().x() + solution * edge_direction.x(),
            transformed_edge.from().y() + solution * edge_direction.y()
        );
        if(!projected_vertex_edge.avoids(Edge<Interval>(Vector2Interval<Interval>(Interval(0), Interval(0)), intersection))) {
            return false;
        }
    }
    return true;
}

template<IntervalType Interval>
bool is_projected_vertex_avoiding_polygon_advanced_fixed_theta(const Vector3Interval<Interval>& vertex, const typename Interval::Number& theta, const Interval& phi, const Polygon<Interval>& polygon) {
    if(!Interval(theta).cos().is_nonzero()) {
        // degenerate case, division by zero would occur in the inverse transformation
        // default to combined
        return is_projected_vertex_outside_polygon_combined(vertex, Interval(theta), phi, polygon);
    }
    for(const Edge<Interval>& edge: polygon.edges()) {
        if(!is_projected_vertex_avoiding_edge_advanced_fixed_theta(vertex, theta, phi, edge)) {
            return false;
        }
    }
    return true;
}

template<IntervalType Interval>
bool is_projected_vertex_outside_polygon_advanced(const Vector3Interval<Interval>& vertex, const Interval& theta, const Interval& phi, const Polygon<Interval>& polygon) {
    if(!(theta.len() < Interval::pi() / 4) || !(phi.len() < Interval::pi() / 4)) {
        // projected vertex might surround the polygon, meaning it avoids, but is not outside
        // default to combined
        return is_projected_vertex_outside_polygon_combined(vertex, theta, phi, polygon);
    }
    return is_projected_vertex_outside_polygon_combined(vertex, Interval(theta.mid()), Interval(phi.mid()), polygon) &&
           is_projected_vertex_avoiding_polygon_advanced_fixed_phi(vertex, theta, phi.min(), polygon) &&
           is_projected_vertex_avoiding_polygon_advanced_fixed_phi(vertex, theta, phi.max(), polygon) &&
           is_projected_vertex_avoiding_polygon_advanced_fixed_theta(vertex, theta.min(), phi, polygon) &&
           is_projected_vertex_avoiding_polygon_advanced_fixed_theta(vertex, theta.max(), phi, polygon);
}

template<IntervalType Interval>
Polygon<Interval> convex_hull(const std::vector<Vector2Interval<Interval>>& vertices) {
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

        const Vector2Interval<Interval> from = vertices[from_index];

        std::optional<size_t> most_clockwise_index;
        for(size_t clockwise_index = 0; clockwise_index < vertices.size(); clockwise_index++) {
            if(!most_clockwise_index.has_value() && (vertices[clockwise_index] - from).len().is_pos()) {
                most_clockwise_index = static_cast<int>(clockwise_index);
            }
            if(most_clockwise_index.has_value() && Edge<Interval>(from, vertices[most_clockwise_index.value()]).orientation(vertices[clockwise_index]) == Orientation::CLOCKWISE) {
                most_clockwise_index = static_cast<int>(clockwise_index);
            }
        }

        std::vector<size_t> to_indices;
        for(size_t to_index = 0; to_index < vertices.size(); to_index++) {
            if(!(vertices[to_index] - from).len().is_pos()) {
                continue;
            }
            const Vector2Interval<Interval> to = vertices[to_index];
            if(Edge<Interval>(from, vertices[most_clockwise_index.value()]).orientation(to) == Orientation::COUNTERCLOCKWISE) {
                continue;
            }
            bool most_clockwise = true;
            for(size_t clockwise_index = 0; clockwise_index < vertices.size(); clockwise_index++) {
                if(clockwise_index == from_index || clockwise_index == to_index) {
                    continue;
                }
                if(Edge<Interval>(from, to).orientation(vertices[clockwise_index]) == Orientation::CLOCKWISE) {
                    most_clockwise = false;
                    break;
                }
            }
            if(most_clockwise) {
                to_indices.push_back(to_index);
            }
        }
        std::ranges::stable_sort(to_indices, [&](const size_t a, const size_t b) {
            return (vertices[a] - from).len().mid() < (vertices[b] - from).len().mid();
        });
        for(const size_t to_index: to_indices) {
            edges.emplace_back(vertices[from_index], vertices[to_index]);
            if(!visited[to_index]) {
                queue.push(to_index);
            }
        }
    }
    return Polygon(edges);
}
