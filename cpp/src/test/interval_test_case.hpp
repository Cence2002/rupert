#pragma once

#include "interval/intervals.hpp"
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#define INTERVAL_TYPES  \
    FloatInterval,      \
    BoostInterval,      \
    MpfiInterval

#define INTERVAL_TEST_CASE(name)  \
    TEMPLATE_TEST_CASE_SIG(       \
        name,                     \
        "",                       \
        ((typename Interval)),    \
        INTERVAL_TYPES            \
    )

#define DEFAULT_MARGIN 1e-9

inline bool check_float(const double value, const double target_value) {
    if(std::isnan(value) || std::isnan(target_value)) {
        return std::isnan(value) && std::isnan(target_value);
    }
    return Catch::Approx(value).margin(DEFAULT_MARGIN) == target_value;
}

inline bool check_float_is_nan(const double value) {
    return check_float(value, std::numeric_limits<double>::quiet_NaN());
}

template<typename Interval>
bool check_interval(const Interval interval, const double target_min, const double target_max) {
    const auto& [interval_min, interval_max] = interval.to_floats();
    if(std::isnan(interval_min) || std::isnan(interval_max) ||
       std::isnan(target_min) || std::isnan(target_max)) {
        return std::isnan(interval_min) && std::isnan(target_min) &&
               std::isnan(interval_max) && std::isnan(target_max);
    }
    return check_float(interval_min, target_min) &&
           check_float(interval_max, target_max);
}

template<typename Interval>
bool check_interval(const Interval interval, const double target_value) {
    return check_interval(interval, target_value, target_value);
}

template<typename Interval>
bool check_interval_is_nan(const Interval interval) {
    return check_interval(interval, std::numeric_limits<double>::quiet_NaN());
}
