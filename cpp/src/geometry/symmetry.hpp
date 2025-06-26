#pragma once

#include <symmetry.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

template<IntervalType Interval>
bool is_centrally_symmetric(const std::vector<Vertex<Interval>>& vertices) {
    return std::all_of(vertices.begin(), vertices.end(), [&vertices](const Vertex<Interval>& vertex) {
        return std::any_of(vertices.begin(), vertices.end(), [&vertex](const Vertex<Interval>& other_vertex) {
            return !vertex.diff(-other_vertex);
        });
    });
}

namespace LinearAlgebra = boost::numeric::ublas;

template<IntervalType Interval>
using Matrix3 = LinearAlgebra::bounded_matrix<Interval, 3, 3, boost::numeric::ublas::row_major>;

template<IntervalType Interval>
Matrix3<Interval> transpose(const Matrix3<Interval>& matrix) {
    return LinearAlgebra::trans(matrix);
}

template<IntervalType Interval>
Matrix3<Interval> compose(const Matrix3<Interval>& matrix, const Matrix3<Interval>& other_matrix) {
    return LinearAlgebra::prod(other_matrix, matrix);
}

template<IntervalType Interval>
Matrix3<Interval> basis_change(const Matrix3<Interval>& basis, const Matrix3<Interval>& other_basis) {
    return compose(transpose(basis), other_basis);
}

template<IntervalType Interval>
Matrix3<Interval> zero_matrix() {
    Matrix3<Interval> matrix;
    matrix(0, 0) = Interval(0);
    matrix(0, 1) = Interval(0);
    matrix(0, 2) = Interval(0);
    matrix(1, 0) = Interval(0);
    matrix(1, 1) = Interval(0);
    matrix(1, 2) = Interval(0);
    matrix(2, 0) = Interval(0);
    matrix(2, 1) = Interval(0);
    matrix(2, 2) = Interval(0);
    return matrix;
}

template<IntervalType Interval>
Matrix3<Interval> identity_matrix() {
    Matrix3<Interval> matrix = zero_matrix<Interval>();
    matrix(0, 0) = Interval(1);
    matrix(1, 1) = Interval(1);
    matrix(2, 2) = Interval(1);
    return matrix;
}

template<IntervalType Interval>
Matrix3<Interval> reflection_matrix_x() {
    Matrix3<Interval> reflection = identity_matrix<Interval>();
    reflection(0, 0) = Interval(-1);
    return reflection;
}

template<IntervalType Interval>
Matrix3<Interval> reflection_matrix_y() {
    Matrix3<Interval> reflection = identity_matrix<Interval>();
    reflection(1, 1) = Interval(-1);
    return reflection;
}

template<IntervalType Interval>
Matrix3<Interval> reflection_matrix_z() {
    Matrix3<Interval> reflection = identity_matrix<Interval>();
    reflection(2, 2) = Interval(-1);
    return reflection;
}

template<IntervalType Interval>
Matrix3<Interval> rotation_matrix_x(const Interval& angle) {
    Matrix3<Interval> rotation = zero_matrix<Interval>();
    rotation(0, 0) = Interval(1);
    rotation(1, 1) = angle.cos();
    rotation(1, 2) = -angle.sin();
    rotation(2, 1) = angle.sin();
    rotation(2, 2) = angle.cos();
    return rotation;
}

template<IntervalType Interval>
Matrix3<Interval> rotation_matrix_y(const Interval& angle) {
    Matrix3<Interval> rotation = zero_matrix<Interval>();
    rotation(1, 1) = Interval(1);
    rotation(2, 2) = angle.cos();
    rotation(2, 0) = -angle.sin();
    rotation(0, 2) = angle.sin();
    rotation(0, 0) = angle.cos();
    return rotation;
}

template<IntervalType Interval>
Matrix3<Interval> rotation_matrix_z(const Interval& angle) {
    Matrix3<Interval> rotation = zero_matrix<Interval>();
    rotation(2, 2) = Interval(1);
    rotation(0, 0) = angle.cos();
    rotation(0, 1) = -angle.sin();
    rotation(1, 0) = angle.sin();
    rotation(1, 1) = angle.cos();
    return rotation;
}

template<IntervalType Interval>
Matrix3<Interval> projection_matrix(const Interval& theta, const Interval& phi) {
    const Matrix3<Interval> rotation_z = rotation_matrix_z(theta);
    const Matrix3<Interval> rotation_x = rotation_matrix_x(phi);
    return compose(rotation_z, rotation_x);
}

template<IntervalType Interval>
Matrix3<Interval> projection_rotation_matrix(const Interval& theta, const Interval& phi, const Interval& alpha) {
    const Matrix3<Interval> projection = projection_matrix(theta, phi);
    const Matrix3<Interval> rotation_z = rotation_matrix_z(alpha);
    return compose(projection, rotation_z);
}

template<IntervalType Interval>
Matrix3<Interval> orthonormal_basis(const Vertex<Interval>& from, const Vertex<Interval>& to, const bool right_handed) {
    const Vertex<Interval> x_axis = from.normalised();
    const Vertex<Interval> y_axis = (to - from * to.dot(x_axis)).normalised();
    const Vertex<Interval> z_axis = x_axis.cross(y_axis).normalised() * (right_handed ? Interval(1) : Interval(-1));
    Matrix3<Interval> basis;
    basis(0, 0) = x_axis.x();
    basis(0, 1) = y_axis.x();
    basis(0, 2) = z_axis.x();
    basis(1, 0) = x_axis.y();
    basis(1, 1) = y_axis.y();
    basis(1, 2) = z_axis.y();
    basis(2, 0) = x_axis.z();
    basis(2, 1) = y_axis.z();
    basis(2, 2) = z_axis.z();
    return basis;
}

template<IntervalType Interval>
Vertex<Interval> apply_matrix(const Matrix3<Interval>& matrix, const Vertex<Interval>& vertex) {
    return Vertex<Interval>(
        matrix(0, 0) * vertex.x() + matrix(1, 0) * vertex.y() + matrix(2, 0) * vertex.z(),
        matrix(0, 1) * vertex.x() + matrix(1, 1) * vertex.y() + matrix(2, 1) * vertex.z(),
        matrix(0, 2) * vertex.x() + matrix(1, 2) * vertex.y() + matrix(2, 2) * vertex.z()
    );
}

template<IntervalType Interval>
std::vector<Matrix3<Interval>> symmetries(const std::vector<Vertex<Interval>>& vertices, const bool right_handed) {
    const Vertex<Interval> from_vertex = vertices[0];
    const Vertex<Interval> to_vertex = from_vertex.diff(-vertices[1]) ? vertices[1] : vertices[2];
    const Matrix3<Interval> basis = orthonormal_basis(from_vertex, to_vertex, true);
    std::vector<Matrix3<Interval>> symmetries;
    for(const Vertex<Interval>& from_vertex_image: vertices) {
        for(const Vertex<Interval>& to_vertex_image: vertices) {
            if(((from_vertex_image - to_vertex_image).len() - (from_vertex - to_vertex).len()).is_nonzero()) {
                continue;
            }
            const Matrix3<Interval> image_basis = orthonormal_basis(from_vertex_image, to_vertex_image, right_handed);
            const Matrix3<Interval> symmetry = basis_change(basis, image_basis);
            const bool is_symmetry = std::ranges::all_of(vertices, [&](const Vertex<Interval>& vertex) {
                const Vertex<Interval> vertex_image = apply_matrix(symmetry, vertex);
                return std::any_of(vertices.begin(), vertices.end(), [&](const Vertex<Interval>& other_vertex) {
                    return !vertex_image.diff(other_vertex);
                });
            });
            if(is_symmetry) {
                symmetries.push_back(symmetry);
            }
        }
    }
    return symmetries;
}

template<IntervalType Interval>
Interval cos_angle_of_rotation(const Matrix3<Interval>& matrix) {
    return (matrix(0, 0) + matrix(1, 1) + matrix(2, 2) - Interval(1)) / Interval(2);
}

template<IntervalType Interval>
Interval cos_angle_between(const Matrix3<Interval>& matrix, const Matrix3<Interval>& other_matrix) {
    return cos_angle_of_rotation(basis_change(matrix, other_matrix));
}
