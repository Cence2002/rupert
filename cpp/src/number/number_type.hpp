#pragma once

#include <iostream>

template<typename Integer>
concept IntegerType = std::is_integral_v<Integer>;

template<typename Number>
concept NumberType =
    requires {
        { Number::name } -> std::convertible_to<std::string>;

        typename Number::Value;
    } &&

    std::is_default_constructible_v<Number> &&
    std::is_constructible_v<Number, int> &&

    std::is_destructible_v<Number> &&
    std::is_copy_constructible_v<Number> &&
    std::is_copy_assignable_v<Number> &&
    std::is_move_constructible_v<Number> &&
    std::is_move_assignable_v<Number> &&

    requires(Number number) {
        { number.value() } -> std::same_as<typename Number::Value&>;
    } &&

    requires(const Number number) {
        { number.value() } -> std::same_as<const typename Number::Value&>;
    } &&

    requires(const Number number, const Number other_number, const int integer) {
        { number.float_value() } -> std::same_as<double>;

        { number.is_positive() } -> std::same_as<bool>;
        { number.is_negative() } -> std::same_as<bool>;
        { number.is_nonzero() } -> std::same_as<bool>;
        { number.is_nan() } -> std::same_as<bool>;

        { Number::nan() } -> std::same_as<Number>;
    } &&

    requires(const size_t print_precision) {
        { Number::set_print_precision(print_precision) } -> std::same_as<void>;
    } &&

    requires(std::ostream& ostream, const Number number) {
        { ostream << number } -> std::same_as<std::ostream&>;
    };
