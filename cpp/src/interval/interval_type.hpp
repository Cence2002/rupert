#pragma once

#include "number/number.hpp"

enum class IntervalPrintMode {
    min_and_max,
    mid_and_rad,
};

template<typename Interval>
concept IntervalType =

    NumberType<typename Interval::Number> &&

    std::is_default_constructible_v<Interval> &&
    std::is_constructible_v<Interval, typename Interval::Number> &&
    std::is_constructible_v<Interval, typename Interval::Number, typename Interval::Number> &&
    std::is_constructible_v<Interval, int> &&
    std::is_constructible_v<Interval, int, int> &&

    std::is_destructible_v<Interval> &&
    std::is_copy_constructible_v<Interval> &&
    std::is_copy_assignable_v<Interval> &&
    std::is_move_constructible_v<Interval> &&
    std::is_move_assignable_v<Interval> &&

    requires(Interval interval, const Interval other_interval, const typename Interval::Number number, const int integer) {
        { interval.is_nan() } -> std::same_as<bool>;
        { interval.is_positive() } -> std::same_as<bool>;
        { interval.is_negative() } -> std::same_as<bool>;
        { interval.is_nonzero() } -> std::same_as<bool>;

        { interval > other_interval } -> std::same_as<bool>;
        { interval > number } -> std::same_as<bool>;
        { number > interval } -> std::same_as<bool>;
        { interval > integer } -> std::same_as<bool>;
        { integer > interval } -> std::same_as<bool>;

        { interval < other_interval } -> std::same_as<bool>;
        { interval < number } -> std::same_as<bool>;
        { number < interval } -> std::same_as<bool>;
        { interval < integer } -> std::same_as<bool>;
        { integer < interval } -> std::same_as<bool>;

        { interval.min() } -> std::same_as<typename Interval::Number>; // lower bound
        { interval.max() } -> std::same_as<typename Interval::Number>; // upper bound
        { interval.mid() } -> std::same_as<typename Interval::Number>; // approximate
        { interval.len() } -> std::same_as<typename Interval::Number>; // upper bound
        { interval.rad() } -> std::same_as<typename Interval::Number>; // upper bound

        { +interval } -> std::same_as<Interval>;
        { -interval } -> std::same_as<Interval>;

        { interval + other_interval } -> std::same_as<Interval>;
        { interval + number } -> std::same_as<Interval>;
        { number + interval } -> std::same_as<Interval>;
        { interval + integer } -> std::same_as<Interval>;
        { integer + interval } -> std::same_as<Interval>;

        { interval - other_interval } -> std::same_as<Interval>;
        { interval - number } -> std::same_as<Interval>;
        { number - interval } -> std::same_as<Interval>;
        { interval - integer } -> std::same_as<Interval>;
        { integer - interval } -> std::same_as<Interval>;

        { interval * other_interval } -> std::same_as<Interval>;
        { interval * number } -> std::same_as<Interval>;
        { number * interval } -> std::same_as<Interval>;
        { interval * integer } -> std::same_as<Interval>;
        { integer * interval } -> std::same_as<Interval>;

        { interval / other_interval } -> std::same_as<Interval>;
        { interval / number } -> std::same_as<Interval>;
        { number / interval } -> std::same_as<Interval>;
        { interval / integer } -> std::same_as<Interval>;
        { integer / interval } -> std::same_as<Interval>;

        { interval += other_interval } -> std::same_as<Interval&>;
        { interval += number } -> std::same_as<Interval&>;
        { interval += integer } -> std::same_as<Interval&>;

        { interval -= other_interval } -> std::same_as<Interval&>;
        { interval -= number } -> std::same_as<Interval&>;
        { interval -= integer } -> std::same_as<Interval&>;

        { interval *= other_interval } -> std::same_as<Interval&>;
        { interval *= number } -> std::same_as<Interval&>;
        { interval *= integer } -> std::same_as<Interval&>;

        { interval /= other_interval } -> std::same_as<Interval&>;
        { interval /= number } -> std::same_as<Interval&>;
        { interval /= integer } -> std::same_as<Interval&>;

        { interval.inv() } -> std::same_as<Interval>;
        { interval.sqr() } -> std::same_as<Interval>;
        { interval.sqrt() } -> std::same_as<Interval>;

        { interval.cos() } -> std::same_as<Interval>;
        { interval.sin() } -> std::same_as<Interval>;
        { interval.tan() } -> std::same_as<Interval>;
        { interval.acos() } -> std::same_as<Interval>;
        { interval.asin() } -> std::same_as<Interval>;
        { interval.atan() } -> std::same_as<Interval>;

        { Interval::nan() } -> std::same_as<Interval>;
        { Interval::unit() } -> std::same_as<Interval>;
        { Interval::pi() } -> std::same_as<Interval>;
    } &&

    requires(std::ostream& ostream, const Interval interval, const size_t print_precision, const IntervalPrintMode print_mode) {
        { ostream << interval } -> std::same_as<std::ostream&>; // approximate
        { Interval::set_print_precision(print_precision) };
        { Interval::set_print_mode(print_mode) };
        { Interval::name } -> std::convertible_to<std::string>; // TODO: remove this (only used for tests, should be handled there)
    };
