#pragma once

#include "interval/interval_type.hpp"
#include "interval/float_interval.hpp"
#include "interval/boost_interval.hpp"
#include "interval/mpfi_interval.hpp"

enum class PrintMode {
    min_and_max,
    mid_and_rad,
    mid_only,
};

inline PrintMode print_mode = PrintMode::mid_and_rad;
inline std::streamsize print_precision = 6;

inline std::string to_string(const double number) {
    std::ostringstream number_str;
    number_str.precision(print_precision);
    number_str << number;
    return number_str.str();
}

template<IntervalType Interval>
std::ostream& operator<<(std::ostream& ostream, const Interval& interval) {
    switch(print_mode) {
        case PrintMode::min_and_max: {
            return ostream << "[" << to_string(interval.min().to_float()) << " : " << to_string(interval.max().to_float()) << "]";
        }
        case PrintMode::mid_and_rad: {
            return ostream << "[" << to_string(interval.mid().to_float()) << " ~ " << to_string(interval.rad().to_float()) << "]";
        }
        case PrintMode::mid_only: {
            return ostream << to_string(interval.mid().to_float());
        }
        default: throw std::invalid_argument("Unknown IntervalPrintMode");
    }
}

static_assert(IntervalType<FloatInterval>);
static_assert(IntervalType<BoostInterval>);
static_assert(IntervalType<MpfiInterval>);
