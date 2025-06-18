#pragma once

#include <iostream>
#include <mpfr.h>

inline size_t number_print_precision = 6;

template<typename Integer>
concept IntegerType =
    std::integral<Integer> &&
    std::numeric_limits<Integer>::min() >= std::numeric_limits<int32_t>::min() &&
    std::numeric_limits<Integer>::max() <= std::numeric_limits<int32_t>::max();

template<typename Float>
concept FloatType =
    std::same_as<Float, double> ||
    std::is_same_v<Float, mpfr_t>;

template<typename Number>
concept NumberType =

    FloatType<typename Number::Value> &&

    std::is_constructible_v<Number, int> &&
    std::is_destructible_v<Number> &&
    std::is_copy_constructible_v<Number> &&
    std::is_move_constructible_v<Number> &&

    !std::is_default_constructible_v<Number> &&
    !std::is_copy_assignable_v<Number> &&
    !std::is_move_assignable_v<Number> &&

    requires(Number number) {
        { number.value() } -> std::same_as<const typename Number::Value&>;
        { number.float_value() } -> std::same_as<double>; // approximate
    } &&

    requires(const Number number, const Number other_number) {
        { Number::nan() } -> std::same_as<Number>;
        { number.is_nan() } -> std::same_as<bool>;

        { number.is_positive() } -> std::same_as<bool>;
        { number.is_negative() } -> std::same_as<bool>;
        { number.is_nonzero() } -> std::same_as<bool>;

        { number > other_number } -> std::same_as<bool>;
        { number < other_number } -> std::same_as<bool>;
    } &&

    requires(std::ostream& ostream, const Number number, const size_t print_precision) {
        { ostream << number } -> std::same_as<std::ostream&>; // approximate
        { Number::name } -> std::convertible_to<std::string>; // TODO: remove this (only used for tests, should be handled there)
    };
