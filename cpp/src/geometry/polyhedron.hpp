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
    explicit Polyhedron(
        const std::vector<Vector3<Interval>>& vertices,
        const std::vector<Matrix<Interval>>& rotations,
        const std::vector<Matrix<Interval>>& reflections
    ) : vertices_(vertices),
        rotations_(rotations),
        reflections_(reflections) {}

    void add_symmetries(const std::vector<Matrix<Interval>>& rotations, const std::vector<Matrix<Interval>>& reflections) {
        rotations_.insert(rotations_.end(), rotations.begin(), rotations.end());
        reflections_.insert(reflections_.end(), reflections.begin(), reflections.end());
    }

    // rotations_ = symmetries(vertices_, true);
    // reflections_ = symmetries(vertices_, false);

    const std::vector<Vector3<Interval>>& vertices() const {
        return vertices_;
    }

    const std::vector<Matrix<Interval>>& rotations() const {
        return rotations_;
    }

    const std::vector<Matrix<Interval>>& reflections() const {
        return reflections_;
    }

    Polyhedron normalized() const {
        size_t max_index = 0;
        for(size_t i = 1; i < vertices_.size(); i++) {
            if(vertices_[i].len() > vertices_[max_index].len()) {
                max_index = i;
            }
        }
        const Interval max_length = vertices_[max_index].len().max();
        std::vector<Vector3<Interval>> normalized_vertices;
        for(const Vector3<Interval>& vertex: vertices_) {
            normalized_vertices.emplace_back(vertex / max_length);
        }
        return Polyhedron(normalized_vertices, rotations_, reflections_);
    }
};
