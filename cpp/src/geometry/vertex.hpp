#pragma once

#include "interval/interval.hpp"

template<IntervalType Interval>
struct Vertex {
private:
    Interval x_;
    Interval y_;
    Interval z_;

public:
    explicit Vertex(const Interval& x, const Interval& y, const Interval& z) : x_(x), y_(y), z_(z) {}

    ~Vertex() = default;

    Vertex(const Vertex& vertex) = default;

    Vertex(Vertex&& vertex) = default;

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

    Interval dist(const Vertex& vertex) const {
        return (operator-(vertex)).len();
    }

    bool diff(const Vertex& vertex) const {
        return dist(vertex).is_positive();
    }

    Vertex hull(const Vertex& vertex) const {
        return Vertex(x_.hull(vertex.x_), y_.hull(vertex.y_), z_.hull(vertex.z_));
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

    Vertex unit() const {
        const Interval norm = len();
        return Vertex(x_ / norm, y_ / norm, z_ / norm);
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Vertex& vertex) {
        return ostream << "(" << vertex.x_ << " | " << vertex.y_ << " | " << vertex.z_ << ")";
    }
};
