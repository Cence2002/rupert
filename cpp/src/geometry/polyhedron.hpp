#pragma once

#include "geometry/vector3.hpp"
#include "geometry/matrix.hpp"
#include <vector>
#include <set>

template<IntervalType Interval>
class Polyhedron {
private:
    std::vector<Vector3<Interval>> vertices_;
    // any trivially non-negative value (distances and angles) smaller than this equals zero
    Interval epsilon_; // TODO this has to be symbolically verified

    std::vector<Vector3<Interval>> face_normals_{};
    std::vector<std::vector<size_t>> faces_{};

    std::vector<Matrix<Interval>> rotations_{};
    std::vector<Matrix<Interval>> reflections_{};

    void check_centrally_symmetric() {
        if(!std::ranges::all_of(vertices_, [&](const Vector3<Interval>& vertex) {
            return std::ranges::any_of(vertices_, [&](const Vector3<Interval>& other_vertex) {
                return !(vertex + other_vertex).len().pos();
            });
        })) {
            throw std::runtime_error("Polyhedron is not centrally symmetric");
        }
    }

    static Matrix<Interval> orthonormal_basis(const Vector3<Interval>& from, const Vector3<Interval>& to, const bool right_handed) {
        const Vector3<Interval> x_axis = from.unit();
        const Vector3<Interval> y_axis = (to - x_axis * to.dot(x_axis)).unit();
        const Vector3<Interval> z_axis = right_handed ? x_axis.cross(y_axis).unit() : y_axis.cross(x_axis).unit();
        return Matrix<Interval>(
            x_axis.x(), y_axis.x(), z_axis.x(),
            x_axis.y(), y_axis.y(), z_axis.y(),
            x_axis.z(), y_axis.z(), z_axis.z()
        );
    }

    void setup_symmetries() {
        rotations_.clear();
        reflections_.clear();

        const Vector3<Interval> from = vertices_[0];
        const Vector3<Interval> to = (from + vertices_[1]).len().pos() ? vertices_[1] : vertices_[2];
        const Matrix<Interval> basis = orthonormal_basis(from, to, true);
        for(const bool right_handed: {true, false}) {
            for(const auto& from_image: vertices_) {
                for(const auto& to_image: vertices_) {
                    const Matrix<Interval> image_basis = orthonormal_basis(from_image, to_image, right_handed);
                    const Matrix<Interval> symmetry = Matrix<Interval>::relative_rotation(basis, image_basis);
                    if(std::ranges::all_of(vertices_, [&](const Vector3<Interval>& vertex) {
                        const Vector3<Interval> vertex_image = symmetry * vertex;
                        return std::ranges::any_of(vertices_, [&](const Vector3<Interval>& other_vertex) {
                            return !vertex_image.diff(other_vertex);
                        });
                    })) {
                        if(right_handed) {
                            rotations_.push_back(symmetry);
                        } else {
                            reflections_.push_back(symmetry);
                        }
                    }
                }
            }
        }
    }

    void setup_faces() {
        face_normals_.clear();
        faces_.clear();

        std::vector<std::pair<std::tuple<size_t, size_t, size_t>, size_t>> triangles_with_normal_indices;
        for(size_t index_0 = 0; index_0 < vertices_.size(); ++index_0) {
            for(size_t index_1 = 0; index_1 < vertices_.size(); ++index_1) {
                for(size_t index_2 = 0; index_2 < vertices_.size(); ++index_2) {
                    if(index_0 == index_1 || index_1 == index_2 || index_2 == index_0) {
                        continue;
                    }
                    const Vector3<Interval> vertex_0 = vertices_[index_0];
                    const Vector3<Interval> vertex_1 = vertices_[index_1];
                    const Vector3<Interval> vertex_2 = vertices_[index_2];
                    const Vector3<Interval> normal = (vertex_1 - vertex_0).cross(vertex_2 - vertex_0).unit();
                    if(std::ranges::any_of(vertices_, [&](const Vector3<Interval>& vertex) {
                        return normal.dot(vertex - vertex_0).pos();
                    })) {
                        continue;
                    }
                    const auto normal_index_iterator = std::ranges::find_if(face_normals_, [&](const Vector3<Interval>& face_normal) {
                        return !face_normal.diff(normal);
                    });
                    const size_t normal_index = std::distance(face_normals_.begin(), normal_index_iterator);
                    if(normal_index_iterator == face_normals_.end()) {
                        face_normals_.push_back(normal);
                    }
                    triangles_with_normal_indices.emplace_back(std::make_tuple(index_0, index_1, index_2), normal_index);
                }
            }
        }

        std::vector<std::pair<std::pair<size_t, size_t>, size_t>> triangle_edges_with_normal_indices;
        for(const auto& [triangle, normal_index]: triangles_with_normal_indices) {
            const size_t index_0 = std::get<0>(triangle);
            const size_t index_1 = std::get<1>(triangle);
            const size_t index_2 = std::get<2>(triangle);
            triangle_edges_with_normal_indices.emplace_back(std::minmax(index_0, index_1), normal_index);
            triangle_edges_with_normal_indices.emplace_back(std::minmax(index_1, index_2), normal_index);
            triangle_edges_with_normal_indices.emplace_back(std::minmax(index_2, index_0), normal_index);
        }

        std::set<std::pair<std::pair<size_t, size_t>, size_t>> edges_with_normal_indices;
        for(const auto& [edge, normal_index]: triangle_edges_with_normal_indices) {
            if(std::ranges::any_of(triangle_edges_with_normal_indices, [&](const auto& other_edge_with_normal_index) {
                const auto& [other_edge, other_normal_index] = other_edge_with_normal_index;
                return edge == other_edge && normal_index != other_normal_index;
            })) {
                edges_with_normal_indices.emplace(edge, normal_index);
            }
        }

        for(size_t normal_index = 0; normal_index < face_normals_.size(); ++normal_index) {
            std::set<size_t> vertex_indices;
            for(const auto& [edge, edge_normal_index]: edges_with_normal_indices) {
                if(normal_index == edge_normal_index) {
                    vertex_indices.insert(edge.first);
                    vertex_indices.insert(edge.second);
                }
            }

            const auto min_element = std::ranges::min_element(vertex_indices);
            const Vector3<Interval> face_interior = (vertices_[*min_element] + vertices_[*std::next(min_element)] + vertices_[*std::next(min_element, 2)]) / Interval(3);

            std::vector<size_t> face;

            const size_t first_index = *min_element;
            face.push_back(first_index);
            vertex_indices.erase(first_index);

            while(!vertex_indices.empty()) {
                const size_t last_index = face.back();
                const size_t next_index = *std::ranges::max_element(vertex_indices, [&](const size_t index_0, const size_t index_1) {
                    return (vertices_[last_index] - face_interior).unit().cross((vertices_[index_0] - face_interior).unit()).dot(face_normals_[normal_index]) <
                           (vertices_[last_index] - face_interior).unit().cross((vertices_[index_1] - face_interior).unit()).dot(face_normals_[normal_index]);
                });
                face.push_back(next_index);
                vertex_indices.erase(next_index);
            }
            faces_.push_back(face);
        }
    }

    void setup_silhouettes() {
        sil
    }

    void setup() {
        check_centrally_symmetric();
        setup_faces();
        setup_symmetries();
        setup_silhouettes();
    }

public:
    explicit Polyhedron(const std::vector<Vector3<Interval>>& vertices) : vertices_(vertices), epsilon_(Interval(1) / Interval(1000000)) {
        setup();
    }

    const std::vector<Vector3<Interval>>& vertices() const {
        return vertices_;
    }

    const Interval& epsilon() const {
        return epsilon_;
    }

    const std::vector<Vector3<Interval>>& face_normals() const {
        return face_normals_;
    }

    const std::vector<std::vector<size_t>>& faces() const {
        return faces_;
    }

    const std::vector<Matrix<Interval>>& rotations() const {
        return rotations_;
    }

    const std::vector<Matrix<Interval>>& reflections() const {
        return reflections_;
    }
};
