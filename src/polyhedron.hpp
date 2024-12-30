#pragma once

#include "base.hpp"

struct Polyhedron {
    static std::vector<Vector3> normalised(const std::vector<Vector3> &vertices) {
        std::vector<Vector3> normalised_vertices;
        for(const Vector3 &vertex: vertices) {
            normalised_vertices.push_back(vertex.normalized());
        }
        return normalised_vertices;
    }

    static std::vector<Vector3> permutations(const std::vector<Vector3> &vertices) {
        std::vector<Vector3> permutations;
        for(const Vector3 &vertex: vertices) {
            permutations.push_back({vertex.x, vertex.y, vertex.z});
            permutations.push_back({vertex.z, vertex.x, vertex.y});
            permutations.push_back({vertex.y, vertex.z, vertex.x});
            permutations.push_back({vertex.x, vertex.z, vertex.y});
            permutations.push_back({vertex.y, vertex.x, vertex.z});
            permutations.push_back({vertex.z, vertex.y, vertex.x});
        }
        return permutations;
    }

    static std::vector<Vector3> rotations(const std::vector<Vector3> &vertices) {
        std::vector<Vector3> rotations;
        for(const Vector3 &vertex: vertices) {
            rotations.push_back({vertex.x, vertex.y, vertex.z});
            rotations.push_back({vertex.z, vertex.x, vertex.y});
            rotations.push_back({vertex.y, vertex.z, vertex.x});
        }
        return rotations;
    }

    static std::vector<Vector3> flips(const std::vector<Vector3> &vertices, bool flip_x, bool flip_y, bool flip_z) {
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
        std::vector<Vector3> flips;
        for(const double sign_x: signs_x) {
            for(const double sign_y: signs_y) {
                for(const double sign_z: signs_z) {
                    Vector3 signs(sign_x, sign_y, sign_z);
                    for(const Vector3 &vertex: vertices) {
                        flips.push_back(vertex * signs);
                    }
                }
            }
        }
        return flips;
    }

    static std::vector<Vector3> combine(const std::vector<std::vector<Vector3>> &vertices) {
        std::vector<Vector3> combine;
        for(const std::vector<Vector3> &vertex: vertices) {
            combine.insert(combine.end(), vertex.begin(), vertex.end());
        }
        return combine;
    }

    static std::vector<Vector3> octahedron() {
        return normalised(rotations(flips({{1, 0, 0}}, true, false, false)));
    }

    static std::vector<Vector3> cube() {
        return normalised(flips({{1, 1, 1}}, true, true, true));
    }

    static std::vector<Vector3> icosahedron() {
        const double phi = (1 + std::sqrt(5)) / 2;
        return normalised(rotations(flips({{1, phi, 0}}, true, true, false)));
    }

    static std::vector<Vector3> dodecahedron() {
        const double phi = (1 + std::sqrt(5)) / 2;
        const double phi_reciprocal = 2 / (1 + std::sqrt(5));
        return normalised(combine({
                flips({{1, 1, 1}}, true, true, true),
                rotations(flips({{phi, phi_reciprocal, 0}}, true, true, false))
            }));
    }

    static std::vector<Vector3> rhombicosidodecahedron() {
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
