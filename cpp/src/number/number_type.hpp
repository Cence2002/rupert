#pragma once

#include <iostream>

template<typename Integer>
concept IntegerType =
    std::integral<Integer> &&
    std::numeric_limits<Integer>::min() >= std::numeric_limits<int32_t>::min() &&
    std::numeric_limits<Integer>::max() <= std::numeric_limits<int32_t>::max();

template<typename Number>
concept NumberType =

    std::is_default_constructible_v<Number> &&
    std::is_constructible_v<Number, int> &&

    std::is_destructible_v<Number> &&
    std::is_copy_constructible_v<Number> &&
    std::is_copy_assignable_v<Number> &&
    std::is_move_constructible_v<Number> &&
    std::is_move_assignable_v<Number> &&

    requires(Number number, const Number const_number) {
        typename Number::Value;
        { number.value() } -> std::same_as<typename Number::Value&>;
        { const_number.value() } -> std::same_as<const typename Number::Value&>;
    } &&

    requires(const Number number, const int integer) {
        { number.float_value() } -> std::same_as<double>;

        { number.is_nan() } -> std::same_as<bool>;
        { number.is_positive() } -> std::same_as<bool>;
        { number.is_negative() } -> std::same_as<bool>;
        { number.is_nonzero() } -> std::same_as<bool>;
    } &&

    requires(std::ostream& ostream, const Number number, const size_t print_precision) {
        { ostream << number } -> std::same_as<std::ostream&>;
        { Number::set_print_precision(print_precision) } -> std::same_as<void>;
        { Number::name } -> std::convertible_to<std::string>; // TODO: remove this (only used for tests, should be handled there)
    };
