#pragma once

#include "vector.hpp"

struct Polyhedron {
    static std::vector<Vector3d> normalised(const std::vector<Vector3d> &vertices) {
        std::vector<Vector3d> normalised_vertices;
        for(const Vector3d &vertex: vertices) {
            normalised_vertices.push_back(vertex.unit());
        }
        return normalised_vertices;
    }

    static std::vector<Vector3d> permutations(const std::vector<Vector3d> &vertices) {
        std::vector<Vector3d> permutations;
        for(const auto &[x, y, z]: vertices) {
            permutations.push_back({x, y, z});
            permutations.push_back({z, x, y});
            permutations.push_back({y, z, x});
            permutations.push_back({x, z, y});
            permutations.push_back({y, x, z});
            permutations.push_back({z, y, x});
        }
        return permutations;
    }

    static std::vector<Vector3d> rotations(const std::vector<Vector3d> &vertices) {
        std::vector<Vector3d> rotations;
        for(const auto &[x, y, z]: vertices) {
            rotations.push_back({x, y, z});
            rotations.push_back({z, x, y});
            rotations.push_back({y, z, x});
        }
        return rotations;
    }

    static std::vector<Vector3d> flips(const std::vector<Vector3d> &vertices, bool flip_x, bool flip_y, bool flip_z) {
        std::vector<double> signs_x{1};
        if(flip_x) {
            signs_x.push_back(-1);
        }
        std::vector<double> signs_y{1};
        if(flip_y) {
            signs_y.push_back(-1);
        }
        std::vector<double> signs_z{1};
        if(flip_z) {
            signs_z.push_back(-1);
        }
        std::vector<Vector3d> flips;
        for(const double sign_x: signs_x) {
            for(const double sign_y: signs_y) {
                for(const double sign_z: signs_z) {
                    Vector3d signs(sign_x, sign_y, sign_z);
                    for(const Vector3d &vertex: vertices) {
                        flips.push_back(vertex * signs);
                    }
                }
            }
        }
        return flips;
    }

    static std::vector<Vector3d> combine(const std::vector<std::vector<Vector3d>> &vertices) {
        std::vector<Vector3d> combine;
        for(const std::vector<Vector3d> &vertex: vertices) {
            combine.insert(combine.end(), vertex.begin(), vertex.end());
        }
        return combine;
    }

    static std::vector<Vector3d> octahedron() {
        return normalised(rotations(flips({{1, 0, 0}}, true, false, false)));
    }

    static std::vector<Vector3d> cube() {
        return normalised(flips({{1, 1, 1}}, true, true, true));
    }

    static std::vector<Vector3d> icosahedron() {
        const double phi = (1 + std::sqrt(5)) / 2;
        return normalised(rotations(flips({{1, phi, 0}}, true, true, false)));
    }

    static std::vector<Vector3d> dodecahedron() {
        const double phi = (1 + std::sqrt(5)) / 2;
        const double phi_reciprocal = 2 / (1 + std::sqrt(5));
        return normalised(combine({
                flips({{1, 1, 1}}, true, true, true),
                rotations(flips({{phi, phi_reciprocal, 0}}, true, true, false))
            }));
    }

    static std::vector<Vector3d> rhombicosidodecahedron() {
        const double phi = (1 + std::sqrt(5)) / 2;
        const double phi_square = (3 + std::sqrt(5)) / 2;
        const double phi_cube = 2 + std::sqrt(5);
        const double phi_double = 1 + 2 * std::sqrt(5);
        const double phi_plus_two = (5 + std::sqrt(5)) / 2;
        return normalised(rotations(combine({
                flips({{1, 1, phi_cube}}, true, true, true),
                flips({{phi_square, phi, phi_double}}, true, true, true),
                flips({{phi_plus_two, 0, phi_square}}, true, false, true),
            })));
    }
};
