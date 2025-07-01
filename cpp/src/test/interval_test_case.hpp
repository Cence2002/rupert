#pragma once

#include <catch2/catch_template_test_macros.hpp>
#include "interval/intervals.hpp"

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
