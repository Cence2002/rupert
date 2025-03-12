#pragma once

#include "number/float_number.hpp"
#include "number/mpfr_number.hpp"

static_assert(NumberType<FloatNumber>);
static_assert(NumberType<MpfrNumber>);
