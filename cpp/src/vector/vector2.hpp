#pragma once

#include "interval/interval.hpp"

template<IntervalType Interval>
struct Vector2 {
private:
    Interval x_;
    Interval y_;

public:
    explicit Vector2() : x_(), y_() {}

    ~Vector2() = default;

    Vector2(const Vector2& vector2) = default;

    Vector2& operator=(const Vector2& vector2) = default;

    Vector2(Vector2&& vector2) = default;

    Vector2& operator=(Vector2&& vector2) = default;

    explicit Vector2(const Interval& x, const Interval& y) : x_(x), y_(y) {}

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

    Vector2 operator+() const {
        return Vector2(+x_, +y_);
    }

    Vector2 operator-() const {
        return Vector2(-x_, -y_);
    }

    Vector2 operator+(const Vector2& vector2) const {
        return Vector2(x_ + vector2.x_, y_ + vector2.y_);
    }

    Vector2 operator-(const Vector2& vector2) const {
        return Vector2(x_ - vector2.x_, y_ - vector2.y_);
    }

    Vector2 operator*(const Interval& interval) const {
        return Vector2(x_ * interval, y_ * interval);
    }

    Vector2 operator/(const Interval& interval) const {
        return Vector2(x_ / interval, y_ / interval);
    }

    Interval len() const {
        return (x_.sqr() + y_.sqr()).sqrt();
    }

    Interval len_sqr() const {
        return x_.sqr() + y_.sqr();
    }

    static Interval dot(const Vector2& vector2, const Vector2& other_vector2) {
        return vector2.x() * other_vector2.x() + vector2.y() * other_vector2.y();
    }

    static Interval cross(const Vector2& vector2, const Vector2& other_vector2) {
        return vector2.x() * other_vector2.y() - vector2.y() * other_vector2.x();
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Vector2& vector2) {
        return ostream << "(" << vector2.x_ << " | " << vector2.y_ << ")";
    }
};
