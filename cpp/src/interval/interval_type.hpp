#pragma once

#include <iostream>
#include <limits>
#include <sstream>

template<typename Integer>
concept IntegerType =
    std::integral<Integer> &&
    std::numeric_limits<Integer>::min() >= std::numeric_limits<int32_t>::min() &&
    std::numeric_limits<Integer>::max() <= std::numeric_limits<int32_t>::max();

template<typename Interval>
concept IntervalType =
    std::is_constructible_v<Interval, int> &&
    std::is_constructible_v<Interval, int, int> &&
    std::is_destructible_v<Interval> &&
    std::is_copy_constructible_v<Interval> &&
    std::is_move_constructible_v<Interval> &&

    !std::is_default_constructible_v<Interval> &&
    !std::is_copy_assignable_v<Interval> &&
    !std::is_move_assignable_v<Interval> &&

    requires(Interval interval, const Interval other_interval) {
        { interval.to_float() } -> std::same_as<double>;
        { interval.to_floats() } -> std::same_as<std::pair<double, double>>;

        { Interval::nan() } -> std::same_as<Interval>;
        { interval.is_nan() } -> std::same_as<bool>;

        { interval.is_positive() } -> std::same_as<bool>;
        { interval.is_negative() } -> std::same_as<bool>;
        { interval.is_nonzero() } -> std::same_as<bool>;

        { interval > other_interval } -> std::same_as<bool>;
        { interval < other_interval } -> std::same_as<bool>;

        { interval.min() } -> std::same_as<Interval>; // lower bound
        { interval.max() } -> std::same_as<Interval>; // upper bound
        { interval.mid() } -> std::same_as<Interval>; // approximate
        { interval.len() } -> std::same_as<Interval>; // upper bound
        { interval.rad() } -> std::same_as<Interval>; // upper bound
        { interval.hull(other_interval) } -> std::same_as<Interval>;

        { +interval } -> std::same_as<Interval>;
        { -interval } -> std::same_as<Interval>;
        { interval + other_interval } -> std::same_as<Interval>;
        { interval - other_interval } -> std::same_as<Interval>;
        { interval * other_interval } -> std::same_as<Interval>;
        { interval / other_interval } -> std::same_as<Interval>;

        { interval.inv() } -> std::same_as<Interval>;
        { interval.sqr() } -> std::same_as<Interval>;
        { interval.sqrt() } -> std::same_as<Interval>;

        { Interval::pi() } -> std::same_as<Interval>;
        { interval.cos() } -> std::same_as<Interval>;
        { interval.sin() } -> std::same_as<Interval>;
        { interval.tan() } -> std::same_as<Interval>;
        { interval.acos() } -> std::same_as<Interval>;
        { interval.asin() } -> std::same_as<Interval>;
        { interval.atan() } -> std::same_as<Interval>;
    };
