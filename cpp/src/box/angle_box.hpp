#pragma once

#include "box/box.hpp"
#include "interval/intervals.hpp"

namespace AngleBox {
    template<IntervalType Interval>
    Interval theta(const Box2& box) {
        return box.range(0).interval<Interval>();
    }

    template<IntervalType Interval>
    Interval phi(const Box2& box) {
        return box.range(1).interval<Interval>();
    }

    template<IntervalType Interval>
    Interval theta(const Box3& box) {
        return box.range(0).interval<Interval>();
    }

    template<IntervalType Interval>
    Interval phi(const Box3& box) {
        return box.range(1).interval<Interval>();
    }

    template<IntervalType Interval>
    Interval alpha(const Box3& box) {
        return box.range(2).interval<Interval>();
    }
}
