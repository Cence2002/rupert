#pragma once

#include <iostream>

template<typename Number>
concept NumberType =
    requires {
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
        { number.value() } -> std::same_as<typename Number::Value const&>;
    } &&

    requires(const int print_precision) {
        { Number::set_print_precision(print_precision) } -> std::same_as<void>;
    } &&

    requires(const Number number, std::ostream& os) {
        { os << number } -> std::same_as<std::ostream&>;
    };
