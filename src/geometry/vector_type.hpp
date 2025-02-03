#pragma once

#include "interval/interval.hpp"
#include <iostream>

template<typename V, typename I>
concept VectorType =
    IntervalType<I> &&

    std::is_default_constructible_v<V> &&
    std::is_destructible_v<V> &&
    std::is_copy_constructible_v<V> &&
    std::is_copy_assignable_v<V> &&
    std::is_move_constructible_v<V> &&
    std::is_move_assignable_v<V> &&

    requires(const V vector, const V v, const I i, const int n) {
        { +vector } -> std::same_as<V>;
        { -vector } -> std::same_as<V>;

        { vector + v } -> std::same_as<V>;
        { vector + i } -> std::same_as<V>;
        { vector + n } -> std::same_as<V>;
        { i + vector } -> std::same_as<V>;
        { n + vector } -> std::same_as<V>;

        { vector - v } -> std::same_as<V>;
        { vector - i } -> std::same_as<V>;
        { vector - n } -> std::same_as<V>;
        { i - vector } -> std::same_as<V>;
        { n - vector } -> std::same_as<V>;

        { vector * v } -> std::same_as<V>;
        { vector * i } -> std::same_as<V>;
        { vector * n } -> std::same_as<V>;
        { i * vector } -> std::same_as<V>;
        { n * vector } -> std::same_as<V>;

        { vector / v } -> std::same_as<V>;
        { vector / i } -> std::same_as<V>;
        { vector / n } -> std::same_as<V>;
        { i / vector } -> std::same_as<V>;
        { n / vector } -> std::same_as<V>;

        { vector.len() } -> std::same_as<I>;
    } &&

    requires(const V a, std::ostream& os) {
        { os << a } -> std::same_as<std::ostream&>;
    };

enum class Orientation {
    COUNTERCLOCKWISE,
    CLOCKWISE,
    COLLINEAR
};

inline std::ostream& operator<<(std::ostream& os, const Orientation& orientation) {
    switch(orientation) {
        case Orientation::COUNTERCLOCKWISE: return os << "COUNTERCLOCKWISE";
        case Orientation::CLOCKWISE: return os << "CLOCKWISE";
        case Orientation::COLLINEAR: return os << "COLLINEAR";
        default: throw std::invalid_argument("Invalid orientation");
    }
}
