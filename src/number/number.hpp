#pragma once

#include "number/number_type.hpp"

#include "number/float_number.hpp"
static_assert(NumberType<FloatNumber>);

#include "number/mpfr_number.hpp"
static_assert(NumberType<MpfrNumber>);
