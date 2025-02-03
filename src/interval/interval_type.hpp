#pragma once

#include "number/number.hpp"
#include <iostream>

template<typename Integer>
concept IntegerType = std::is_integral_v<Integer>;

enum class IntervalPrintMode {
    MIN_AND_MAX,
    MID_AND_RAD,
};

template<typename Interval>
concept IntervalType =
    requires {
        typename Interval::Number;
    } &&

    std::is_default_constructible_v<Interval> &&
    std::is_constructible_v<Interval, int> &&
    std::is_constructible_v<Interval, int, int> &&
    std::is_constructible_v<Interval, typename Interval::Number> &&
    std::is_constructible_v<Interval, typename Interval::Number, typename Interval::Number> &&

    std::is_destructible_v<Interval> &&
    std::is_copy_constructible_v<Interval> &&
    std::is_copy_assignable_v<Interval> &&
    std::is_move_constructible_v<Interval> &&
    std::is_move_assignable_v<Interval> &&

    requires(const Interval interval, const Interval other_interval, const typename Interval::Number number, const int integer) {
        { interval.min() } -> std::same_as<typename Interval::Number>;
        { interval.max() } -> std::same_as<typename Interval::Number>;
        { interval.mid() } -> std::same_as<typename Interval::Number>;
        { interval.len() } -> std::same_as<typename Interval::Number>;
        { interval.rad() } -> std::same_as<typename Interval::Number>;

        { interval.pos() } -> std::same_as<bool>;
        { interval.neg() } -> std::same_as<bool>;

        { interval > other_interval } -> std::same_as<bool>;
        { interval > integer } -> std::same_as<bool>;
        { integer > interval } -> std::same_as<bool>;
        { interval > number } -> std::same_as<bool>;
        { number > interval } -> std::same_as<bool>;

        { interval < other_interval } -> std::same_as<bool>;
        { interval < integer } -> std::same_as<bool>;
        { integer < interval } -> std::same_as<bool>;
        { interval < number } -> std::same_as<bool>;
        { number < interval } -> std::same_as<bool>;

        { +interval } -> std::same_as<Interval>;
        { -interval } -> std::same_as<Interval>;

        { interval + other_interval } -> std::same_as<Interval>;
        { interval + integer } -> std::same_as<Interval>;
        { integer + interval } -> std::same_as<Interval>;
        { interval + number } -> std::same_as<Interval>;
        { number + interval } -> std::same_as<Interval>;

        { interval - other_interval } -> std::same_as<Interval>;
        { interval - integer } -> std::same_as<Interval>;
        { integer - interval } -> std::same_as<Interval>;
        { interval - number } -> std::same_as<Interval>;
        { number - interval } -> std::same_as<Interval>;

        { interval * other_interval } -> std::same_as<Interval>;
        { interval * integer } -> std::same_as<Interval>;
        { integer * interval } -> std::same_as<Interval>;
        { interval * number } -> std::same_as<Interval>;
        { number * interval } -> std::same_as<Interval>;

        { interval / other_interval } -> std::same_as<Interval>;
        { interval / integer } -> std::same_as<Interval>;
        { integer / interval } -> std::same_as<Interval>;
        { interval / number } -> std::same_as<Interval>;
        { number / interval } -> std::same_as<Interval>;

        { interval.inv() } -> std::same_as<Interval>;
        { interval.sqr() } -> std::same_as<Interval>;
        { interval.sqrt() } -> std::same_as<Interval>;

        { interval.cos() } -> std::same_as<Interval>;
        { interval.sin() } -> std::same_as<Interval>;

        { Interval::unit() } -> std::same_as<Interval>;
        { Interval::pi() } -> std::same_as<Interval>;
    } &&

    requires(const int print_precision) {
        { Interval::set_print_precision(print_precision) };
    } &&

    requires(const IntervalPrintMode print_mode) {
        { Interval::set_print_mode(print_mode) };
    } &&

    requires(const Interval interval, std::ostream& os) {
        { os << interval } -> std::same_as<std::ostream&>;
    };
