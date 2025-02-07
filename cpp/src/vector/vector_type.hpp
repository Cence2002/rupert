#pragma once

#include "interval/interval.hpp"
#include <iostream>

template<typename Vector, typename Interval>
concept VectorType =
    IntervalType<Interval> &&

    std::is_default_constructible_v<Vector> &&
    std::is_destructible_v<Vector> &&
    std::is_copy_constructible_v<Vector> &&
    std::is_copy_assignable_v<Vector> &&
    std::is_move_constructible_v<Vector> &&
    std::is_move_assignable_v<Vector> &&

    requires(const Vector vector, const Vector other_vector) {
        { +vector } -> std::same_as<Vector>;
        { -vector } -> std::same_as<Vector>;

        { vector + other_vector } -> std::same_as<Vector>;
        { vector - other_vector } -> std::same_as<Vector>;

        { vector.len() } -> std::same_as<Interval>;
    } &&

    requires(const Vector a, std::ostream& os) {
        { os << a } -> std::same_as<std::ostream&>;
    };

template<typename Vector2, typename Interval>
concept Vector2Type =
    VectorType<Vector2, Interval> &&

    std::is_constructible_v<Vector2, Interval, Interval> &&
    std::is_constructible_v<Vector2, Interval, int> &&
    std::is_constructible_v<Vector2, int, Interval> &&
    std::is_constructible_v<Vector2, int, int> &&

    requires(Vector2 vector2) {
        { vector2.x() } -> std::convertible_to<Interval&>;
        { vector2.y() } -> std::convertible_to<Interval&>;
    } &&

    requires(const Vector2 vector2) {
        { vector2.x() } -> std::convertible_to<const Interval&>;
        { vector2.y() } -> std::convertible_to<const Interval&>;
    };

template<typename Vector3, typename Interval>
concept Vector3Type =
    VectorType<Vector3, Interval> &&

    std::is_constructible_v<Vector3, Interval, Interval, Interval> &&
    std::is_constructible_v<Vector3, Interval, Interval, int> &&
    std::is_constructible_v<Vector3, Interval, int, Interval> &&
    std::is_constructible_v<Vector3, Interval, int, int> &&
    std::is_constructible_v<Vector3, int, Interval, Interval> &&
    std::is_constructible_v<Vector3, int, Interval, int> &&
    std::is_constructible_v<Vector3, int, int, Interval> &&
    std::is_constructible_v<Vector3, int, int, int> &&

    requires(Vector3 vector3) {
        { vector3.x() } -> std::convertible_to<Interval&>;
        { vector3.y() } -> std::convertible_to<Interval&>;
        { vector3.z() } -> std::convertible_to<Interval&>;
    } &&

    requires(const Vector3 vector3) {
        { vector3.x() } -> std::convertible_to<const Interval&>;
        { vector3.y() } -> std::convertible_to<const Interval&>;
        { vector3.z() } -> std::convertible_to<const Interval&>;
    };
