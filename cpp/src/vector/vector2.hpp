#pragma once

#include "interval/interval.hpp"

template<IntervalType Interval>
struct Vector2Number {
private:
    using Number = typename Interval::Number;

    Number x_;
    Number y_;

public:
    explicit Vector2Number() : x_(), y_() {}

    ~Vector2Number() = default;

    Vector2Number(const Vector2Number& vector2) = default;

    Vector2Number& operator=(const Vector2Number& vector2) = default;

    Vector2Number(Vector2Number&& vector2) = default;

    Vector2Number& operator=(Vector2Number&& vector2) = default;

    explicit Vector2Number(const Number& x, const Number& y) : x_(x), y_(y) {}

    Number& x() {
        return x_;
    }

    Number& y() {
        return y_;
    }

    const Number& x() const {
        return x_;
    }

    const Number& y() const {
        return y_;
    }

    bool operator==(const Vector2Number& vector2) const {
        return x_ == vector2.x_ && y_ == vector2.y_;
    }

    bool operator!=(const Vector2Number& vector2) const {
        return x_ != vector2.x_ || y_ != vector2.y_;
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Vector2Number& vector2) {
        return ostream << "(" << vector2.x_ << " | " << vector2.y_ << ")";
    }
};

template<IntervalType Interval>
struct Vector2Interval {
private:
    Interval x_;
    Interval y_;

public:
    explicit Vector2Interval() : x_(), y_() {}

    ~Vector2Interval() = default;

    Vector2Interval(const Vector2Interval& vector2) = default;

    Vector2Interval& operator=(const Vector2Interval& vector2) = default;

    Vector2Interval(Vector2Interval&& vector2) = default;

    Vector2Interval& operator=(Vector2Interval&& vector2) = default;

    explicit Vector2Interval(const Interval& x, const Interval& y) : x_(x), y_(y) {}

    explicit Vector2Interval(const Vector2Number<Interval>& vector2) : x_(vector2.x()), y_(vector2.y()) {}

    Interval& x() {
        return x_;
    }

    Interval& y() {
        return y_;
    }

    const Interval& x() const {
        return x_;
    }

    const Interval& y() const {
        return y_;
    }

    Vector2Interval operator+() const {
        return Vector2Interval(+x_, +y_);
    }

    Vector2Interval operator-() const {
        return Vector2Interval(-x_, -y_);
    }

    Vector2Interval operator+(const Vector2Interval& vector2) const {
        return Vector2Interval(x_ + vector2.x_, y_ + vector2.y_);
    }

    Vector2Interval operator-(const Vector2Interval& vector2) const {
        return Vector2Interval(x_ - vector2.x_, y_ - vector2.y_);
    }

    Interval len() const {
        return (x_.sqr() + y_.sqr()).sqrt();
    }

    Interval len_sqr() const {
        return x_.sqr() + y_.sqr();
    }

    static Interval dot(const Vector2Interval& vector2, const Vector2Interval& other_vector2) {
        return vector2.x() * other_vector2.x() + vector2.y() * other_vector2.y();
    }

    static Interval cross(const Vector2Interval& vector2, const Vector2Interval& other_vector2) {
        return vector2.x() * other_vector2.y() - vector2.y() * other_vector2.x();
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Vector2Interval& vector2) {
        return ostream << "(" << vector2.x_ << " | " << vector2.y_ << ")";
    }
};
