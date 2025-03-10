#pragma once

#include "vector.hpp"

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
std::vector<Vector2<Interval>> vector2_hull(const Vector2<Interval>& vector2) {
    return std::vector<Vector2<Interval>>{
        Vector2<Interval>(Interval(vector2.x().min()), Interval(vector2.y().min())),
        Vector2<Interval>(Interval(vector2.x().min()), Interval(vector2.y().max())),
        Vector2<Interval>(Interval(vector2.x().max()), Interval(vector2.y().max())),
        Vector2<Interval>(Interval(vector2.x().max()), Interval(vector2.y().min()))
    };
}

// (x, y) rotated with angle alpha
// X = x * cos(alpha) - y * sin(alpha)
// Y = x * sin(alpha) + y * cos(alpha)

template<IntervalType Interval>
Vector2<Interval> rotation_trivial(const Vector2<Interval>& projected_vertex, const Interval& alpha) {
    return Vector2<Interval>(
        harmonic_trivial(projected_vertex.x(), -projected_vertex.y(), alpha),
        harmonic_trivial(projected_vertex.y(), projected_vertex.x(), alpha)
    );
}

template<IntervalType Interval>
Vector2<Interval> rotation_combined(const Vector2<Interval>& projected_vertex, const Interval& alpha) {
    return Vector2<Interval>(
        harmonic_combined(projected_vertex.x(), -projected_vertex.y(), alpha),
        harmonic_combined(projected_vertex.y(), projected_vertex.x(), alpha)
    );
}

// (x, y, z) projected angle theta and phi
// X = -x * sin(theta) + y * cos(theta)
// Y = (x * cos(theta) + y * sin(theta)) * cos(phi) - z * sin(phi)

template<IntervalType Interval>
Vector2<Interval> projection_trivial(const Vertex<Interval>& vertex, const Interval& theta, const Interval& phi) {
    const Vector2<Interval> reflected_vertex = Vector2<Interval>(vertex.x(), -vertex.y());
    const Interval shifted_theta = theta + Interval::pi() / 2;
    const Vector2<Interval> rotated_reflected_vertex = rotation_trivial(reflected_vertex, shifted_theta);
    return Vector2<Interval>(
        rotated_reflected_vertex.x(),
        harmonic_trivial(rotated_reflected_vertex.y(), -vertex.z(), phi)
    );
}

template<IntervalType Interval>
Vector2<Interval> projection_combined(const Vertex<Interval>& vertex, const Interval& theta, const Interval& phi) {
    const Vector2<Interval> reflected_vertex = Vector2<Interval>(vertex.x(), -vertex.y());
    const Interval shifted_theta = theta + Interval::pi() / 2;
    const Vector2<Interval> rotated_reflected_vertex = rotation_combined(reflected_vertex, shifted_theta);
    return Vector2<Interval>(
        rotated_reflected_vertex.x(),
        harmonic_combined(rotated_reflected_vertex.y(), -vertex.z(), phi)
    );
}
