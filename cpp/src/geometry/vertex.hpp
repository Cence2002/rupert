#pragma once

#include "interval/interval.hpp"

template<IntervalType Interval>
struct Vertex {
private:
    Interval x_;
    Interval y_;
    Interval z_;

public:
    explicit Vertex() : x_(), y_(), z_() {}

    ~Vertex() = default;

    Vertex(const Vertex& vertex) = default;

    Vertex& operator=(const Vertex& vertex) = default;

    Vertex(Vertex&& vertex) = default;

    Vertex& operator=(Vertex&& vertex) = default;

    explicit Vertex(const Interval& x, const Interval& y, const Interval& z) noexcept : x_(x), y_(y), z_(z) {}

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

    Vertex operator+() const {
        return Vertex(+x_, +y_, +z_);
    }

    Vertex operator-() const {
        return Vertex(-x_, -y_, -z_);
    }

    Vertex operator+(const Vertex& vertex) const {
        return Vertex(x_ + vertex.x_, y_ + vertex.y_, z_ + vertex.z_);
    }

    Vertex operator-(const Vertex& vertex) const {
        return Vertex(x_ - vertex.x_, y_ - vertex.y_, z_ - vertex.z_);
    }

    Vertex operator*(const Interval& interval) const {
        return Vertex(x_ * interval, y_ * interval, z_ * interval);
    }

    Vertex operator/(const Interval& interval) const {
        return Vertex(x_ / interval, y_ / interval, z_ / interval);
    }

    Interval len() const {
        return (x_.sqr() + y_.sqr() + z_.sqr()).sqrt();
    }

    Interval len_sqr() const {
        return x_.sqr() + y_.sqr() + z_.sqr();
    }

    Interval dot(const Vertex& vertex) const {
        return x_ * vertex.x_ + y_ * vertex.y_ + z_ * vertex.z_;
    }

    Vertex cross(const Vertex& vertex) const {
        return Vertex(
            y_ * vertex.z_ - z_ * vertex.y_,
            z_ * vertex.x_ - x_ * vertex.z_,
            x_ * vertex.y_ - y_ * vertex.x_
        );
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Vertex& vertex) {
        return ostream << "(" << vertex.x_ << " | " << vertex.y_ << " | " << vertex.z_ << ")";
    }
};
