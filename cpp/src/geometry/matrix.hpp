#pragma once

#include "interval/intervals.hpp"
#include "geometry/vector3.hpp"

template<IntervalType Interval>
struct Matrix {
private:
    Interval xx_, xy_, xz_;
    Interval yx_, yy_, yz_;
    Interval zx_, zy_, zz_;

public:
    explicit Matrix(
        const Interval& xx, const Interval& xy, const Interval& xz,
        const Interval& yx, const Interval& yy, const Interval& yz,
        const Interval& zx, const Interval& zy, const Interval& zz
    ) : xx_(xx), xy_(xy), xz_(xz),
        yx_(yx), yy_(yy), yz_(yz),
        zx_(zx), zy_(zy), zz_(zz) {}

    ~Matrix() = default;

    Matrix(const Matrix& matrix) = default;

    Matrix(Matrix&& matrix) = default;

    Matrix& operator=(const Matrix&) = delete;

    Matrix& operator=(Matrix&&) = delete;

    static Matrix zero() {
        return Matrix(Interval(0), Interval(0), Interval(0),
                      Interval(0), Interval(0), Interval(0),
                      Interval(0), Interval(0), Interval(0));
    }

    static Matrix identity() {
        return Matrix(Interval(1), Interval(0), Interval(0),
                      Interval(0), Interval(1), Interval(0),
                      Interval(0), Interval(0), Interval(1));
    }

    Matrix transpose() const {
        return Matrix(xx_, yx_, zx_,
                      xy_, yy_, zy_,
                      xz_, yz_, zz_);
    }

    Vector3<Interval> operator*(const Vector3<Interval>& vector) const {
        return Vector3<Interval>(
            xx_ * vector.x() + xy_ * vector.y() + xz_ * vector.z(),
            yx_ * vector.x() + yy_ * vector.y() + yz_ * vector.z(),
            zx_ * vector.x() + zy_ * vector.y() + zz_ * vector.z()
        );
    }

    Matrix operator*(const Matrix& matrix) const {
        return Matrix(
            xx_ * matrix.xx_ + xy_ * matrix.yx_ + xz_ * matrix.zx_,
            xx_ * matrix.xy_ + xy_ * matrix.yy_ + xz_ * matrix.zy_,
            xx_ * matrix.xz_ + xy_ * matrix.yz_ + xz_ * matrix.zz_,

            yx_ * matrix.xx_ + yy_ * matrix.yx_ + yz_ * matrix.zx_,
            yx_ * matrix.xy_ + yy_ * matrix.yy_ + yz_ * matrix.zy_,
            yx_ * matrix.xz_ + yy_ * matrix.yz_ + yz_ * matrix.zz_,

            zx_ * matrix.xx_ + zy_ * matrix.yx_ + zz_ * matrix.zx_,
            zx_ * matrix.xy_ + zy_ * matrix.yy_ + zz_ * matrix.zy_,
            zx_ * matrix.xz_ + zy_ * matrix.yz_ + zz_ * matrix.zz_
        );
    }

    static Matrix reflection_x() {
        return Matrix(Interval(-1), Interval(0), Interval(0),
                      Interval(0), Interval(1), Interval(0),
                      Interval(0), Interval(0), Interval(1));
    }

    static Matrix reflection_y() {
        return Matrix(Interval(1), Interval(0), Interval(0),
                      Interval(0), Interval(-1), Interval(0),
                      Interval(0), Interval(0), Interval(1));
    }

    static Matrix reflection_z() {
        return Matrix(Interval(1), Interval(0), Interval(0),
                      Interval(0), Interval(1), Interval(0),
                      Interval(0), Interval(0), Interval(-1));
    }

    static Matrix rotation_x(const Interval& angle) {
        return Matrix(
            Interval(1), Interval(0), Interval(0),
            Interval(0), angle.cos(), -angle.sin(),
            Interval(0), angle.sin(), angle.cos()
        );
    }

    static Matrix rotation_y(const Interval& angle) {
        return Matrix(
            angle.cos(), Interval(0), angle.sin(),
            Interval(0), Interval(1), Interval(0),
            -angle.sin(), Interval(0), angle.cos()
        );
    }

    static Matrix rotation_z(const Interval& angle) {
        return Matrix(
            angle.cos(), -angle.sin(), Interval(0),
            angle.sin(), angle.cos(), Interval(0),
            Interval(0), Interval(0), Interval(1)
        );
    }

    static Matrix orientation(const Interval& theta, const Interval& phi) {
        return rotation_x(phi) * rotation_z(theta);
    }

    static Matrix relative_rotation(const Matrix& from, const Matrix& to) {
        return to * from.transpose();
    }

    Interval cos_angle() const {
        return (xx_ + yy_ + zz_ - Interval(1)) / Interval(2);
    }

    static Interval cos_angle_between(const Matrix& matrix_0, const Matrix& matrix_1) {
        return relative_rotation(matrix_0, matrix_1).cos_angle();
    }

    friend std::ostream& operator<<(std::ostream& os, const Matrix& matrix) {
        return os << "[" <<
               "[" << matrix.xx_ << "," << matrix.xy_ << "," << matrix.xz_ << "]," <<
               "[" << matrix.yx_ << "," << matrix.yy_ << "," << matrix.yz_ << "]," <<
               "[" << matrix.zx_ << "," << matrix.zy_ << "," << matrix.zz_ << "]" <<
               "]";
    }
};
