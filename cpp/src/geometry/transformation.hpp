#pragma once

#include "geometry/vector.hpp"
#include "geometry/vertex.hpp"
#include <vector>

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

// (X, Y) = R(alpha) * (x, y)
// X = x * cos(alpha) - y * sin(alpha)
// Y = x * sin(alpha) + y * cos(alpha)

template<IntervalType Interval>
Vector<Interval> rotation_trivial(const Vector<Interval>& vector, const Interval& alpha) {
    return Vector<Interval>(
        harmonic_trivial(vector.x(), -vector.y(), alpha),
        harmonic_trivial(vector.y(), vector.x(), alpha)
    );
}

template<IntervalType Interval>
Vector<Interval> rotation_combined(const Vector<Interval>& vector, const Interval& alpha) {
    return Vector<Interval>(
        harmonic_combined(vector.x(), -vector.y(), alpha),
        harmonic_combined(vector.y(), vector.x(), alpha)
    );
}

// (X, Y, _) = Rx(phi) * Rz(theta) * (x, y, z)
// X = x * cos(theta) - y * sin(theta)
// Y = (y * cos(theta) + x * sin(theta)) * cos(phi) - z * sin(phi)

template<IntervalType Interval>
Vector<Interval> projection_trivial(const Vertex<Interval>& vertex, const Interval& theta, const Interval& phi) {
    return Vector<Interval>(
        harmonic_trivial(vertex.x(), -vertex.y(), theta),
        harmonic_trivial(harmonic_trivial(vertex.y(), vertex.x(), theta), -vertex.z(), phi)
    );
}

template<IntervalType Interval>
Vector<Interval> projection_combined(const Vertex<Interval>& vertex, const Interval& theta, const Interval& phi) {
    return Vector<Interval>(
        harmonic_combined(vertex.x(), -vertex.y(), theta),
        harmonic_combined(harmonic_combined(vertex.y(), vertex.x(), theta), -vertex.z(), phi)
    );
}

template<IntervalType Interval>
std::vector<Vector<Interval>> vector_hull(const Vector<Interval>& vector) {
    return std::vector<Vector<Interval>>{
        Vector<Interval>(Interval(vector.x().min()), Interval(vector.y().min())),
        Vector<Interval>(Interval(vector.x().min()), Interval(vector.y().max())),
        Vector<Interval>(Interval(vector.x().max()), Interval(vector.y().max())),
        Vector<Interval>(Interval(vector.x().max()), Interval(vector.y().min()))
    };
}

template<IntervalType Interval>
std::vector<Vector<Interval>> rotation_hull_trivial(const Vector<Interval>& projected_vertex, const Interval& alpha) {
    const Vector<Interval> rotated_vertex = rotation_trivial(projected_vertex, alpha);
    return vector_hull(rotated_vertex);
}

template<IntervalType Interval>
std::vector<Vector<Interval>> rotation_hull_combined(const Vector<Interval>& projected_vertex, const Interval& alpha) {
    const Vector<Interval> rotated_vertex = rotation_combined(projected_vertex, alpha);
    return vector_hull(rotated_vertex);
}

template<IntervalType Interval>
std::vector<Vector<Interval>> rotation_hull_triangle(const Vector<Interval>& vector, const Interval& alpha) {
    if(alpha.len() > Interval::pi() / 3) {
        // apex of triangle is not well-defined, or it might be numerically unstable
        // default to combined
        return rotation_hull_combined(vector, alpha);
    }

    Vector<Interval> min_rotated_vertex = rotation_trivial(vector, Interval(alpha.min()));
    Vector<Interval> mid_rotated_vertex = rotation_trivial(vector, Interval(alpha.mid()));
    Vector<Interval> max_rotated_vertex = rotation_trivial(vector, Interval(alpha.max()));

    const Interval cos_alpha_rad = Interval(alpha.rad()).cos();
    Vector<Interval> mid_rotated_vertex_scaled(
        mid_rotated_vertex.x() / cos_alpha_rad,
        mid_rotated_vertex.y() / cos_alpha_rad
    );

    return std::vector<Vector<Interval>>{
        min_rotated_vertex,
        mid_rotated_vertex_scaled,
        max_rotated_vertex
    };
}

template<IntervalType Interval>
std::vector<Vector<Interval>> rotation_hull_polygon(const Vector<Interval>& vector, const Interval& alpha, const int resolution) {
    if(alpha.len() > Interval::pi() / 3 * resolution) {
        return rotation_hull_combined(vector, alpha);
    }
    std::vector<Vector<Interval>> rotated_vertices;
    const Interval scaling_factor = 1 / (Interval(alpha.rad()) / resolution).cos();
    for(int i = 0; i <= resolution; i++) {
        const Interval alpha_i = Interval(alpha.min()) * (resolution - i) / resolution + Interval(alpha.max()) * i / resolution;
        const Vector<Interval> rotated_vertex = rotation_trivial(vector, alpha_i);
        const Vector<Interval> scaled_rotated_vertex = rotated_vertex * scaling_factor;
        rotated_vertices.push_back(scaled_rotated_vertex);
    }
    return rotated_vertices;
}

template<IntervalType Interval>
std::vector<Vector<Interval>> projection_hull_trivial(const Vertex<Interval>& vertex, const Interval& theta, const Interval& phi) {
    const Vector<Interval> projected_vertex = projection_trivial(vertex, theta, phi);
    return vector_hull(projected_vertex);
}

template<IntervalType Interval>
std::vector<Vector<Interval>> projection_hull_combined(const Vertex<Interval>& vertex, const Interval& theta, const Interval& phi) {
    const Vector<Interval> projected_vertex = projection_combined(vertex, theta, phi);
    return vector_hull(projected_vertex);
}

template<IntervalType Interval>
std::vector<Vector<Interval>> projection_hull_triangle(const Vertex<Interval>& vertex, const Interval& theta, const Interval& phi) {
    if(theta.len() > Interval::pi() / 3) {
        // apex of triangle (in rotation_hull_triangle) is not well-defined, or it might be numerically unstable
        // default to combined
        return projection_hull_combined(vertex, theta, phi);
    }

    std::vector<Vector<Interval>> rotation_hull = rotation_hull_triangle(vertex, theta);
    if(rotation_hull.size() != 3) {
        throw std::runtime_error("rotation_hull_triangle must return 3 vertices");
    }

    const Vector<Interval> min_rotated_reflected_vertex = rotation_hull[0];
    const Vector<Interval> mid_rotated_reflected_vertex = rotation_hull[1];
    const Vector<Interval> max_rotated_reflected_vertex = rotation_hull[2];

    const Vector<Interval> scaled_min_rotated_reflected_vertex(
        min_rotated_reflected_vertex.x(),
        harmonic_combined(min_rotated_reflected_vertex.y(), -vertex.z(), phi)
    );
    const Vector<Interval> scaled_mid_rotated_reflected_vertex(
        mid_rotated_reflected_vertex.x(),
        harmonic_combined(mid_rotated_reflected_vertex.y(), -vertex.z(), phi)
    );
    const Vector<Interval> scaled_max_rotated_reflected_vertex(
        max_rotated_reflected_vertex.x(),
        harmonic_combined(max_rotated_reflected_vertex.y(), -vertex.z(), phi)
    );

    return std::vector<Vector<Interval>>{
        Vector<Interval>(scaled_min_rotated_reflected_vertex.x(), Interval(scaled_min_rotated_reflected_vertex.y().min())),
        Vector<Interval>(scaled_min_rotated_reflected_vertex.x(), Interval(scaled_min_rotated_reflected_vertex.y().max())),
        Vector<Interval>(scaled_mid_rotated_reflected_vertex.x(), Interval(scaled_mid_rotated_reflected_vertex.y().min())),
        Vector<Interval>(scaled_mid_rotated_reflected_vertex.x(), Interval(scaled_mid_rotated_reflected_vertex.y().max())),
        Vector<Interval>(scaled_max_rotated_reflected_vertex.x(), Interval(scaled_max_rotated_reflected_vertex.y().min())),
        Vector<Interval>(scaled_max_rotated_reflected_vertex.x(), Interval(scaled_max_rotated_reflected_vertex.y().max()))
    };
}

template<IntervalType Interval>
std::vector<Vector<Interval>> projection_hull_polygon(const Vertex<Interval>& vertex, const Interval& theta, const Interval& phi, const int resolution) {
    if(theta.len() > Interval::pi() / 3 * resolution) {
        return projection_hull_combined(vertex, theta, phi);
    }

    const Vector<Interval> vector = Vector<Interval>(vertex.x(), vertex.y());
    const std::vector<Vector<Interval>> rotation_hull = rotation_hull_polygon(vector, theta, resolution);

    std::vector<Vector<Interval>> projected_vertices;
    for(const Vector<Interval>& rotated_vertex: rotation_hull) {
        const Interval harmonic = harmonic_combined(rotated_vertex.y(), -vertex.z(), phi);
        projected_vertices.push_back(Vector<Interval>(rotated_vertex.x(), Interval(harmonic.min())));
        projected_vertices.push_back(Vector<Interval>(rotated_vertex.x(), Interval(harmonic.max())));
    }

    return projected_vertices;
}

template<IntervalType Interval>
std::vector<Vector<Interval>> projection_hull_advanced_approximate(const Vertex<Interval>& vertex, const Interval& theta, const Interval& phi, const int resolution = 8) {
    if(theta.len() > Interval::pi() / 3) {
        return projection_hull_combined(vertex, theta, phi);
    }
    std::vector<Vector<Interval>> projected_vertices;
    for(int i = 0; i <= resolution; i++) {
        const Interval theta_i = Interval(theta.min()) + Interval(theta.len()) * i / resolution;
        const Vector<Interval> projected_vertex = projection_combined(vertex, theta_i, Interval(phi.min()));
        projected_vertices.push_back(projected_vertex);
    }
    for(int i = 0; i <= resolution; i++) {
        const Interval phi_i = Interval(phi.min()) + Interval(phi.len()) * i / resolution;
        const Vector<Interval> projected_vertex = projection_combined(vertex, Interval(theta.max()), phi_i);
        projected_vertices.push_back(projected_vertex);
    }
    for(int i = 0; i <= resolution; i++) {
        const Interval theta_i = Interval(theta.max()) - Interval(theta.len()) * i / resolution;
        const Vector<Interval> projected_vertex = projection_combined(vertex, theta_i, Interval(phi.max()));
        projected_vertices.push_back(projected_vertex);
    }
    for(int i = 0; i <= resolution; i++) {
        const Interval phi_i = Interval(phi.max()) - Interval(phi.len()) * i / resolution;
        const Vector<Interval> projected_vertex = projection_combined(vertex, Interval(theta.min()), phi_i);
        projected_vertices.push_back(projected_vertex);
    }
    return projected_vertices;
}
