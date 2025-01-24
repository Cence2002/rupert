#pragma once

#include "interval.hpp"
#include <iostream>
#include <vector>

template<typename V, typename I>
concept VectorType =
        requires(const V a, const V b, const I c, std::ostream &os) {
            { +a } -> std::same_as<V>;
            { -a } -> std::same_as<V>;
            { a.mag() } -> std::same_as<I>;

            { a + b } -> std::same_as<V>;
            { a - b } -> std::same_as<V>;
            { a * b } -> std::same_as<V>;
            { a / b } -> std::same_as<V>;

            { a + c } -> std::same_as<V>;
            { a - c } -> std::same_as<V>;
            { a * c } -> std::same_as<V>;
            { a / c } -> std::same_as<V>;

            { c + a } -> std::same_as<V>;
            { c - a } -> std::same_as<V>;
            { c * a } -> std::same_as<V>;
            { c / a } -> std::same_as<V>;

            { os << a } -> std::same_as<std::ostream&>;
        };

template<typename V, typename I>
concept Vector2Type =
        VectorType<V, I> &&
        requires(V a) {
            { a.x } -> std::convertible_to<I>;
            { a.y } -> std::convertible_to<I>;
        } &&
        std::is_constructible_v<V, I, I> &&
        std::is_constructible_v<V, double, double>;

template<typename V, typename I>
concept Vector3Type =
        VectorType<V, I> &&
        requires(const V a) {
            { a.x } -> std::convertible_to<I>;
            { a.x } -> std::convertible_to<I>;
            { a.x } -> std::convertible_to<I>;
        } &&
        std::is_constructible_v<V, double, double, double>;

template<IntervalType I>
class Line2;

template<IntervalType I>
class Vector2 {
public:
    I x, y;

    explicit Vector2(const I x, const I y) : x(x), y(y) {}

    explicit Vector2(const double x, const double y) : x(I(x)), y(I(y)) {}

    Vector2 operator+() const {
        return Vector2(+x, +y);
    }

    Vector2 operator-() const {
        return Vector2(-x, -y);
    }

    Vector2 operator+(const I s) const {
        return Vector2(x + s, y + s);
    }

    Vector2 operator+(const Vector2 &v) const {
        return Vector2(x + v.x, y + v.y);
    }

    Vector2 operator-(const I s) const {
        return Vector2(x - s, y - s);
    }

    Vector2 operator-(const Vector2 &v) const {
        return Vector2(x - v.x, y - v.y);
    }

    Vector2 operator*(const I s) const {
        return Vector2(x * s, y * s);
    }

    Vector2 operator*(const Vector2 &v) const {
        return Vector2(x * v.x, y * v.y);
    }

    Vector2 operator/(const I s) const {
        return Vector2(x / s, y / s);
    }

    Vector2 operator/(const Vector2 &v) const {
        return Vector2(x / v.x, y / v.y);
    }

    I mag() const {
        return (x * x + y * y).sqrt();
    }

    Vector2 rotate(const I angle) const {
        const I sin_angle = angle.sin();
        const I cos_angle = angle.cos();
        return Vector2(
            x * cos_angle - y * sin_angle,
            x * sin_angle + y * cos_angle
        );
    }

    static I cross(const Vector2 &a, const Vector2 &b) {
        return a.x * b.y - a.y * b.x;
    }

    static I orient(const Vector2 &a, const Vector2 &b, const Vector2 &c) {
        return cross(b - a, c - a);
    }

    bool inside(const std::vector<Vector2<I>> &vertices) const {
        return Line2<I>(Vector2<I>(0, 0), *this).avoids(vertices);
    }

    bool outside(const std::vector<Vector2<I>> &vertices) const {
        // origin->vector line might go through a vertex, making this function return false
        // despite the vector being outside the polygon
        // TODO: replace this with avoids(vector, vector*c) for sufficiently large c
        // it would also break for the convex hull consisting of a set of Lines instead of a sequence of points
        // as it could intersect a Line and still be inside the convex hull
        return Line2<I>(Vector2<I>(0, 0), *this).intersects(vertices);
    }

    bool inside_advanced(const std::vector<Vector2<I>> &vertices) const {
        return Vector2<I>(x.min(), y.min()).inside(vertices) &&
               Vector2<I>(x.min(), y.max()).inside(vertices) &&
               Vector2<I>(x.max(), y.min()).inside(vertices) &&
               Vector2<I>(x.max(), y.max()).inside(vertices);
    }

    bool outside_advanced(const std::vector<Vector2<I>> &vertices) const {
        return Vector2<I>(x.mid(), y.mid()).outside(vertices) &&
               Line2<I>(Vector2<I>(x.min(), y.min()), Vector2<I>(x.min(), y.max())).avoids(vertices) &&
               Line2<I>(Vector2<I>(x.max(), y.min()), Vector2<I>(x.max(), y.max())).avoids(vertices) &&
               Line2<I>(Vector2<I>(x.min(), y.min()), Vector2<I>(x.max(), y.min())).avoids(vertices) &&
               Line2<I>(Vector2<I>(x.min(), y.max()), Vector2<I>(x.max(), y.max())).avoids(vertices) &&
               !(x.has(0) && y.has(0));
    }

    bool intersects_advanced(const std::vector<Vector2<I>> &vertices) const {
        return Vector2<I>(x.mid(), y.mid()).inside(vertices) ||
               Line2<I>(Vector2<I>(x.min(), y.min()), Vector2<I>(x.min(), y.max())).intersects(vertices) ||
               Line2<I>(Vector2<I>(x.max(), y.min()), Vector2<I>(x.max(), y.max())).intersects(vertices) ||
               Line2<I>(Vector2<I>(x.min(), y.min()), Vector2<I>(x.max(), y.min())).intersects(vertices) ||
               Line2<I>(Vector2<I>(x.min(), y.max()), Vector2<I>(x.max(), y.max())).intersects(vertices) ||
               (x.has(0) && y.has(0));
    }

    friend std::ostream &operator<<(std::ostream &os, const Vector2 &v) {
        return os << "(" << v.x << ", " << v.y << ")";
    }
};

template<IntervalType I>
class Vector3 {
public:
    I x, y, z;

    explicit Vector3(const I x, const I y, const I z) : x(x), y(y), z(z) {}

    explicit Vector3(const double x, const double y, const double z) : x(I(x)), y(I(y)), z(I(z)) {}

    Vector3 operator+() const {
        return Vector3(+x, +y, +z);
    }

    Vector3 operator-() const {
        return Vector3(-x, -y, -z);
    }

    Vector3 operator+(const I s) const {
        return Vector3(x + s, y + s, z + s);
    }

    Vector3 operator+(const Vector3 &v) const {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }

    Vector3 operator-(const I s) const {
        return Vector3(x - s, y - s, z - s);
    }

    Vector3 operator-(const Vector3 &v) const {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }

    Vector3 operator*(const I s) const {
        return Vector3(x * s, y * s, z * s);
    }

    Vector3 operator*(const Vector3 &v) const {
        return Vector3(x * v.x, y * v.y, z * v.z);
    }

    Vector3 operator/(const I s) const {
        return Vector3(x / s, y / s, z / s);
    }

    Vector3 operator/(const Vector3 &v) const {
        return Vector3(x / v.x, y / v.y, z / v.z);
    }

    I mag() const {
        return (x * x + y * y + z * z).sqrt();
    }

    Vector2<I> project(const I theta, const I phi) const {
        const I sin_theta = theta.sin();
        const I cos_theta = theta.cos();
        return Vector2<I>(
            -x * sin_theta + y * cos_theta,
            (x * cos_theta + y * sin_theta) * phi.cos() - z * phi.sin()
        );
    }

    friend std::ostream &operator<<(std::ostream &os, const Vector3 &v) {
        return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    }
};

template<IntervalType I>
Vector2<I> operator+(const I s, const Vector2<I> &v) {
    return Vector2(s + v.x, s + v.y);
}

template<IntervalType I>
Vector2<I> operator-(const I s, const Vector2<I> &v) {
    return Vector2(s - v.x, s - v.y);
}

template<IntervalType I>
Vector2<I> operator*(const I s, const Vector2<I> &v) {
    return Vector2(s * v.x, s * v.y);
}

template<IntervalType I>
Vector2<I> operator/(const I s, const Vector2<I> &v) {
    return Vector2(s / v.x, s / v.y);
}

template<IntervalType I>
class Line2 {
public:
    Vector2<I> from;
    Vector2<I> to;

    explicit Line2(const Vector2<I> &from, const Vector2<I> &to) : from(from), to(to) {}

    bool intersects(const Line2 &other) const {
        return (
                   Vector2<I>::orient(from, to, other.from) *
                   Vector2<I>::orient(from, to, other.to)
               ).neg() && (
                   Vector2<I>::orient(other.from, other.to, from) *
                   Vector2<I>::orient(other.from, other.to, to)
               ).neg();
    }

    bool avoids(const Line2 &other) const {
        return (
                   Vector2<I>::orient(from, to, other.from) *
                   Vector2<I>::orient(from, to, other.to)
               ).pos() || (
                   Vector2<I>::orient(other.from, other.to, from) *
                   Vector2<I>::orient(other.from, other.to, to)
               ).pos();
    }

    bool intersects(const std::vector<Vector2<I>> &vertices) const {
        for(size_t i = 0; i < vertices.size(); i++) {
            if(intersects(Line2(vertices[i], vertices[(i + 1) % vertices.size()]))) {
                return true;
            }
        }
        return false;
    }

    bool avoids(const std::vector<Vector2<I>> &vertices) const {
        for(size_t i = 0; i < vertices.size(); i++) {
            if(!avoids(Line2(vertices[i], vertices[(i + 1) % vertices.size()]))) {
                return false;
            }
        }
        return true;
    }
};

template<IntervalType I>
Vector3<I> operator+(const I s, const Vector3<I> &v) {
    return Vector3(s + v.x, s + v.y, s + v.z);
}

template<IntervalType I>
Vector3<I> operator-(const I s, const Vector3<I> &v) {
    return Vector3(s - v.x, s - v.y, s - v.z);
}

template<IntervalType I>
Vector3<I> operator*(const I s, const Vector3<I> &v) {
    return Vector3(s * v.x, s * v.y, s * v.z);
}

template<IntervalType I>
Vector3<I> operator/(const I s, const Vector3<I> &v) {
    return Vector3(s / v.x, s / v.y, s / v.z);
}
