#pragma once

#include "interval/intervals.hpp"

template<IntervalType Interval>
class Vector2 {
private:
    Interval x_;
    Interval y_;

public:
    explicit Vector2(const Interval& x, const Interval& y) : x_(x), y_(y) {}

    ~Vector2() = default;

    Vector2(const Vector2& vector) = default;

    Vector2(Vector2&& vector) = default;

    Vector2& operator=(const Vector2&) = delete;

    Vector2& operator=(Vector2&&) = delete;

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

    Vector2 operator+(const Vector2& vector) const {
        return Vector2(x_ + vector.x_, y_ + vector.y_);
    }

    Vector2 operator-(const Vector2& vector) const {
        return Vector2(x_ - vector.x_, y_ - vector.y_);
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

    Interval dist(const Vector2& vector) const {
        return operator-(vector).len();
    }

    bool diff(const Vector2& vector) const {
        return dist(vector).pos();
    }

    Vector2 hull(const Vector2& vector) const {
        return Vector2(x_.hull(vector.x_), y_.hull(vector.y_));
    }

    Interval dot(const Vector2& vector) {
        return x_ * vector.x_ + y_ * vector.y_;
    }

    Interval cross(const Vector2& vector) {
        return x_ * vector.y_ - y_ * vector.x_;
    }

    Vector2 unit() const {
        const Interval norm = len();
        return Vector2(x_ / norm, y_ / norm);
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Vector2& vector) {
        return ostream << "(" << vector.x_ << "," << vector.y_ << ")";
    }
};
