#pragma once

#include "geometry/vector3.hpp"
#include "geometry/matrix.hpp"
#include <vector>
#include <set>
#include <map>
#include <boost/dynamic_bitset.hpp>
#include <boost/algorithm/string/join.hpp>

using Bitset = boost::dynamic_bitset<>;

struct Outline {
    Bitset normal_mask;
    std::vector<size_t> vertex_indices;

    bool same_vertex_indices(const std::vector<size_t>& other_vertex_indices) const {
        if(vertex_indices.size() != other_vertex_indices.size()) {
            return false;
        }
        const size_t size = vertex_indices.size();
        for(size_t offset = 0; offset < size; ++offset) {
            bool same = true;
            for(size_t index = 0; index < size; ++index) {
                if(vertex_indices[index] != other_vertex_indices[(index + offset) % size]) {
                    same = false;
                    break;
                }
            }
            if(same) {
                return true;
            }
        }
        return false;
    }
};

template<IntervalType Interval>
class Polyhedron {
    std::vector<Vector3<Interval>> vertices_;

    std::vector<Vector3<Interval>> face_normals_{};
    std::vector<std::vector<size_t>> faces_{};

    std::vector<Outline> outlines_{};

    std::vector<Matrix<Interval>> rotations_{};
    std::vector<Matrix<Interval>> reflections_{};

    std::vector<std::vector<size_t>> outline_rotations_{};
    std::vector<std::vector<size_t>> outline_reflections_{};

    void check_centrally_symmetric() {
        if(!std::ranges::all_of(vertices_, [&](const Vector3<Interval>& vertex) {
            return std::ranges::any_of(vertices_, [&](const Vector3<Interval>& other_vertex) {
                return !other_vertex.diff(-vertex);
            });
        })) {
            throw std::runtime_error("Polyhedron is not centrally symmetric");
        }
    }

    void setup_faces() {
        face_normals_.clear();
        faces_.clear();

        std::vector<std::pair<std::pair<size_t, size_t>, size_t>> triangle_edges_with_normal_indices;
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
                    triangle_edges_with_normal_indices.emplace_back(std::minmax(index_0, index_1), normal_index);
                    triangle_edges_with_normal_indices.emplace_back(std::minmax(index_1, index_2), normal_index);
                    triangle_edges_with_normal_indices.emplace_back(std::minmax(index_2, index_0), normal_index);
                }
            }
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
                const Vector3<Interval> projected_last_vertex = (vertices_[last_index] - face_interior).unit();
                const size_t next_index = *std::ranges::min_element(vertex_indices, [&](const size_t index_0, const size_t index_1) {
                    const Vector3<Interval> projected_vertex_0 = (vertices_[index_0] - face_interior).unit();
                    const Vector3<Interval> projected_vertex_1 = (vertices_[index_1] - face_interior).unit();
                    const bool angle_0_pos = face_normals_[normal_index].dot(projected_last_vertex.cross(projected_vertex_0)).pos();
                    const bool angle_1_pos = face_normals_[normal_index].dot(projected_last_vertex.cross(projected_vertex_1)).pos();
                    if(angle_0_pos != angle_1_pos) {
                        return angle_0_pos;
                    }
                    const Interval dot_0 = projected_last_vertex.dot(projected_vertex_0);
                    const Interval dot_1 = projected_last_vertex.dot(projected_vertex_1);
                    return angle_0_pos ? dot_0 > dot_1 : dot_0 < dot_1;
                });
                face.push_back(next_index);
                vertex_indices.erase(next_index);
            }
            faces_.push_back(face);
        }

        std::map<size_t, size_t> face_sizes;
        for(const auto& face: faces_) {
            const size_t size = face.size();
            if(face_sizes.contains(size)) {
                face_sizes[size]++;
            } else {
                face_sizes[size] = 1;
            }
        }
        std::vector<std::string> face_size_strings;
        for(const auto& [size, count]: face_sizes) {
            face_size_strings.emplace_back("\t" + std::to_string(count) + " x " + std::to_string(size) + "-sided polygons");
        }
        const std::string face_sizes_string = boost::algorithm::join(face_size_strings, "\n");

        std::cout << "Found " << face_normals_.size() << " faces" << std::endl;
        std::cout << face_sizes_string << std::endl;
    }

    void setup_outlines() {
        outlines_.clear();

        for(size_t index_0 = 0; index_0 < face_normals_.size(); ++index_0) {
            for(size_t index_1 = 0; index_1 < face_normals_.size(); ++index_1) {
                if(index_0 == index_1) {
                    continue;
                }
                const Vector3<Interval> normal_0 = face_normals_[index_0];
                const Vector3<Interval> normal_1 = face_normals_[index_1];
                if(!normal_1.diff(-normal_0)) {
                    continue;
                }
                const Vector3<Interval> cross_product = normal_0.cross(normal_1).unit();
                const Vector3<Interval> bisector = (normal_0 + normal_1).unit();

                std::optional<size_t> closest_index;
                for(size_t index = 0; index < face_normals_.size(); ++index) {
                    const Interval dot = cross_product.dot(face_normals_[index]);
                    if(!dot.pos()) {
                        continue;
                    }
                    if(!closest_index.has_value() || dot < cross_product.dot(face_normals_[closest_index.value()])) {
                        closest_index = index;
                    }
                }
                const Interval epsilon = cross_product.dot(face_normals_[closest_index.value()]) / Interval(100);
                const Vector3<Interval> direction = (cross_product + bisector * epsilon).unit();

                Bitset normal_mask = get_normal_mask(direction);
                if(normal_mask.none()) {
                    continue;
                }
                if(normal_mask.count() != face_normals_.size() / 2) {
                    continue;
                }
                if(std::ranges::any_of(outlines_, [&](const Outline& outline) {
                    return outline.normal_mask == normal_mask;
                })) {
                    continue;
                }

                std::set<size_t> vertex_indices;
                for(size_t face_index = 0; face_index < faces_.size(); ++face_index) {
                    if(!normal_mask.test(face_index)) {
                        continue;
                    }
                    for(const size_t vertex_index: faces_[face_index]) {
                        for(size_t other_face_index = 0; other_face_index < faces_.size(); ++other_face_index) {
                            if(!normal_mask.test(other_face_index) && std::ranges::find(faces_[other_face_index], vertex_index) != faces_[other_face_index].end()) {
                                vertex_indices.insert(vertex_index);
                                break;
                            }
                        }
                    }
                }

                std::vector<size_t> outline;
                const size_t first_index = *std::ranges::min_element(vertex_indices);
                outline.push_back(first_index);
                vertex_indices.erase(first_index);

                while(!vertex_indices.empty()) {
                    const size_t last_index = outline.back();
                    const Vector3<Interval> projected_last_vertex = (vertices_[last_index] - direction * vertices_[last_index].dot(direction)).unit();
                    const size_t next_index = *std::ranges::min_element(vertex_indices, [&](const size_t vertex_index_0, const size_t vertex_index_1) {
                        const Vector3<Interval> projected_vertex_0 = (vertices_[vertex_index_0] - direction * vertices_[vertex_index_0].dot(direction)).unit();
                        const Vector3<Interval> projected_vertex_1 = (vertices_[vertex_index_1] - direction * vertices_[vertex_index_1].dot(direction)).unit();
                        const bool angle_0_pos = direction.dot(projected_last_vertex.cross(projected_vertex_0)).pos();
                        const bool angle_1_pos = direction.dot(projected_last_vertex.cross(projected_vertex_1)).pos();
                        if(angle_0_pos != angle_1_pos) {
                            return angle_0_pos;
                        }
                        const Interval dot_0 = projected_last_vertex.dot(projected_vertex_0);
                        const Interval dot_1 = projected_last_vertex.dot(projected_vertex_1);
                        return angle_0_pos ? dot_0 > dot_1 : dot_0 < dot_1;
                    });
                    outline.push_back(next_index);
                    vertex_indices.erase(next_index);
                }

                outlines_.push_back(Outline{normal_mask, outline});
            }
        }

        std::map<size_t, size_t> outline_sizes;
        for(const auto& [normal_mask, vertex_indices]: outlines_) {
            const size_t size = vertex_indices.size();
            if(outline_sizes.contains(size)) {
                outline_sizes[size]++;
            } else {
                outline_sizes[size] = 1;
            }
        }
        std::vector<std::string> outline_size_strings;
        for(const auto& [size, count]: outline_sizes) {
            outline_size_strings.emplace_back("\t" + std::to_string(count) + " x " + std::to_string(size) + "-sided skew polygons");
        }
        const std::string outline_sizes_string = boost::algorithm::join(outline_size_strings, "\n");

        std::cout << "Found " << outlines_.size() << " outlines" << std::endl;
        std::cout << outline_sizes_string << std::endl;
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
        const Vector3<Interval> to = vertices_[1].diff(-from) ? vertices_[1] : vertices_[2];
        const Interval dist = from.dist(to);
        const Matrix<Interval> basis = orthonormal_basis(from, to, true);
        for(const bool right_handed: {true, false}) {
            for(const auto& from_image: vertices_) {
                for(const auto& to_image: vertices_) {
                    if((from_image.dist(to_image) - dist).nonz()) {
                        continue;
                    }
                    const Matrix<Interval> image_basis = orthonormal_basis(from_image, to_image, right_handed);
                    const Matrix<Interval> symmetry = Matrix<Interval>::relative_rotation(basis, image_basis);
                    if(std::ranges::all_of(vertices_, [&](const Vector3<Interval>& vertex) {
                        const Vector3<Interval> vertex_image = symmetry * vertex;
                        return std::ranges::any_of(vertices_, [&](const Vector3<Interval>& other_vertex) {
                            return !vertex_image.diff(other_vertex);
                        });
                    })) {
                        std::vector<Matrix<Interval>>& symmetries = right_handed ? rotations_ : reflections_;
                        if(std::ranges::all_of(symmetries, [&](const Matrix<Interval>& rotation) {
                            return Matrix<Interval>::relative_rotation(symmetry, rotation).cos_angle() < Interval(1);
                        })) {
                            symmetries.push_back(symmetry);
                        }
                    }
                }
            }
        }

        std::map<int, size_t> rotation_angles;
        for(const Matrix<Interval>& rotation: rotations_) {
            const Interval cos_angle = rotation.cos_angle();
            const Interval angle = cos_angle.pos() ? cos_angle.min().acos() : cos_angle.max().acos();
            const int rounded_angle = static_cast<int>(std::round((angle / Interval::pi() * Interval(180)).to_float()));
            if(rotation_angles.contains(rounded_angle)) {
                ++rotation_angles[rounded_angle];
            } else {
                rotation_angles[rounded_angle] = 1;
            }
        }
        std::vector<std::string> rotation_angle_strings;
        for(const auto& [angle, count]: rotation_angles) {
            rotation_angle_strings.emplace_back("\t" + std::to_string(count) + " x " + std::to_string(angle) + "-degree rotations");
        }
        const std::string rotation_angles_string = boost::algorithm::join(rotation_angle_strings, "\n");

        std::cout << "Found " << rotations_.size() << " rotations" << std::endl;
        std::cout << rotation_angles_string << std::endl;
        std::cout << "Found " << reflections_.size() << " reflections" << std::endl;
    }

    void setup_outline_symmetries() {
        outline_rotations_.clear();
        outline_reflections_.clear();

        for(const auto& [normal_mask, vertex_indices]: outlines_) {
            std::set<size_t> rotations;
            std::set<size_t> reflections;

            for(const bool right_handed: {true, false}) {
                const std::vector<Matrix<Interval>>& symmetries = right_handed ? rotations_ : reflections_;
                std::set<size_t>& outline_symmetries = right_handed ? rotations : reflections;
                for(const Matrix<Interval>& symmetry: symmetries) {
                    std::vector<size_t> transformed_vertex_indices;
                    for(const size_t vertex_index: vertex_indices) {
                        const Vector3<Interval> rotated_vertex = symmetry * vertices_[vertex_index];
                        const auto vertex_iterator = std::ranges::find_if(vertices_, [&](const Vector3<Interval>& vertex) {
                            return !rotated_vertex.diff(vertex);
                        });
                        const size_t transformed_vertex_index = std::distance(vertices_.begin(), vertex_iterator);
                        transformed_vertex_indices.push_back(transformed_vertex_index);
                    }
                    for(size_t outline_index = 0; outline_index < outlines_.size(); outline_index++) {
                        if(outlines_[outline_index].same_vertex_indices(transformed_vertex_indices)) {
                            outline_symmetries.insert(outline_index);
                        }
                    }
                }
            }

            outline_rotations_.push_back(std::vector<size_t>(rotations.begin(), rotations.end()));
            outline_reflections_.push_back(std::vector<size_t>(reflections.begin(), reflections.end()));
        }

        std::cout << "Found all outline rotations" << std::endl;
        std::cout << "Found all outline reflections" << std::endl;
    }

    void setup() {
        check_centrally_symmetric();
        setup_faces();
        setup_outlines();
        setup_symmetries();
        setup_outline_symmetries();
    }

public:
    explicit Polyhedron(const std::vector<Vector3<Interval>>& vertices) : vertices_(vertices) {
        setup();
    }

    const std::vector<Vector3<Interval>>& vertices() const {
        return vertices_;
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

    const std::vector<Outline>& outlines() const {
        return outlines_;
    }

    Bitset get_normal_mask(const Vector3<Interval>& direction) const {
        Bitset normal_mask(face_normals_.size());
        for(size_t index = 0; index < face_normals_.size(); ++index) {
            const Interval dot = direction.dot(face_normals_[index]);
            if(dot.pos()) {
                normal_mask.set(index);
            } else if(dot.neg()) {
                normal_mask.reset(index);
            } else {
                normal_mask.reset();
                return normal_mask;
            }
        }
        return normal_mask;
    }
};
