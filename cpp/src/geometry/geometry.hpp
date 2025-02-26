#pragma once

#include "geometry/edge.hpp"
#include "geometry/polygon.hpp"
#include "geometry/polyhedron.hpp"
#include <queue>
#include <optional>

template<IntervalType Interval>
std::vector<Vector2Interval<Interval>> vector2_hull(const Vector2Interval<Interval> &vector2) {
    return std::vector<Vector2Interval<Interval>>{
                Vector2Interval<Interval>(Interval(vector2.x().min()), Interval(vector2.y().min())),
                Vector2Interval<Interval>(Interval(vector2.x().min()), Interval(vector2.y().max())),
                Vector2Interval<Interval>(Interval(vector2.x().max()), Interval(vector2.y().max())),
                Vector2Interval<Interval>(Interval(vector2.x().max()), Interval(vector2.y().min()))
            };
}

template<IntervalType Interval>
Vector2Interval<Interval> rotation(const Vector2Interval<Interval> &projected_vertex, const Interval &alpha) {
    const Interval cos_alpha = alpha.cos();
    const Interval sin_alpha = alpha.sin();
    return Vector2Interval<Interval>(
        projected_vertex.x() * cos_alpha - projected_vertex.y() * sin_alpha,
        projected_vertex.x() * sin_alpha + projected_vertex.y() * cos_alpha
    );
}

template<IntervalType Interval>
std::vector<Vector2Interval<Interval>> rotation_hull_trivial(const Vector2Interval<Interval> &projected_vertex, const Interval &alpha) {
    const Vector2Interval<Interval> rotated_vertex = rotation(projected_vertex, alpha);
    return vector2_hull(rotated_vertex);
}

template<IntervalType Interval>
std::vector<Vector2Interval<Interval>> rotation_hull_triangle(const Vector2Interval<Interval> &vector2, const Interval &alpha) {
    if(!(alpha.len() < Interval::pi() / 2)) {
        throw std::invalid_argument("rotation_hull_triangle only supports |alpha| < pi/2");
    }

    const Interval cos_alpha_min = Interval(alpha.min()).cos();
    const Interval cos_alpha_max = Interval(alpha.max()).cos();
    const Interval cos_alpha_mid = Interval(alpha.mid()).cos();

    const Interval sin_alpha_min = Interval(alpha.min()).sin();
    const Interval sin_alpha_max = Interval(alpha.max()).sin();
    const Interval sin_alpha_mid = Interval(alpha.mid()).sin();

    const Interval cos_alpha_rad = Interval(alpha.rad()).cos();

    Vector2Interval<Interval> min_rotated_vertex(
        vector2.x() * cos_alpha_min - vector2.y() * sin_alpha_min,
        vector2.x() * sin_alpha_min + vector2.y() * cos_alpha_min
    );
    Vector2Interval<Interval> max_rotated_vertex(
        vector2.x() * cos_alpha_max - vector2.y() * sin_alpha_max,
        vector2.x() * sin_alpha_max + vector2.y() * cos_alpha_max
    );
    Vector2Interval<Interval> mid_rotated_vertex(
        (vector2.x() * cos_alpha_mid - vector2.y() * sin_alpha_mid) / cos_alpha_rad,
        (vector2.x() * sin_alpha_mid + vector2.y() * cos_alpha_mid) / cos_alpha_rad
    );
    return std::vector<Vector2Interval<Interval>>{
                min_rotated_vertex,
                max_rotated_vertex,
                mid_rotated_vertex
            };
}

template<IntervalType Interval>
Vector2Interval<Interval> projection(const IntervalVector3<Interval> &vertex, const Interval &theta, const Interval &phi) {
    const Interval cos_theta = theta.cos();
    const Interval sin_theta = theta.sin();
    const Interval cos_phi = phi.cos();
    const Interval sin_phi = phi.sin();
    return Vector2Interval<Interval>(
        -vertex.x() * sin_phi + vertex.y() * cos_phi,
        (vertex.x() * cos_phi + vertex.y() * sin_phi) * cos_theta - vertex.z() * sin_theta
    );
}

template<IntervalType Interval>
std::vector<Vector2Interval<Interval>> projection_hull_trivial(const IntervalVector3<Interval> &vertex, const Interval &theta, const Interval &phi) {
    const Vector2Interval<Interval> projected_vertex = projection(vertex, theta, phi);
    return vector2_hull(projected_vertex);
}

template<IntervalType Interval>
std::vector<Vector2Interval<Interval>> projection_hull_triangle(const IntervalVector3<Interval> &vertex, const Interval &theta, const Interval &phi) {
    if(!(phi.len() < Interval::pi() / 2)) {
        throw std::invalid_argument("rotation_hull_triangle only supports |phi| < pi/2");
    }

    const Vector2Interval<Interval> reflected_vertex = Vector2Interval<Interval>(vertex.x(), -vertex.y());
    const Interval shifted_phi = phi + Interval::pi() / 2;

    const Interval cos_shifted_phi_min = Interval(shifted_phi.min()).cos();
    const Interval cos_shifted_phi_max = Interval(shifted_phi.max()).cos();
    const Interval cos_shifted_phi_mid = Interval(shifted_phi.mid()).cos();

    const Interval sin_shifted_phi_min = Interval(shifted_phi.min()).sin();
    const Interval sin_shifted_phi_max = Interval(shifted_phi.max()).sin();
    const Interval sin_shifted_phi_mid = Interval(shifted_phi.mid()).sin();

    const Interval cos_shifted_phi_rad = Interval(shifted_phi.rad()).cos();

    const Vector2Interval<Interval> min_rotated_reflected_vertex(
        reflected_vertex.x() * cos_shifted_phi_min - reflected_vertex.y() * sin_shifted_phi_min,
        reflected_vertex.x() * sin_shifted_phi_min + reflected_vertex.y() * cos_shifted_phi_min
    );
    const Vector2Interval<Interval> max_rotated_reflected_vertex(
        reflected_vertex.x() * cos_shifted_phi_max - reflected_vertex.y() * sin_shifted_phi_max,
        reflected_vertex.x() * sin_shifted_phi_max + reflected_vertex.y() * cos_shifted_phi_max
    );
    const Vector2Interval<Interval> mid_rotated_reflected_vertex(
        (reflected_vertex.x() * cos_shifted_phi_mid - reflected_vertex.y() * sin_shifted_phi_mid) / cos_shifted_phi_rad,
        (reflected_vertex.x() * sin_shifted_phi_mid + reflected_vertex.y() * cos_shifted_phi_mid) / cos_shifted_phi_rad
    );

    const Interval cos_theta = theta.cos();
    const Interval sin_theta = theta.sin();

    const Vector2Interval<Interval> scaled_min_rotated_reflected_vertex(
        min_rotated_reflected_vertex.x(),
        min_rotated_reflected_vertex.y() * cos_theta - vertex.z() * sin_theta
    );
    const Vector2Interval<Interval> scaled_max_rotated_reflected_vertex(
        max_rotated_reflected_vertex.x(),
        max_rotated_reflected_vertex.y() * cos_theta - vertex.z() * sin_theta
    );
    const Vector2Interval<Interval> scaled_mid_rotated_reflected_vertex(
        mid_rotated_reflected_vertex.x(),
        mid_rotated_reflected_vertex.y() * cos_theta - vertex.z() * sin_theta
    );

    return std::vector<Vector2Interval<Interval>>{
                Vector2Interval<Interval>(scaled_min_rotated_reflected_vertex.x(), Interval(scaled_min_rotated_reflected_vertex.y().min())),
                Vector2Interval<Interval>(scaled_min_rotated_reflected_vertex.x(), Interval(scaled_min_rotated_reflected_vertex.y().max())),
                Vector2Interval<Interval>(scaled_max_rotated_reflected_vertex.x(), Interval(scaled_max_rotated_reflected_vertex.y().min())),
                Vector2Interval<Interval>(scaled_max_rotated_reflected_vertex.x(), Interval(scaled_max_rotated_reflected_vertex.y().max())),
                Vector2Interval<Interval>(scaled_mid_rotated_reflected_vertex.x(), Interval(scaled_mid_rotated_reflected_vertex.y().min())),
                Vector2Interval<Interval>(scaled_mid_rotated_reflected_vertex.x(), Interval(scaled_mid_rotated_reflected_vertex.y().max()))
            };
}

template<IntervalType Interval>
bool is_vector2_inside_polygon(const Vector2Interval<Interval> &vector2, const Polygon<Interval> &polygon) {
    bool all_counter_clockwise = true;
    for(const Edge<Interval> &edge: polygon.edges()) {
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
bool is_projected_vertex_inside_polygon_trivial(const IntervalVector3<Interval> &vertex, const Interval &theta, const Interval &phi, const Polygon<Interval> &polygon) {
    const Vector2Interval<Interval> projected_vertex = projection(vertex, theta, phi);
    return is_vector2_inside_polygon(projected_vertex, polygon);
}

template<IntervalType Interval>
bool is_vector2_outside_polygon(const Vector2Interval<Interval> &vector2, const Polygon<Interval> &polygon) {
    bool any_clockwise = false;
    for(const Edge<Interval> &edge: polygon.edges()) {
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
bool is_projected_vertex_outside_polygon_trivial(const IntervalVector3<Interval> &vertex, const Interval &theta, const Interval &phi, const Polygon<Interval> &polygon) {
    const Vector2Interval<Interval> projected_vertex = projection(vertex, theta, phi);
    return is_vector2_outside_polygon(projected_vertex, polygon);
}

template<IntervalType Interval>
Polygon<Interval> convex_hull(const std::vector<Vector2Interval<Interval>> &vertices) {
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
