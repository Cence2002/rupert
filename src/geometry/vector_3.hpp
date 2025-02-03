#pragma once

#include "geometry/vector_type.hpp"

template<typename V, typename I>
concept Vector3Type =
        VectorType<V, I> &&

        std::is_constructible_v<V, I, I, I> &&
        std::is_constructible_v<V, I, I, int> &&
        std::is_constructible_v<V, I, int, I> &&
        std::is_constructible_v<V, I, int, int> &&
        std::is_constructible_v<V, int, I, I> &&
        std::is_constructible_v<V, int, I, int> &&
        std::is_constructible_v<V, int, int, I> &&
        std::is_constructible_v<V, int, int, int> &&

        requires(V vector) {
            { vector.x() } -> std::convertible_to<I&>;
            { vector.y() } -> std::convertible_to<I&>;
            { vector.z() } -> std::convertible_to<I&>;
        } &&

        requires(const V vector) {
            { vector.x() } -> std::convertible_to<const I&>;
            { vector.y() } -> std::convertible_to<const I&>;
            { vector.z() } -> std::convertible_to<const I&>;
        };

template<IntervalType I>
struct Vector3 {
private:
    I x_;
    I y_;
    I z_;

public:
    explicit Vector3() = default;

    ~Vector3() = default;

    Vector3(const Vector3& v) = default;

    Vector3& operator=(const Vector3& v) = default;

    Vector3(Vector3&& v) = default;

    Vector3& operator=(Vector3&& v) = default;

    explicit Vector3(const I& x, const I& y, const I& z) : x_(x), y_(y), z_(z) {}

    template<IntegerType Int>
    explicit Vector3(const I& x, const I& y, const Int z) : x_(x), y_(y), z_(I(z)) {}

    template<IntegerType Int>
    explicit Vector3(const I& x, const Int y, const I& z) : x_(x), y_(I(y)), z_(z) {}

    template<IntegerType Int>
    explicit Vector3(const I& x, const Int y, const Int z) : x_(x), y_(I(y)), z_(I(z)) {}

    template<IntegerType Int>
    explicit Vector3(const Int x, const I& y, const I& z) : x_(I(x)), y_(y), z_(z) {}

    template<IntegerType Int>
    explicit Vector3(const Int x, const I& y, const Int z) : x_(I(x)), y_(y), z_(I(z)) {}

    template<IntegerType Int>
    explicit Vector3(const Int x, const Int y, const I& z) : x_(I(x)), y_(I(y)), z_(z) {}

    template<IntegerType Int>
    explicit Vector3(const Int x, const Int y, const Int z) : x_(I(x)), y_(I(y)), z_(I(z)) {}

    I& x() {
        return x_;
    }

    I& y() {
        return y_;
    }

    I& z() {
        return z_;
    }

    const I& x() const {
        return x_;
    }

    const I& y() const {
        return y_;
    }

    const I& z() const {
        return z_;
    }

    Vector3 operator+() const {
        return Vector3(+x_, +y_, +z_);
    }

    Vector3 operator-() const {
        return Vector3(-x_, -y_, -z_);
    }

    Vector3 operator+(const Vector3& v) const {
        return Vector3(x_ + v.x_, y_ + v.y_, z_ + v.z_);
    }

    Vector3 operator+(const I& i) const {
        return Vector3(x_ + i, y_ + i, z_ + i);
    }

    template<IntegerType Int>
    Vector3 operator+(const Int n) const {
        return Vector3(x_ + n, y_ + n, z_ + n);
    }

    friend Vector3 operator+(const I& i, const Vector3& v) {
        return Vector3(i + v.x_, i + v.y_, i + v.z_);
    }

    template<IntegerType Int>
    friend Vector3 operator+(const Int n, const Vector3& v) {
        return Vector3(I(n) + v.x_, I(n) + v.y_, I(n) + v.z_);
    }

    Vector3 operator-(const Vector3& v) const {
        return Vector3(x_ - v.x_, y_ - v.y_, z_ - v.z_);
    }

    Vector3 operator-(const I& i) const {
        return Vector3(x_ - i, y_ - i, z_ - i);
    }

    template<IntegerType Int>
    Vector3 operator-(const Int n) const {
        return Vector3(x_ - n, y_ - n, z_ - n);
    }

    friend Vector3 operator-(const I& i, const Vector3& v) {
        return Vector3(i - v.x_, i - v.y_, i - v.z_);
    }

    template<IntegerType Int>
    friend Vector3 operator-(const Int n, const Vector3& v) {
        return Vector3(I(n) - v.x_, I(n) - v.y_, I(n) - v.z_);
    }

    Vector3 operator*(const Vector3& v) const {
        return Vector3(x_ * v.x_, y_ * v.y_, z_ * v.z_);
    }

    Vector3 operator*(const I& i) const {
        return Vector3(x_ * i, y_ * i, z_ * i);
    }

    template<IntegerType Int>
    Vector3 operator*(const Int n) const {
        return Vector3(x_ * n, y_ * n, z_ * n);
    }

    friend Vector3 operator*(const I& i, const Vector3& v) {
        return Vector3(i * v.x_, i * v.y_, i * v.z_);
    }

    template<IntegerType Int>
    friend Vector3 operator*(const Int n, const Vector3& v) {
        return Vector3(I(n) * v.x_, I(n) * v.y_, I(n) * v.z_);
    }

    Vector3 operator/(const Vector3& v) const {
        return Vector3(x_ / v.x_, y_ / v.y_, z_ / v.z_);
    }

    Vector3 operator/(const I& i) const {
        return Vector3(x_ / i, y_ / i, z_ / i);
    }

    template<IntegerType Int>
    Vector3 operator/(const Int n) const {
        return Vector3(x_ / n, y_ / n, z_ / n);
    }

    friend Vector3 operator/(const I& i, const Vector3& v) {
        return Vector3(i / v.x_, i / v.y_, i / v.z_);
    }

    template<IntegerType Int>
    friend Vector3 operator/(const Int n, const Vector3& v) {
        return Vector3(I(n) / v.x_, I(n) / v.y_, I(n) / v.z_);
    }

    I len() const {
        return (x_.sqr() + y_.sqr() + z_.sqr()).sqrt();
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector3& v) {
        return os << "(" << v.x_ << " , " << v.y_ << ", " << v.z_ << ")";
    }

    Vector2<I> project(const I& theta, const I& phi) const {
        const I sin_theta = theta.sin();
        const I cos_theta = theta.cos();
        return Vector2<I>(
            -x_ * sin_theta + y_ * cos_theta,
            (x_ * cos_theta + y_ * sin_theta) * phi.cos() - z_ * phi.sin()
        );
    }
};
