#pragma once

#include "interval/interval.hpp"

template<IntervalType Interval>
struct Vector2 {
private:
    Interval x_;
    Interval y_;

public:
    explicit Vector2(const Interval& x, const Interval& y) noexcept : x_(x), y_(y) {}

    ~Vector2() = default;

    Vector2(const Vector2& vector2) = default;

    Vector2(Vector2&& vector2) = default;

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

    Interval dist(const Vector2& vector2) const {
        return (operator-(vector2)).len();
    }

    bool diff(const Vector2& vector2) const {
        return dist(vector2).is_positive();
    }

    Vector2 hull(const Vector2& vector2) const {
        return Vector2(x_.hull(vector2.x_), y_.hull(vector2.y_));
    }

    Interval dot(const Vector2& vector2) {
        return x_ * vector2.x_ + y_ * vector2.y_;
    }

    Interval cross(const Vector2& vector2) {
        return x_ * vector2.y_ - y_ * vector2.x_;
    }

    Vector2 unit() const {
        const Interval norm = len();
        return Vector2(x_ / norm, y_ / norm);
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Vector2& vector2) {
        return ostream << "(" << vector2.x_ << "," << vector2.y_ << ")";
    }
};
