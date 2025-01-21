#pragma once

#include "vector.hpp"
#include <vector>

template<IntervalType T>
class Polyhedron {
    using Vector = Vector3<T>;

    static std::vector<Vector> normalised(const std::vector<Vector> &vertices) {
        std::vector<Vector> normalised_vertices;
        for(const Vector &vertex: vertices) {
            normalised_vertices.push_back(vertex / vertex.mag());
        }
        return normalised_vertices;
    }

    static std::vector<Vector> permutations(const std::vector<Vector> &vertices) {
        std::vector<Vector> permutations;
        for(const auto &[x, y, z]: vertices) {
            permutations.push_back(Vector3(x, y, z));
            permutations.push_back(Vector3(z, x, y));
            permutations.push_back(Vector3(y, z, x));
            permutations.push_back(Vector3(x, z, y));
            permutations.push_back(Vector3(y, x, z));
            permutations.push_back(Vector3(z, y, x));
        }
        return permutations;
    }

    static std::vector<Vector> rotations(const std::vector<Vector> &vertices) {
        std::vector<Vector> rotations;
        for(const auto &[x, y, z]: vertices) {
            rotations.push_back(Vector(x, y, z));
            rotations.push_back(Vector(z, x, y));
            rotations.push_back(Vector(y, z, x));
        }
        return rotations;
    }

    static std::vector<Vector> flips(const std::vector<Vector> &vertices, const bool flip_x, const bool flip_y, const bool flip_z) {
        std::vector<Vector> flips;
        for(const double sign_x: flip_x ? std::vector<double>{1, -1} : std::vector<double>{1}) {
            for(const double sign_y: flip_y ? std::vector<double>{1, -1} : std::vector<double>{1}) {
                for(const double sign_z: flip_z ? std::vector<double>{1, -1} : std::vector<double>{1}) {
                    for(const auto &[x, y, z]: vertices) {
                        flips.push_back(Vector(x * sign_x, y * sign_y, z * sign_z));
                    }
                }
            }
        }
        return flips;
    }

    static std::vector<Vector> combine(const std::vector<std::vector<Vector>> &vertices) {
        std::vector<Vector> combine;
        for(const std::vector<Vector> &vertex: vertices) {
            combine.insert(combine.end(), vertex.begin(), vertex.end());
        }
        return combine;
    }

public:
    static std::vector<Vector> octahedron() {
        return normalised(rotations(flips({Vector(1, 0, 0)}, true, false, false)));
    }

    static std::vector<Vector> cube() {
        return normalised(flips({Vector(1, 1, 1)}, true, true, true));
    }

    static std::vector<Vector> icosahedron() {
        const T phi = 1 + T(5).sqrt() / 2;
        return normalised(rotations(flips({Vector(1, phi, 0)}, true, true, false)));
    }

    static std::vector<Vector> dodecahedron() {
        const T phi = 1 + T(5).sqrt() / 2;
        const T phi_reciprocal = 2 / (1 + T(5).sqrt());
        return normalised(combine({
                flips({Vector(1, 1, 1)}, true, true, true),
                rotations(flips({Vector(phi, phi_reciprocal, 0)}, true, true, false))
            }));
    }

    static std::vector<Vector> rhombicosidodecahedron() {
        const T phi = 1 + T(5).sqrt() / 2;
        const T phi_square = 3 + T(5).sqrt() / 2;
        const T phi_cube = 2 + T(5).sqrt();
        const T phi_double = 1 + 2 * T(5).sqrt();
        const T phi_plus_two = 5 + T(5).sqrt() / 2;
        return normalised(rotations(combine({
                flips({Vector(T(1), T(1), phi_cube)}, true, true, true),
                flips({Vector(phi_square, phi, phi_double)}, true, true, true),
                flips({Vector(phi_plus_two, T(0), phi_square)}, true, false, true),
            })));
    }
};
