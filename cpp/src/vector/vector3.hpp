#pragma once

#include "interval/interval.hpp"

template<IntervalType Interval>
struct IntervalVector3 {
private:
    Interval x_;
    Interval y_;
    Interval z_;

public:
    explicit IntervalVector3() = default;

    ~IntervalVector3() = default;

    IntervalVector3(const IntervalVector3& intervalvector3) = default;

    IntervalVector3& operator=(const IntervalVector3& intervalvector3) = default;

    IntervalVector3(IntervalVector3&& intervalvector3) = default;

    IntervalVector3& operator=(IntervalVector3&& intervalvector3) = default;

    explicit IntervalVector3(const Interval& x, const Interval& y, const Interval& z) : x_(x), y_(y), z_(z) {}

    Interval& x() {
        return x_;
    }

    Interval& y() {
        return y_;
    }

    Interval& z() {
        return z_;
    }

    const Interval& x() const {
        return x_;
    }

    const Interval& y() const {
        return y_;
    }

    const Interval& z() const {
        return z_;
    }

    IntervalVector3 operator+() const {
        return IntervalVector3(+x_, +y_, +z_);
    }

    IntervalVector3 operator-() const {
        return IntervalVector3(-x_, -y_, -z_);
    }

    IntervalVector3 operator+(const IntervalVector3& intervalvector3) const {
        return IntervalVector3(x_ + intervalvector3.x_, y_ + intervalvector3.y_, z_ + intervalvector3.z_);
    }

    IntervalVector3 operator-(const IntervalVector3& intervalvector3) const {
        return IntervalVector3(x_ - intervalvector3.x_, y_ - intervalvector3.y_, z_ - intervalvector3.z_);
    }

    Interval len() const {
        return (x_.sqr() + y_.sqr() + z_.sqr()).sqrt();
    }

    friend std::ostream& operator<<(std::ostream& ostream, const IntervalVector3& intervalvector3) {
        return ostream << "(" << intervalvector3.x_ << " | " << intervalvector3.y_ << " | " << intervalvector3.z_ << ")";
    }
};
