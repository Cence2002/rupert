#pragma once

#include "geometry/util.hpp"

template<IntervalType Interval>
std::vector<Vector2<Interval>> rotation_hull_trivial(const Vector2<Interval>& projected_vertex, const Interval& alpha) {
    const Vector2<Interval> rotated_vertex = rotation_trivial(projected_vertex, alpha);
    return vector2_hull(rotated_vertex);
}

template<IntervalType Interval>
std::vector<Vector2<Interval>> rotation_hull_combined(const Vector2<Interval>& projected_vertex, const Interval& alpha) {
    const Vector2<Interval> rotated_vertex = rotation_combined(projected_vertex, alpha);
    return vector2_hull(rotated_vertex);
}

template<IntervalType Interval>
std::vector<Vector2<Interval>> rotation_hull_triangle(const Vector2<Interval>& vector2, const Interval& alpha) {
    if(!(alpha.len() < Interval::pi() / 3)) {
        // apex of triangle is not well-defined, or it might be numerically unstable
        // default to combined
        return rotation_hull_combined(vector2, alpha);
    }

    Vector2<Interval> min_rotated_vertex = rotation_trivial(vector2, Interval(alpha.min()));
    Vector2<Interval> mid_rotated_vertex = rotation_trivial(vector2, Interval(alpha.mid()));
    Vector2<Interval> max_rotated_vertex = rotation_trivial(vector2, Interval(alpha.max()));

    const Interval cos_alpha_rad = Interval(alpha.rad()).cos();
    Vector2<Interval> mid_rotated_vertex_scaled(
        mid_rotated_vertex.x() / cos_alpha_rad,
        mid_rotated_vertex.y() / cos_alpha_rad
    );

    return std::vector<Vector2<Interval>>{
        min_rotated_vertex,
        mid_rotated_vertex,
        max_rotated_vertex
    };
}

template<IntervalType Interval>
std::vector<Vector2<Interval>> projection_hull_trivial(const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    const Vector2<Interval> projected_vertex = projection_trivial(vertex, theta, phi);
    return vector2_hull(projected_vertex);
}

template<IntervalType Interval>
std::vector<Vector2<Interval>> projection_hull_combined(const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    const Vector2<Interval> projected_vertex = projection_combined(vertex, theta, phi);
    return vector2_hull(projected_vertex);
}

template<IntervalType Interval>
std::vector<Vector2<Interval>> projection_hull_triangle(const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    if(!(theta.len() < Interval::pi() / 3)) {
        // apex of triangle (in rotation_hull_triangle) is not well-defined, or it might be numerically unstable
        // default to combined
        return projection_hull_combined(vertex, theta, phi);
    }

    const Vector2<Interval> reflected_vertex = Vector2<Interval>(vertex.x(), -vertex.y());
    const Interval shifted_theta = theta + Interval::pi() / 2;

    std::vector<Vector2<Interval>> rotation_hull = rotation_hull_triangle(reflected_vertex, shifted_theta);
    if(rotation_hull.size() != 3) {
        throw std::runtime_error("rotation_hull_triangle must return 3 vertices");
    }

    const Vector2<Interval> min_rotated_reflected_vertex = rotation_hull[0];
    const Vector2<Interval> mid_rotated_reflected_vertex = rotation_hull[1];
    const Vector2<Interval> max_rotated_reflected_vertex = rotation_hull[2];

    const Vector2<Interval> scaled_min_rotated_reflected_vertex(
        min_rotated_reflected_vertex.x(),
        harmonic_combined(min_rotated_reflected_vertex.y(), -vertex.z(), phi)
    );
    const Vector2<Interval> scaled_mid_rotated_reflected_vertex(
        mid_rotated_reflected_vertex.x(),
        harmonic_combined(mid_rotated_reflected_vertex.y(), -vertex.z(), phi)
    );
    const Vector2<Interval> scaled_max_rotated_reflected_vertex(
        max_rotated_reflected_vertex.x(),
        harmonic_combined(max_rotated_reflected_vertex.y(), -vertex.z(), phi)
    );

    return std::vector<Vector2<Interval>>{
        Vector2<Interval>(scaled_min_rotated_reflected_vertex.x(), Interval(scaled_min_rotated_reflected_vertex.y().min())),
        Vector2<Interval>(scaled_min_rotated_reflected_vertex.x(), Interval(scaled_min_rotated_reflected_vertex.y().max())),
        Vector2<Interval>(scaled_mid_rotated_reflected_vertex.x(), Interval(scaled_mid_rotated_reflected_vertex.y().min())),
        Vector2<Interval>(scaled_mid_rotated_reflected_vertex.x(), Interval(scaled_mid_rotated_reflected_vertex.y().max())),
        Vector2<Interval>(scaled_max_rotated_reflected_vertex.x(), Interval(scaled_max_rotated_reflected_vertex.y().min())),
        Vector2<Interval>(scaled_max_rotated_reflected_vertex.x(), Interval(scaled_max_rotated_reflected_vertex.y().max()))
    };
}

template<IntervalType Interval>
std::vector<Vector2<Interval>> projection_hull_advanced_approximate(const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi, const int resolution = 8) {
    if(!(theta.len() < Interval::pi() / 3)) {
        return projection_hull_combined(vertex, theta, phi);
    }
    std::vector<Vector2<Interval>> projected_vertices;
    for(int i = 0; i <= resolution; i++) {
        const Interval theta_i = Interval(theta.min()) + Interval(theta.len()) * i / resolution;
        const Vector2<Interval> projected_vertex = projection_combined(vertex, theta_i, Interval(phi.min()));
        projected_vertices.push_back(projected_vertex);
    }
    for(int i = 0; i <= resolution; i++) {
        const Interval phi_i = Interval(phi.min()) + Interval(phi.len()) * i / resolution;
        const Vector2<Interval> projected_vertex = projection_combined(vertex, Interval(theta.max()), phi_i);
        projected_vertices.push_back(projected_vertex);
    }
    for(int i = 0; i <= resolution; i++) {
        const Interval theta_i = Interval(theta.max()) - Interval(theta.len()) * i / resolution;
        const Vector2<Interval> projected_vertex = projection_combined(vertex, theta_i, Interval(phi.max()));
        projected_vertices.push_back(projected_vertex);
    }
    for(int i = 0; i <= resolution; i++) {
        const Interval phi_i = Interval(phi.max()) - Interval(phi.len()) * i / resolution;
        const Vector2<Interval> projected_vertex = projection_combined(vertex, Interval(theta.min()), phi_i);
        projected_vertices.push_back(projected_vertex);
    }
    return projected_vertices;
}