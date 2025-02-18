#pragma once

#include "vector/vector_type.hpp"

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

    explicit Vector3(const Interval& x, const Interval& y, const Interval& z) : x_(x), y_(y), z_(z) {}

    template<IntegerType Int>
    explicit Vector3(const Interval& x, const Interval& y, const Int z) : x_(x), y_(y), z_(Interval(z)) {}

    template<IntegerType Int>
    explicit Vector3(const Interval& x, const Int y, const Interval& z) : x_(x), y_(Interval(y)), z_(z) {}

    template<IntegerType Int>
    explicit Vector3(const Interval& x, const Int y, const Int z) : x_(x), y_(Interval(y)), z_(Interval(z)) {}

    template<IntegerType Int>
    explicit Vector3(const Int x, const Interval& y, const Interval& z) : x_(Interval(x)), y_(y), z_(z) {}

    template<IntegerType Int>
    explicit Vector3(const Int x, const Interval& y, const Int z) : x_(Interval(x)), y_(y), z_(Interval(z)) {}

    template<IntegerType Int>
    explicit Vector3(const Int x, const Int y, const Interval& z) : x_(Interval(x)), y_(Interval(y)), z_(z) {}

    template<IntegerType Int>
    explicit Vector3(const Int x, const Int y, const Int z) : x_(Interval(x)), y_(Interval(y)), z_(Interval(z)) {}

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

    Interval len() const {
        return (x_.sqr() + y_.sqr() + z_.sqr()).sqrt();
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector3& vector3) {
        return os << "(" << vector3.x_ << " | " << vector3.y_ << " | " << vector3.z_ << ")";
    }

    Vector2<Interval> project(const Interval& phi, const Interval& theta) const {
        const Interval& cos_phi = phi.cos();
        const Interval& sin_phi = phi.sin();
        const Interval& cos_theta = theta.cos();
        const Interval& sin_theta = theta.sin();
        return Vector2<Interval>(
            -x_ * sin_phi + y_ * cos_phi,
            (x_ * cos_phi + y_ * sin_phi) * cos_theta - z_ * sin_theta
        );
    }

    std::vector<Vector2<Interval>> project_hull(const Interval& phi, const Interval& theta) const {
        const Interval cos_phi = phi.cos();
        const Interval sin_phi = phi.sin();
        const Interval cos_theta = theta.cos();
        const Interval sin_theta = theta.sin();
        const Vector2<Interval> projection = Vector2<Interval>(
            -x_ * sin_phi + y_ * cos_phi,
            (x_ * cos_phi + y_ * sin_phi) * cos_theta - z_ * sin_theta
        );
        return std::vector<Vector2<Interval>>{
            Vector2<Interval>(Interval(projection.x().min()), Interval(projection.y().min())),
            Vector2<Interval>(Interval(projection.x().min()), Interval(projection.y().max())),
            Vector2<Interval>(Interval(projection.x().max()), Interval(projection.y().min())),
            Vector2<Interval>(Interval(projection.x().max()), Interval(projection.y().max()))
        };
    }
};
