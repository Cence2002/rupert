#pragma once

#include "interval/float_interval.hpp"
#include "interval/boost_interval.hpp"
#include "interval/mpfi_interval.hpp"

static_assert(IntervalType<FloatInterval>);
static_assert(IntervalType<BoostInterval>);
static_assert(IntervalType<MpfiInterval>);
