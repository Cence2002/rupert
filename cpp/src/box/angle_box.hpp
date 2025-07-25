#pragma once

#include "box/box.hpp"
#include "interval/intervals.hpp"

namespace Angle {
    template<IntervalType Interval>
    Interval angle(const Range& range) {
        return Interval::tau() * range.interval<Interval>();
    }

    template<IntervalType Interval>
    Interval angle_mid(const Range& range) {
        return Interval::tau() * range.interval_mid<Interval>();
    }

    inline Range theta_range(const Box2& box) {
        return box.range(0);
    }

    inline Range phi_range(const Box2& box) {
        return box.range(1);
    }

    inline Range theta_range(const Box3& box) {
        return box.range(0);
    }

    inline Range phi_range(const Box3& box) {
        return box.range(1);
    }

    inline Range alpha_range(const Box3& box) {
        return box.range(2);
    }

    template<IntervalType Interval>
    Interval theta(const Box2& box) {
        return angle<Interval>(theta_range(box));
    }

    template<IntervalType Interval>
    Interval phi(const Box2& box) {
        return angle<Interval>(phi_range(box));
    }

    template<IntervalType Interval>
    Interval theta(const Box3& box) {
        return angle<Interval>(theta_range(box));
    }

    template<IntervalType Interval>
    Interval phi(const Box3& box) {
        return angle<Interval>(phi_range(box));
    }

    template<IntervalType Interval>
    Interval alpha(const Box3& box) {
        return angle<Interval>(alpha_range(box));
    }

    template<IntervalType Interval>
    Interval theta_mid(const Box2& box) {
        return angle_mid<Interval>(theta_range(box));
    }

    template<IntervalType Interval>
    Interval phi_mid(const Box2& box) {
        return angle_mid<Interval>(phi_range(box));
    }

    template<IntervalType Interval>
    Interval theta_mid(const Box3& box) {
        return angle_mid<Interval>(theta_range(box));
    }

    template<IntervalType Interval>
    Interval phi_mid(const Box3& box) {
        return angle_mid<Interval>(phi_range(box));
    }

    template<IntervalType Interval>
    Interval alpha_mid(const Box3& box) {
        return angle_mid<Interval>(alpha_range(box));
    }

    template<IntervalType Interval>
    Interval angle_radius(const Box2& box) {
        const Interval horizontal_radius = theta<Interval>(box).rad();
        const Interval vertical_radius = phi<Interval>(box).rad();
        return (horizontal_radius.sqr() + vertical_radius.sqr()).sqrt();
    }

    template<IntervalType Interval>
    Interval angle_radius(const Box3& box) {
        const Interval horizontal_radius = theta<Interval>(box).rad() + Angle::alpha<Interval>(box).rad();
        const Interval vertical_radius = phi<Interval>(box).rad();
        return (horizontal_radius.sqr() + vertical_radius.sqr()).sqrt();
    }
}
