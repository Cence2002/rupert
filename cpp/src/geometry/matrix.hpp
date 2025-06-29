#pragma once

#include "interval/interval.hpp"
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

    static Matrix reflect_x() {
        return Matrix(Interval(-1), Interval(0), Interval(0),
                      Interval(0), Interval(1), Interval(0),
                      Interval(0), Interval(0), Interval(1));
    }

    static Matrix reflect_y() {
        return Matrix(Interval(1), Interval(0), Interval(0),
                      Interval(0), Interval(-1), Interval(0),
                      Interval(0), Interval(0), Interval(1));
    }

    static Matrix reflect_z() {
        return Matrix(Interval(1), Interval(0), Interval(0),
                      Interval(0), Interval(1), Interval(0),
                      Interval(0), Interval(0), Interval(-1));
    }

    static Matrix rotate_x(const Interval& angle) {
        return Matrix(
            Interval(1), Interval(0), Interval(0),
            Interval(0), angle.cos(), -angle.sin(),
            Interval(0), angle.sin(), angle.cos()
        );
    }

    static Matrix rotate_y(const Interval& angle) {
        return Matrix(
            angle.cos(), Interval(0), angle.sin(),
            Interval(0), Interval(1), Interval(0),
            -angle.sin(), Interval(0), angle.cos()
        );
    }

    static Matrix rotate_z(const Interval& angle) {
        return Matrix(
            angle.cos(), -angle.sin(), Interval(0),
            angle.sin(), angle.cos(), Interval(0),
            Interval(0), Interval(0), Interval(1)
        );
    }

    Matrix transpose() const {
        return Matrix(xx_, yx_, zx_,
                      xy_, yy_, zy_,
                      xz_, yz_, zz_);
    }

    Vector3<Interval> operator*(const Vector3<Interval>& vector3) const {
        return Vector3<Interval>(
            xx_ * vector3.x() + xy_ * vector3.y() + xz_ * vector3.z(),
            yx_ * vector3.x() + yy_ * vector3.y() + yz_ * vector3.z(),
            zx_ * vector3.x() + zy_ * vector3.y() + zz_ * vector3.z()
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

    static Matrix relative_rotation(const Matrix& from, const Matrix& to) {
        return to * from.transpose();
    }

    Interval cos_angle() const {
        return (xx_ + yy_ + zz_ - Interval(1)) / Interval(2);
    }

    static Interval cos_angle_between(const Matrix& matrix_0, const Matrix& matrix_1) {
        return relative_rotation(matrix_0, matrix_1).cos_angle();
    }
};
