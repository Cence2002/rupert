#pragma once

#include "geometry/vector2.hpp"
#include "geometry/vector3.hpp"
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
        return Interval(sign) * amplitude * (angle - phase).cos();
    }
    if(sin_amplitude.is_nonzero()) {
        const Interval amplitude = (cos_amplitude.sqr() + sin_amplitude.sqr()).sqrt();
        const Interval phase = -(cos_amplitude / sin_amplitude).atan();
        const int sign = sin_amplitude.is_positive() ? 1 : -1;
        return Interval(sign) * amplitude * (angle - phase).sin();
    }
    return harmonic_trivial(cos_amplitude, sin_amplitude, angle);
}

// (X, Y) = R(alpha) * (x, y)
// X = x * cos(alpha) - y * sin(alpha)
// Y = y * cos(alpha) + x * sin(alpha)

template<IntervalType Interval>
Vector2<Interval> rotation_trivial(const Vector2<Interval>& vector, const Interval& alpha) {
    return Vector2<Interval>(
        harmonic_trivial(vector.x(), -vector.y(), alpha),
        harmonic_trivial(vector.y(), vector.x(), alpha)
    );
}

template<IntervalType Interval>
Vector2<Interval> rotation_combined(const Vector2<Interval>& vector, const Interval& alpha) {
    return Vector2<Interval>(
        harmonic_combined(vector.x(), -vector.y(), alpha),
        harmonic_combined(vector.y(), vector.x(), alpha)
    );
}

// (X, Y, _) = Rx(phi) * Rz(theta) * (x, y, z)
// X = x * cos(theta) - y * sin(theta)
// Y = (y * cos(theta) + x * sin(theta)) * cos(phi) - z * sin(phi)

template<IntervalType Interval>
Vector2<Interval> projection_trivial(const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    return Vector2<Interval>(
        harmonic_trivial(vertex.x(), -vertex.y(), theta),
        harmonic_trivial(harmonic_trivial(vertex.y(), vertex.x(), theta), -vertex.z(), phi)
    );
}

template<IntervalType Interval>
Vector2<Interval> projection_combined(const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    return Vector2<Interval>(
        harmonic_combined(vertex.x(), -vertex.y(), theta),
        harmonic_combined(harmonic_combined(vertex.y(), vertex.x(), theta), -vertex.z(), phi)
    );
}

template<IntervalType Interval>
std::vector<Vector2<Interval>> vector_hull(const Vector2<Interval>& vector2) {
    return std::vector<Vector2<Interval>>{
        Vector2<Interval>(Interval(vector2.x().min()), Interval(vector2.y().min())),
        Vector2<Interval>(Interval(vector2.x().max()), Interval(vector2.y().min())),
        Vector2<Interval>(Interval(vector2.x().max()), Interval(vector2.y().max())),
        Vector2<Interval>(Interval(vector2.x().min()), Interval(vector2.y().max())),
    };
}

template<IntervalType Interval>
std::vector<Vector2<Interval>> rotation_hull_trivial(const Vector2<Interval>& projected_vertex, const Interval& alpha) {
    return vector_hull(rotation_trivial(projected_vertex, alpha));
}

template<IntervalType Interval>
std::vector<Vector2<Interval>> rotation_hull_combined(const Vector2<Interval>& projected_vertex, const Interval& alpha) {
    return vector_hull(rotation_combined(projected_vertex, alpha));
}

template<IntervalType Interval>
std::vector<Vector2<Interval>> rotation_hull_polygon(const Vector2<Interval>& vector2, const Interval& alpha, const int resolution) {
    if(resolution < 1) {
        throw std::invalid_argument("Resolution must be at least 1");
    }
    if(Interval(alpha.len()) > Interval::pi() / Interval(2) * Interval(resolution)) {
        return rotation_hull_combined(vector2, alpha);
    }
    std::vector<Vector2<Interval>> rotated_vectors;
    rotated_vectors.emplace_back(rotation_trivial(vector2, Interval(alpha.min())));
    const Interval scaling_factor = (Interval(alpha.rad()) / Interval(resolution)).cos().inv();
    const int pieces = 2 * resolution;
    for(int i = 1; i < pieces; i += 2) {
        const Interval alpha_i = Interval(alpha.min()) * Interval(pieces - i) / Interval(pieces) + Interval(alpha.max()) * Interval(i) / Interval(pieces);
        const Vector2<Interval> rotated_vertex = rotation_trivial(vector2, alpha_i);
        const Vector2<Interval> scaled_rotated_vertex = rotated_vertex * scaling_factor;
        rotated_vectors.emplace_back(scaled_rotated_vertex);
    }
    rotated_vectors.emplace_back(rotation_trivial(vector2, Interval(alpha.max())));
    return rotated_vectors;
}

template<IntervalType Interval>
std::vector<Vector2<Interval>> projection_hull_trivial(const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    return vector_hull(projection_trivial(vertex, theta, phi));
}

template<IntervalType Interval>
std::vector<Vector2<Interval>> projection_hull_combined(const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    return vector_hull(projection_combined(vertex, theta, phi));
}

template<IntervalType Interval>
std::vector<Vector2<Interval>> projection_hull_polygon(const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi, const int resolution) {
    if(Interval(theta.len()) > Interval::pi() / Interval(2) * Interval(resolution)) {
        return projection_hull_combined(vertex, theta, phi);
    }
    const Vector2<Interval> vector = Vector2<Interval>(vertex.x(), vertex.y());
    const std::vector<Vector2<Interval>> rotation_hull = rotation_hull_polygon(vector, theta, resolution);
    std::vector<Vector2<Interval>> projected_vertices;
    for(const Vector2<Interval>& rotated_vertex: rotation_hull) {
        const Interval harmonic = harmonic_combined(rotated_vertex.y(), -vertex.z(), phi);
        projected_vertices.emplace_back(rotated_vertex.x(), Interval(harmonic.min()));
        projected_vertices.emplace_back(rotated_vertex.x(), Interval(harmonic.max()));
    }
    return projected_vertices;
}
