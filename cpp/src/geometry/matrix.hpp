#pragma once

#include "interval/interval.hpp"
#include "geometry/vertex.hpp"
#include <vector>

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

    Vertex<Interval> operator*(const Vertex<Interval>& vertex) const {
        return Vertex<Interval>(
            xx_ * vertex.x() + xy_ * vertex.y() + xz_ * vertex.z(),
            yx_ * vertex.x() + yy_ * vertex.y() + yz_ * vertex.z(),
            zx_ * vertex.x() + zy_ * vertex.y() + zz_ * vertex.z()
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

    Matrix basis_change(const Matrix& other_basis) const {
        return other_basis * transpose();
    }

    Interval cos_angle() const {
        return (xx_ + yy_ + zz_ - Interval(1)) / Interval(2);
    }

    static Interval cos_angle_between(const Matrix& matrix, const Matrix& other_matrix) {
        return matrix.basis_change(other_matrix).cos_angle();
    }

    static Matrix projection_matrix(const Interval& theta, const Interval& phi) {
        return rotate_x(phi) * rotate_z(theta);
    }

    static Matrix projection_rotation_matrix(const Interval& theta, const Interval& phi, const Interval& alpha) {
        return rotate_z(alpha) * projection_matrix(theta, phi);
    }
};

template<IntervalType Interval>
Matrix<Interval> orthonormal_basis(const Vertex<Interval>& from, const Vertex<Interval>& to, bool right_handed) {
    Vertex<Interval> x_axis = from.unit();
    Vertex<Interval> y_axis = (to - from * to.dot(x_axis)).unit();
    Vertex<Interval> z_axis = right_handed ? x_axis.cross(y_axis).unit() : y_axis.cross(x_axis).unit();
    return Matrix<Interval>(
        x_axis.x(), y_axis.x(), z_axis.x(),
        x_axis.y(), y_axis.y(), z_axis.y(),
        x_axis.z(), y_axis.z(), z_axis.z()
    );
}

template<IntervalType Interval>
std::vector<Matrix<Interval>> symmetries(
    const std::vector<Vertex<Interval>>& vertices,
    bool right_handed) {
    const Vertex<Interval> from_vertex = vertices[0];
    const Vertex<Interval> to_vertex = from_vertex.diff(-vertices[1]) ? vertices[1] : vertices[2];
    Matrix<Interval> basis = orthonormal_basis<Interval>(from_vertex, to_vertex, true);
    std::vector<Matrix<Interval>> symmetries;
    symmetries.reserve(vertices.size() * vertices.size());
    for(const auto& from_vertex_image: vertices) {
        for(const auto& to_vertex_image: vertices) {
            if(((from_vertex_image - to_vertex_image).len() - (from_vertex - to_vertex).len()).is_nonzero()) {
                continue;
            }
            Matrix<Interval> image_basis = orthonormal_basis<Interval>(from_vertex_image, to_vertex_image, right_handed);
            Matrix<Interval> symmetry = basis.basis_change(image_basis);
            const bool is_symmetry = std::ranges::all_of(vertices, [&](const Vertex<Interval>& vertex) {
                const Vertex<Interval> vertex_image = symmetry * vertex;
                return std::any_of(vertices.begin(), vertices.end(), [&](const Vertex<Interval>& other_vertex) {
                                       return !vertex_image.diff(other_vertex);
                                   }
                );
            });
            if(is_symmetry) {
                symmetries.emplace_back(symmetry);
            }
        }
    }
    return symmetries;
}
