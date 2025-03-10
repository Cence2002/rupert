#pragma once

#include "vector/vector.hpp"
#include "vector/vertex.hpp"

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
std::vector<Vector<Interval>> vector_hull(const Vector<Interval>& vector) {
    return std::vector<Vector<Interval>>{
        Vector<Interval>(Interval(vector.x().min()), Interval(vector.y().min())),
        Vector<Interval>(Interval(vector.x().min()), Interval(vector.y().max())),
        Vector<Interval>(Interval(vector.x().max()), Interval(vector.y().max())),
        Vector<Interval>(Interval(vector.x().max()), Interval(vector.y().min()))
    };
}

// (x, y) rotated with angle alpha
// X = x * cos(alpha) - y * sin(alpha)
// Y = x * sin(alpha) + y * cos(alpha)

template<IntervalType Interval>
Vector<Interval> rotation_trivial(const Vector<Interval>& projected_vertex, const Interval& alpha) {
    return Vector<Interval>(
        harmonic_trivial(projected_vertex.x(), -projected_vertex.y(), alpha),
        harmonic_trivial(projected_vertex.y(), projected_vertex.x(), alpha)
    );
}

template<IntervalType Interval>
Vector<Interval> rotation_combined(const Vector<Interval>& projected_vertex, const Interval& alpha) {
    return Vector<Interval>(
        harmonic_combined(projected_vertex.x(), -projected_vertex.y(), alpha),
        harmonic_combined(projected_vertex.y(), projected_vertex.x(), alpha)
    );
}

// (x, y, z) projected angle theta and phi
// X = -x * sin(theta) + y * cos(theta)
// Y = (x * cos(theta) + y * sin(theta)) * cos(phi) - z * sin(phi)

template<IntervalType Interval>
Vector<Interval> projection_trivial(const Vertex<Interval>& vertex, const Interval& theta, const Interval& phi) {
    const Vector<Interval> reflected_vertex = Vector<Interval>(vertex.x(), -vertex.y());
    const Interval shifted_theta = theta + Interval::pi() / 2;
    const Vector<Interval> rotated_reflected_vertex = rotation_trivial(reflected_vertex, shifted_theta);
    return Vector<Interval>(
        rotated_reflected_vertex.x(),
        harmonic_trivial(rotated_reflected_vertex.y(), -vertex.z(), phi)
    );
}

template<IntervalType Interval>
Vector<Interval> projection_combined(const Vertex<Interval>& vertex, const Interval& theta, const Interval& phi) {
    const Vector<Interval> reflected_vertex = Vector<Interval>(vertex.x(), -vertex.y());
    const Interval shifted_theta = theta + Interval::pi() / 2;
    const Vector<Interval> rotated_reflected_vertex = rotation_combined(reflected_vertex, shifted_theta);
    return Vector<Interval>(
        rotated_reflected_vertex.x(),
        harmonic_combined(rotated_reflected_vertex.y(), -vertex.z(), phi)
    );
}
