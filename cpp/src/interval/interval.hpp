#pragma once

#include "interval/float_interval.hpp"
#include "interval/boost_interval.hpp"
#include "interval/mpfi_interval.hpp"

template<IntervalType Interval>
std::ostream& operator<<(std::ostream& ostream, const Interval& interval) {
    switch(interval_print_mode) {
        case IntervalPrintMode::min_and_max: {
            return ostream << "[" << interval.min().value() << " : " << interval.max().value() << "]";
        }
        case IntervalPrintMode::mid_and_rad: {
            return ostream << "[" << interval.mid().value() << " ~ " << interval.rad().value() << "]";
        }
        default: throw std::invalid_argument("Unknown IntervalPrintMode");
    }
}

static_assert(IntervalType<FloatInterval>);
static_assert(IntervalType<BoostInterval>);
static_assert(IntervalType<MpfiInterval>);
