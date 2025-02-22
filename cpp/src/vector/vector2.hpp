#pragma once

#include "interval/interval.hpp"

template<IntervalType Interval>
struct IntervalVector2 {
private:
    Interval x_;
    Interval y_;

public:
    explicit IntervalVector2() : x_(), y_() {}

    ~IntervalVector2() = default;

    IntervalVector2(const IntervalVector2& intervalvector2) = default;

    IntervalVector2& operator=(const IntervalVector2& intervalvector2) = default;

    IntervalVector2(IntervalVector2&& intervalvector2) = default;

    IntervalVector2& operator=(IntervalVector2&& intervalvector2) = default;

    explicit IntervalVector2(const Interval& x, const Interval& y) : x_(x), y_(y) {}

    Interval& x() {
        return x_;
    }

    Interval& y() {
        return y_;
    }

    const Interval& x() const {
        return x_;
    }

    const Interval& y() const {
        return y_;
    }

    IntervalVector2 operator+() const {
        return IntervalVector2(+x_, +y_);
    }

    IntervalVector2 operator-() const {
        return IntervalVector2(-x_, -y_);
    }

    IntervalVector2 operator+(const IntervalVector2& intervalvector2) const {
        return IntervalVector2(x_ + intervalvector2.x_, y_ + intervalvector2.y_);
    }

    IntervalVector2 operator-(const IntervalVector2& intervalvector2) const {
        return IntervalVector2(x_ - intervalvector2.x_, y_ - intervalvector2.y_);
    }

    Interval len() const {
        return (x_.sqr() + y_.sqr()).sqrt();
    }

    static Interval dot(const IntervalVector2& intervalvector2, const IntervalVector2& other_intervalvector2) {
        return intervalvector2.x() * other_intervalvector2.x() + intervalvector2.y() * other_intervalvector2.y();
    }

    static Interval cross(const IntervalVector2& intervalvector2, const IntervalVector2& other_intervalvector2) {
        return intervalvector2.x() * other_intervalvector2.y() - intervalvector2.y() * other_intervalvector2.x();
    }

    friend std::ostream& operator<<(std::ostream& ostream, const IntervalVector2& intervalvector2) {
        return ostream << "(" << intervalvector2.x_ << " | " << intervalvector2.y_ << ")";
    }
};

template<IntervalType Interval>
struct NumberVector2 {
private:
    using Number = typename Interval::Number;

    Number x_;
    Number y_;

public:
    explicit NumberVector2() : x_(), y_() {}

    ~NumberVector2() = default;

    NumberVector2(const NumberVector2& numbervector2) = default;

    NumberVector2& operator=(const NumberVector2& numbervector2) = default;

    NumberVector2(NumberVector2&& numbervector2) = default;

    NumberVector2& operator=(NumberVector2&& numbervector2) = default;

    explicit NumberVector2(const Number& x, const Number& y) : x_(x), y_(y) {}

    explicit NumberVector2(const IntervalVector2<Interval>& intervalvector2) : x_(intervalvector2.x()), y_(intervalvector2.y()) {}

    Number& x() {
        return x_;
    }

    Number& y() {
        return y_;
    }

    const Number& x() const {
        return x_;
    }

    const Number& y() const {
        return y_;
    }

    bool operator==(const NumberVector2& numbervector2) const {
        return x_ == numbervector2.x_ && y_ == numbervector2.y_;
    }

    bool operator!=(const NumberVector2& numbervector2) const {
        return x_ != numbervector2.x_ || y_ != numbervector2.y_;
    }

    friend std::ostream& operator<<(std::ostream& ostream, const NumberVector2& numbervector2) {
        return ostream << "(" << numbervector2.x_ << " | " << numbervector2.y_ << ")";
    }
};
