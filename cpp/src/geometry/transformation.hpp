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
// Y = y * cos(alpha) + x * sin(alpha)

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
        Vector<Interval>(Interval(vector.x().max()), Interval(vector.y().min())),
        Vector<Interval>(Interval(vector.x().max()), Interval(vector.y().max())),
        Vector<Interval>(Interval(vector.x().min()), Interval(vector.y().max())),
    };
}

template<IntervalType Interval>
std::vector<Vector<Interval>> rotation_hull_trivial(const Vector<Interval>& projected_vertex, const Interval& alpha) {
    return vector_hull(rotation_trivial(projected_vertex, alpha));
}

template<IntervalType Interval>
std::vector<Vector<Interval>> rotation_hull_combined(const Vector<Interval>& projected_vertex, const Interval& alpha) {
    return vector_hull(rotation_combined(projected_vertex, alpha));
}

template<IntervalType Interval>
std::vector<Vector<Interval>> rotation_hull_polygon(const Vector<Interval>& vector, const Interval& alpha, const int resolution) {
    if(resolution < 1) {
        throw std::invalid_argument("Resolution must be at least 1");
    }
    if(alpha.len() > Interval::pi() / 2 * resolution) {
        return rotation_hull_combined(vector, alpha);
    }
    std::vector<Vector<Interval>> rotated_vectors;
    rotated_vectors.emplace_back(rotation_trivial(vector, Interval(alpha.min())));
    const Interval scaling_factor = 1 / (Interval(alpha.rad()) / resolution).cos();
    const int pieces = 2 * resolution;
    for(int i = 1; i < pieces; i += 2) {
        const Interval alpha_i = Interval(alpha.min()) * (pieces - i) / pieces + Interval(alpha.max()) * i / pieces;
        const Vector<Interval> rotated_vertex = rotation_trivial(vector, alpha_i);
        const Vector<Interval> scaled_rotated_vertex = rotated_vertex * scaling_factor;
        rotated_vectors.emplace_back(scaled_rotated_vertex);
    }
    rotated_vectors.emplace_back(rotation_trivial(vector, Interval(alpha.max())));
    return rotated_vectors;
}

template<IntervalType Interval>
std::vector<Vector<Interval>> projection_hull_trivial(const Vertex<Interval>& vertex, const Interval& theta, const Interval& phi) {
    return vector_hull(projection_trivial(vertex, theta, phi));
}

template<IntervalType Interval>
std::vector<Vector<Interval>> projection_hull_combined(const Vertex<Interval>& vertex, const Interval& theta, const Interval& phi) {
    return vector_hull(projection_combined(vertex, theta, phi));
}

template<IntervalType Interval>
std::vector<Vector<Interval>> projection_hull_polygon(const Vertex<Interval>& vertex, const Interval& theta, const Interval& phi, const int resolution) {
    if(theta.len() > Interval::pi() / 2 * resolution) {
        return projection_hull_combined(vertex, theta, phi);
    }
    const Vector<Interval> vector = Vector<Interval>(vertex.x(), vertex.y());
    const std::vector<Vector<Interval>> rotation_hull = rotation_hull_polygon(vector, theta, resolution);
    std::vector<Vector<Interval>> projected_vertices;
    for(const Vector<Interval>& rotated_vertex: rotation_hull) {
        const Interval harmonic = harmonic_combined(rotated_vertex.y(), -vertex.z(), phi);
        projected_vertices.emplace_back(rotated_vertex.x(), Interval(harmonic.min()));
        projected_vertices.emplace_back(rotated_vertex.x(), Interval(harmonic.max()));
    }
    return projected_vertices;
}
