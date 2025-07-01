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

inline bool float_approx(
    const double value,
    const double target_value =
        std::numeric_limits<double>::quiet_NaN()
) {
    if(std::isnan(value) || std::isnan(target_value)) {
        return std::isnan(value) && std::isnan(target_value);
    }
    return Catch::Approx(value).margin(DEFAULT_MARGIN) == target_value;
}

inline bool interval_approx(
    const std::pair<double, double>& interval,
    const std::pair<double, double>& target_interval =
        {std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()}
) {
    if(std::isnan(interval.first) || std::isnan(interval.second) ||
       std::isnan(target_interval.first) || std::isnan(target_interval.second)) {
        return std::isnan(interval.first) && std::isnan(target_interval.first) &&
               std::isnan(interval.second) && std::isnan(target_interval.second);
    }
    return Catch::Approx(interval.first).margin(DEFAULT_MARGIN) == target_interval.first &&
           Catch::Approx(interval.second).margin(DEFAULT_MARGIN) == target_interval.second;
}
