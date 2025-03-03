#pragma once

#include "interval/interval.hpp"

template<IntervalType Interval>
struct Vector3 {
private:
    Interval x_;
    Interval y_;
    Interval z_;

public:
    explicit Vector3() = default;

    ~Vector3() = default;

    Vector3(const Vector3& vector3) = default;

    Vector3& operator=(const Vector3& vector3) = default;

    Vector3(Vector3&& vector3) = default;

    Vector3& operator=(Vector3&& vector3) = default;

    explicit Vector3(const Interval& x, const Interval& y, const Interval& z) noexcept: x_(x), y_(y), z_(z) {}

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

    Vector3 operator+() const {
        return Vector3(+x_, +y_, +z_);
    }

    Vector3 operator-() const {
        return Vector3(-x_, -y_, -z_);
    }

    Vector3 operator+(const Vector3& vector3) const {
        return Vector3(x_ + vector3.x_, y_ + vector3.y_, z_ + vector3.z_);
    }

    Vector3 operator-(const Vector3& vector3) const {
        return Vector3(x_ - vector3.x_, y_ - vector3.y_, z_ - vector3.z_);
    }

    Vector3 operator*(const Interval& interval) const {
        return Vector3(x_ * interval, y_ * interval, z_ * interval);
    }

    Vector3 operator/(const Interval& interval) const {
        return Vector3(x_ / interval, y_ / interval, z_ / interval);
    }

    Interval len() const {
        return (x_.sqr() + y_.sqr() + z_.sqr()).sqrt();
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Vector3& vector3) {
        return ostream << "(" << vector3.x_ << " | " << vector3.y_ << " | " << vector3.z_ << ")";
    }
};
