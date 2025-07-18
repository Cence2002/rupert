#pragma once

#include "interval/intervals.hpp"

template<IntervalType Interval>
class Vector3 {
    Interval x_;
    Interval y_;
    Interval z_;

public:
    explicit Vector3(const Interval& x, const Interval& y, const Interval& z) : x_(x), y_(y), z_(z) {}

    ~Vector3() = default;

    Vector3(const Vector3& vector) = default;

    Vector3(Vector3&& vector) = default;

    Vector3& operator=(const Vector3&) = delete;

    Vector3& operator=(Vector3&&) = delete;

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

    Vector3 operator+(const Vector3& vector) const {
        return Vector3(x_ + vector.x_, y_ + vector.y_, z_ + vector.z_);
    }

    Vector3 operator-(const Vector3& vector) const {
        return Vector3(x_ - vector.x_, y_ - vector.y_, z_ - vector.z_);
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

    Interval dist(const Vector3& vector) const {
        return operator-(vector).len();
    }

    bool diff(const Vector3& vector) const {
        return dist(vector).pos();
    }

    Vector3 hull(const Vector3& vector) const {
        return Vector3(x_.hull(vector.x_), y_.hull(vector.y_), z_.hull(vector.z_));
    }

    Interval dot(const Vector3& vertex) const {
        return x_ * vertex.x_ + y_ * vertex.y_ + z_ * vertex.z_;
    }

    Vector3 cross(const Vector3& vertex) const {
        return Vector3(
            y_ * vertex.z_ - z_ * vertex.y_,
            z_ * vertex.x_ - x_ * vertex.z_,
            x_ * vertex.y_ - y_ * vertex.x_
        );
    }

    Vector3 unit() const {
        const Interval norm = len();
        return Vector3(x_ / norm, y_ / norm, z_ / norm);
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Vector3& vertex) {
        return ostream << "(" << vertex.x_ << "," << vertex.y_ << "," << vertex.z_ << ")";
    }
};
