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

    requires(const Vector vector, const Vector other_vector, const Interval interval) {
        { +vector } -> std::same_as<Vector>;
        { -vector } -> std::same_as<Vector>;

        { vector + other_vector } -> std::same_as<Vector>;
        { vector - other_vector } -> std::same_as<Vector>;
        { vector * interval } -> std::same_as<Vector>;
        { vector / interval } -> std::same_as<Vector>;

        { vector.len() } -> std::same_as<Interval>;
    } &&

    requires(const Vector a, std::ostream& os) {
        { os << a } -> std::same_as<std::ostream&>;
    };

template<typename Vector2, typename Interval>
concept Vector2Type =
    VectorType<Vector2, Interval> &&

    std::is_constructible_v<Vector2, Interval, Interval> &&

    requires(Vector2 vector2) {
        { vector2.x() } -> std::convertible_to<Interval&>;
        { vector2.y() } -> std::convertible_to<Interval&>;
    } &&

    requires(const Vector2 vector2) {
        { vector2.x() } -> std::convertible_to<const Interval&>;
        { vector2.y() } -> std::convertible_to<const Interval&>;
    };

template<typename Vertex, typename Interval>
concept VertexType =
    VectorType<Vertex, Interval> &&

    std::is_constructible_v<Vertex, Interval, Interval, Interval> &&

    requires(Vertex vertex) {
        { vertex.x() } -> std::convertible_to<Interval&>;
        { vertex.y() } -> std::convertible_to<Interval&>;
        { vertex.z() } -> std::convertible_to<Interval&>;
    } &&

    requires(const Vertex vertex) {
        { vertex.x() } -> std::convertible_to<const Interval&>;
        { vertex.y() } -> std::convertible_to<const Interval&>;
        { vertex.z() } -> std::convertible_to<const Interval&>;
    };
