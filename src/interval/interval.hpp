#pragma once

#include "interval/interval_type.hpp"

#include "interval/float_interval.hpp"
static_assert(IntervalType<FloatInterval>);

#include "interval/boost_interval.hpp"
static_assert(IntervalType<BoostInterval>);

#include "interval/mpfi_interval.hpp"
static_assert(IntervalType<MpfiInterval>);
