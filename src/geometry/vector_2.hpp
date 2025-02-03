#pragma once

#include "geometry/vector_type.hpp"

template<typename V, typename I>
concept Vector2Type =
        VectorType<V, I> &&

        std::is_constructible_v<V, I, I> &&
        std::is_constructible_v<V, I, int> &&
        std::is_constructible_v<V, int, I> &&
        std::is_constructible_v<V, int, int> &&

        requires(V vector) {
            { vector.x() } -> std::convertible_to<I&>;
            { vector.y() } -> std::convertible_to<I&>;
        } &&

        requires(const V vector) {
            { vector.x() } -> std::convertible_to<const I&>;
            { vector.y() } -> std::convertible_to<const I&>;
        };

template<IntervalType I>
struct Vector2 {
private:
    I x_;
    I y_;

public:
    explicit Vector2() : x_(), y_() {}

    ~Vector2() = default;

    Vector2(const Vector2& v) = default;

    Vector2& operator=(const Vector2& v) = default;

    Vector2(Vector2&& v) = default;

    Vector2& operator=(Vector2&& v) = default;

    explicit Vector2(const I& x, const I& y) : x_(x), y_(y) {}

    template<IntegerType Int>
    explicit Vector2(const I& x, const Int y) : x_(x), y_(I(y)) {}

    template<IntegerType Int>
    explicit Vector2(const Int x, const I& y) : x_(I(x)), y_(y) {}

    template<IntegerType Int>
    explicit Vector2(const Int x, const Int y) : x_(I(x)), y_(I(y)) {}

    I& x() {
        return x_;
    }

    I& y() {
        return y_;
    }

    const I& x() const {
        return x_;
    }

    const I& y() const {
        return y_;
    }

    Vector2 operator+() const {
        return Vector2(+x_, +y_);
    }

    Vector2 operator-() const {
        return Vector2(-x_, -y_);
    }

    Vector2 operator+(const Vector2& v) const {
        return Vector2(x_ + v.x_, y_ + v.y_);
    }

    Vector2 operator+(const I& i) const {
        return Vector2(x_ + i, y_ + i);
    }

    template<IntegerType Int>
    Vector2 operator+(const Int n) const {
        return Vector2(x_ + n, y_ + n);
    }

    friend Vector2 operator+(const I& i, const Vector2& v) {
        return Vector2(i + v.x_, i + v.y_);
    }

    template<IntegerType Int>
    friend Vector2 operator+(const Int n, const Vector2& v) {
        return Vector2(I(n) + v.x_, I(n) + v.y_);
    }

    Vector2 operator-(const Vector2& v) const {
        return Vector2(x_ - v.x_, y_ - v.y_);
    }

    Vector2 operator-(const I& i) const {
        return Vector2(x_ - i, y_ - i);
    }

    template<IntegerType Int>
    Vector2 operator-(const Int n) const {
        return Vector2(x_ - n, y_ - n);
    }

    friend Vector2 operator-(const I& i, const Vector2& v) {
        return Vector2(i - v.x_, i - v.y_);
    }

    template<IntegerType Int>
    friend Vector2 operator-(const Int n, const Vector2& v) {
        return Vector2(I(n) - v.x_, I(n) - v.y_);
    }

    Vector2 operator*(const Vector2& v) const {
        return Vector2(x_ * v.x_, y_ * v.y_);
    }

    Vector2 operator*(const I& i) const {
        return Vector2(x_ * i, y_ * i);
    }

    template<IntegerType Int>
    Vector2 operator*(const Int n) const {
        return Vector2(x_ * n, y_ * n);
    }

    friend Vector2 operator*(const I& i, const Vector2& v) {
        return Vector2(i * v.x_, i * v.y_);
    }

    template<IntegerType Int>
    friend Vector2 operator*(const Int n, const Vector2& v) {
        return Vector2(I(n) * v.x_, I(n) * v.y_);
    }

    Vector2 operator/(const Vector2& v) const {
        return Vector2(x_ / v.x_, y_ / v.y_);
    }

    Vector2 operator/(const I& i) const {
        return Vector2(x_ / i, y_ / i);
    }

    template<IntegerType Int>
    Vector2 operator/(const Int n) const {
        return Vector2(x_ / n, y_ / n);
    }

    friend Vector2 operator/(const I& i, const Vector2& v) {
        return Vector2(i / v.x_, i / v.y_);
    }

    template<IntegerType Int>
    friend Vector2 operator/(const Int n, const Vector2& v) {
        return Vector2(I(n) / v.x_, I(n) / v.y_);
    }

    I len() const {
        return (x_.sqr() + y_.sqr()).sqrt();
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector2& v) {
        return os << "(" << v.x_ << " , " << v.y_ << ")";
    }

    Vector2 rotate(const I& angle) const {
        const I sin_angle = angle.sin();
        const I cos_angle = angle.cos();
        return Vector2(
            x_ * cos_angle - y_ * sin_angle,
            x_ * sin_angle + y_ * cos_angle
        );
    }

    I cross(const Vector2& v) const {
        return x_ * v.y_ - y_ * v.x_;
    }

    Orientation orientation(const Vector2& v) const {
        const I cross_product = cross(v);
        if(cross_product.pos()) {
            return Orientation::COUNTERCLOCKWISE;
        }
        if(cross_product.neg()) {
            return Orientation::CLOCKWISE;
        }
        return Orientation::COLLINEAR;
    }
};
