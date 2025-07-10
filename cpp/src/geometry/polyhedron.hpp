#pragma once

#include "geometry/vector3.hpp"
#include "geometry/matrix.hpp"
#include <vector>

template<IntervalType Interval>
struct Polyhedron {
private:
    std::vector<Vector3<Interval>> vertices_;
    // std::vector<std::pair<size_t, size_t>> edge_indices_;
    // std::vector<std::vector<size_t>> face_indices_;
    // std::vector<Vertex<Interval>> face_normals_;
    // // any always non-negative value (distances and angles) smaller than this IS zero
    // Interval epsilon_; // TODO has to be symbolically verified with an external system

    std::vector<Matrix<Interval>> rotations_;
    std::vector<Matrix<Interval>> reflections_;

public:
    explicit Polyhedron(const std::vector<Vector3<Interval>>& vertices) : vertices_(vertices), rotations_(), reflections_() {
        if(!is_centrally_symmetric(vertices_)) {
            throw std::runtime_error("Polyhedron is not centrally symmetric");
        }
        rotations_ = symmetries<Interval>(vertices_, true);
        reflections_ = symmetries<Interval>(vertices_, false);
    }

    const std::vector<Vector3<Interval>>& vertices() const {
        return vertices_;
    }

    const std::vector<Matrix<Interval>>& rotations() const {
        return rotations_;
    }

    const std::vector<Matrix<Interval>>& reflections() const {
        return reflections_;
    }
};

template<IntervalType Interval>
bool is_centrally_symmetric(const std::vector<Vector3<Interval>>& vertices) {
    return std::all_of(vertices.begin(), vertices.end(), [&vertices](const Vector3<Interval>& vertex) {
        return std::any_of(vertices.begin(), vertices.end(), [&vertex](const Vector3<Interval>& other_vertex) {
            return !vertex.diff(-other_vertex);
        });
    });
}

template<IntervalType Interval>
Matrix<Interval> orthonormal_basis(const Vector3<Interval>& from, const Vector3<Interval>& to, const bool right_handed) {
    const Vector3<Interval> x_axis = from.unit();
    const Vector3<Interval> y_axis = (to - x_axis * to.dot(x_axis)).unit();
    const Vector3<Interval> z_axis = right_handed ? x_axis.cross(y_axis).unit() : y_axis.cross(x_axis).unit();
    return Matrix<Interval>(
        x_axis.x(), y_axis.x(), z_axis.x(),
        x_axis.y(), y_axis.y(), z_axis.y(),
        x_axis.z(), y_axis.z(), z_axis.z()
    );
}

template<IntervalType Interval>
std::vector<Matrix<Interval>> symmetries(const std::vector<Vector3<Interval>>& vectors, const bool right_handed) {
    const Vector3<Interval> from_vector = vectors[0];
    size_t to_index = 1;
    for(size_t i = 2; i < vectors.size(); i++) {
        if(vectors[i].diff(from_vector) && vectors[i].dist(from_vector) < vectors[to_index].dist(from_vector)) {
            to_index = i;
        }
    }
    const Vector3<Interval> to_vector = vectors[to_index];
    const Interval distance = to_vector.dist(from_vector);
    Matrix<Interval> basis = orthonormal_basis<Interval>(from_vector, to_vector, true);
    std::vector<Matrix<Interval>> symmetries;
    for(const auto& from_vector_image: vectors) {
        for(const auto& to_vector_image: vectors) {
            if((to_vector_image.dist(from_vector_image) - distance).nonz()) {
                continue;
            }
            Matrix<Interval> image_basis = orthonormal_basis<Interval>(from_vector_image, to_vector_image, right_handed);
            Matrix<Interval> symmetry = Matrix<Interval>::relative_rotation(basis, image_basis);
            const bool is_symmetry = std::ranges::all_of(vectors, [&](const Vector3<Interval>& vector) {
                const Vector3<Interval> vector_image = symmetry * vector;
                return std::ranges::any_of(vectors, [&](const Vector3<Interval>& other_vector) {
                                               return !vector_image.diff(other_vector);
                                           }
                );
            });
            if(is_symmetry) {
                symmetries.push_back(symmetry);
            }
        }
    }
    return symmetries;
}
