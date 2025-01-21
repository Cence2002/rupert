#pragma once

#include "interval.hpp"
#include <vector>

template<typename T, typename C>
concept VectorType =
        requires(T a) {
            { +a } -> std::same_as<T>;
            { -a } -> std::same_as<T>;
            { a.mag() } -> std::same_as<C>;
        } &&
        requires(const T a, const T b) {
            { a + b } -> std::same_as<T>;
            { a - b } -> std::same_as<T>;
            { a * b } -> std::same_as<T>;
            { a / b } -> std::same_as<T>;
        } &&
        requires(const T a, const C b) {
            { a + b } -> std::same_as<T>;
            { a - b } -> std::same_as<T>;
            { a * b } -> std::same_as<T>;
            { a / b } -> std::same_as<T>;
        };

template<typename T, typename C>
concept Vector2Type =
        VectorType<T, C> &&
        requires(T a) {
            { a.x } -> std::convertible_to<C>;
            { a.y } -> std::convertible_to<C>;
        } &&
        std::is_constructible_v<T, C, C>;

template<typename T, typename C>
concept Vector3Type =
        VectorType<T, C> &&
        requires(const T a) {
            { a.x } -> std::convertible_to<C>;
            { a.x } -> std::convertible_to<C>;
            { a.x } -> std::convertible_to<C>;
        } &&
        std::is_constructible_v<T, C, C, C>;

template<IntervalType T>
class Vector2 {
public:
    T x, y;

    explicit Vector2(const T x, const T y) : x(x), y(y) {}

    Vector2 operator+() const {
        return Vector2(+x, +y);
    }

    Vector2 operator-() const {
        return Vector2(-x, -y);
    }

    Vector2 operator+(const T s) const {
        return Vector2(x + s, y + s);
    }

    Vector2 operator+(const Vector2 &v) const {
        return Vector2(x + v.x, y + v.y);
    }

    Vector2 operator-(const T s) const {
        return Vector2(x - s, y - s);
    }

    Vector2 operator-(const Vector2 &v) const {
        return Vector2(x - v.x, y - v.y);
    }

    Vector2 operator*(const T s) const {
        return Vector2(x * s, y * s);
    }

    Vector2 operator*(const Vector2 &v) const {
        return Vector2(x * v.x, y * v.y);
    }

    Vector2 operator/(const T s) const {
        return Vector2(x / s, y / s);
    }

    Vector2 operator/(const Vector2 &v) const {
        return Vector2(x / v.x, y / v.y);
    }

    T mag() const {
        return (x * x + y * y).sqrt();
    }

    Vector2 rotate(const T angle) const {
        const T sin_angle = angle.sin();
        const T cos_angle = angle.cos();
        return Vector2(
            x * cos_angle - y * sin_angle,
            x * sin_angle + y * cos_angle
        );
    }

    static T cross(const Vector2 &a, const Vector2 &b) {
        return a.x * b.y - a.y * b.x;
    }

    static bool line_avoids_line(const Vector2 &a, const Vector2 &b, const Vector2 &c, const Vector2 &d) {
        const Vector2 ab = b - a;
        const Vector2 cd = d - c;
        const Vector2 ac = c - a;
        const Vector2 bd = d - b;
        return (cross(ab, ac) * cross(ab, bd)).pos() || (cross(cd, ac) * cross(cd, bd)).pos();
    }

    static bool line_intersects_line(const Vector2 &a, const Vector2 &b, const Vector2 &c, const Vector2 &d) {
        const Vector2 ab = b - a;
        const Vector2 cd = d - c;
        const Vector2 ac = c - a;
        const Vector2 bd = d - b;
        return (cross(ab, ac) * cross(ab, bd)).neg() && (cross(cd, ac) * cross(cd, bd)).neg();
    }

    static bool line_avoids_polygon(const Vector2 &a, const Vector2 &b, const std::vector<Vector2> &vertices) {
        for(size_t i = 0; i < vertices.size(); i++) {
            if(!line_avoids_line(a, b, vertices[i], vertices[(i + 1) % vertices.size()])) {
                return false;
            }
        }
        return true;
    }

    static bool line_intersects_polygon(const Vector2 &a, const Vector2 &b, const std::vector<Vector2> &vertices) {
        for(size_t i = 0; i < vertices.size(); i++) {
            if(line_intersects_line(a, b, vertices[i], vertices[(i + 1) % vertices.size()])) {
                return true;
            }
        }
        return false;
    }

    static bool point_inside_polygon(const Vector2 &point, const std::vector<Vector2> &vertices) {
        const Vector2 origin = Vector2(0, 0);
        return line_avoids_polygon(origin, point, vertices);
    }

    static bool point_outside_polygon(const Vector2 &point, const std::vector<Vector2> &vertices) {
        const Vector2 origin = Vector2(0, 0);
        return line_intersects_polygon(origin, point, vertices);
    }
};

template<IntervalType T>
class Vector3 {
public:
    T x, y, z;

    explicit Vector3(const T x, const T y, const T z) : x(x), y(y), z(z) {}

    Vector3 operator+() const {
        return Vector3(+x, +y, +z);
    }

    Vector3 operator-() const {
        return Vector3(-x, -y, -z);
    }

    Vector3 operator+(const T s) const {
        return Vector3(x + s, y + s, z + s);
    }

    Vector3 operator+(const Vector3 &v) const {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }

    Vector3 operator-(const T s) const {
        return Vector3(x - s, y - s, z - s);
    }

    Vector3 operator-(const Vector3 &v) const {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }

    Vector3 operator*(const T s) const {
        return Vector3(x * s, y * s, z * s);
    }

    Vector3 operator*(const Vector3 &v) const {
        return Vector3(x * v.x, y * v.y, z * v.z);
    }

    Vector3 operator/(const T s) const {
        return Vector3(x / s, y / s, z / s);
    }

    Vector3 operator/(const Vector3 &v) const {
        return Vector3(x / v.x, y / v.y, z / v.z);
    }

    T mag() const {
        return (x * x + y * y + z * z).sqrt();
    }

    Vector2<T> project(const T theta, const T phi) const {
        const T sin_theta = theta.sin();
        const T cos_theta = theta.cos();
        return Vector2<T>(
            -x * sin_theta + y * cos_theta,
            (x * cos_theta + y * sin_theta) * phi.cos() - z * phi.sin()
        );
    }
};
