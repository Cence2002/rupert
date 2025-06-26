#pragma once

#include "interval/interval.hpp"

template<IntervalType Interval>
struct Vector {
private:
    Interval x_;
    Interval y_;

public:
    explicit Vector(const Interval& x, const Interval& y) : x_(x), y_(y) {}

    ~Vector() = default;

    Vector(const Vector& vector) = default;

    Vector(Vector&& vector) = default;

    const Interval& x() const {
        return x_;
    }

    const Interval& y() const {
        return y_;
    }

    Vector operator+() const {
        return Vector(+x_, +y_);
    }

    Vector operator-() const {
        return Vector(-x_, -y_);
    }

    Vector operator+(const Vector& vector) const {
        return Vector(x_ + vector.x_, y_ + vector.y_);
    }

    Vector operator-(const Vector& vector) const {
        return Vector(x_ - vector.x_, y_ - vector.y_);
    }

    Vector operator*(const Interval& interval) const {
        return Vector(x_ * interval, y_ * interval);
    }

    Vector operator/(const Interval& interval) const {
        return Vector(x_ / interval, y_ / interval);
    }

    Interval len() const {
        return (x_.sqr() + y_.sqr()).sqrt();
    }

    Interval dist(const Vector& vector) const {
        return (*this - vector).len();
    }

    bool diff(const Vector& vector) const {
        return dist(vector).is_positive();
    }

    Vector hull(const Vector& vector) const {
        return Vector(x_.hull(vector.x_), y_.hull(vector.y_));
    }

    Interval dot(const Vector& vector) {
        return x_ * vector.x_ + y_ * vector.y_;
    }

    Interval cross(const Vector& vector) {
        return x_ * vector.y_ - y_ * vector.x_;
    }

    Vector unit() const {
        const Interval norm = len();
        return Vector(x_ / norm, y_ / norm);
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Vector& vector) {
        return ostream << "(" << vector.x_ << " | " << vector.y_ << ")";
    }
};
