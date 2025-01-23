#pragma once

#include "vector.hpp"
#include <vector>

template<IntervalType I>
class Polyhedron {
    static std::vector<Vector3<I>> normalised(const std::vector<Vector3<I>> &vertices) {
        std::vector<Vector3<I>> normalised_vertices;
        for(const Vector3<I> &vertex: vertices) {
            normalised_vertices.push_back(vertex / vertex.mag());
        }
        return normalised_vertices;
    }

    static std::vector<Vector3<I>> permutations(const std::vector<Vector3<I>> &vertices) {
        std::vector<Vector3<I>> permutations;
        for(const auto &[x, y, z]: vertices) {
            permutations.push_back(Vector3<I>(x, y, z));
            permutations.push_back(Vector3<I>(z, x, y));
            permutations.push_back(Vector3<I>(y, z, x));
            permutations.push_back(Vector3<I>(x, z, y));
            permutations.push_back(Vector3<I>(y, x, z));
            permutations.push_back(Vector3<I>(z, y, x));
        }
        return permutations;
    }

    static std::vector<Vector3<I>> rotations(const std::vector<Vector3<I>> &vertices) {
        std::vector<Vector3<I>> rotations;
        for(const auto &[x, y, z]: vertices) {
            rotations.push_back(Vector3<I>(x, y, z));
            rotations.push_back(Vector3<I>(z, x, y));
            rotations.push_back(Vector3<I>(y, z, x));
        }
        return rotations;
    }

    static std::vector<Vector3<I>> flips(const std::vector<Vector3<I>> &vertices, const bool flip_x, const bool flip_y, const bool flip_z) {
        std::vector<Vector3<I>> flips;
        for(const double sign_x: flip_x ? std::vector<double>{1, -1} : std::vector<double>{1}) {
            for(const double sign_y: flip_y ? std::vector<double>{1, -1} : std::vector<double>{1}) {
                for(const double sign_z: flip_z ? std::vector<double>{1, -1} : std::vector<double>{1}) {
                    for(const auto &[x, y, z]: vertices) {
                        flips.push_back(Vector3<I>(x * sign_x, y * sign_y, z * sign_z));
                    }
                }
            }
        }
        return flips;
    }

    static std::vector<Vector3<I>> all_flips(const Vector3<I> &vertex) {
        const bool x_zero = vertex.x.min() == 0 && vertex.x.max() == 0;
        const bool y_zero = vertex.y.min() == 0 && vertex.y.max() == 0;
        const bool z_zero = vertex.z.min() == 0 && vertex.z.max() == 0;
        return flips({vertex}, !x_zero, !y_zero, !z_zero);
    }

    static std::vector<Vector3<I>> combine(const std::vector<std::vector<Vector3<I>>> &vertices) {
        std::vector<Vector3<I>> combine;
        for(const std::vector<Vector3<I>> &vertex: vertices) {
            combine.insert(combine.end(), vertex.begin(), vertex.end());
        }
        return combine;
    }

    inline static const I phi = 1 + I(5).sqrt() / 2;
    inline static const I sqrt2 = I(2).sqrt();

public:
    static std::vector<Vector3<I>> cube() {
        return all_flips(Vector3<I>(1, 1, 1));
    }

    static std::vector<Vector3<I>> octahedron() {
        return rotations(all_flips(Vector3<I>(0, 0, 1)));
    }

    static std::vector<Vector3<I>> dodecahedron() {
        return combine({
                all_flips(Vector3<I>(1, 1, 1)),
                rotations(all_flips(Vector3<I>(0, 1 / phi, phi)))
            });
    }

    static std::vector<Vector3<I>> icosahedron() {
        return rotations(all_flips(Vector3<I>(0, phi, 1)));
    }

    // static std::vector<Vector3<I>> cuboctahedron() {
    //     return rotations(all_flips(Vector3<I>(0, 1, 1)));
    // }
    //
    // static std::vector<Vector3<I>> truncated_cube() {
    //     return rotations(all_flips(Vector3<I>(1, 1, sqrt2 - 1)));
    // }
    //
    // static std::vector<Vector3<I>> truncated_octahedron() {
    //     return permutations(all_flips(Vector3<I>(0,1,2)));
    // }
    //
    // static std::vector<Vector3<I>> rhombicuboctahedron() {
    //     return rotations(all_flips(Vector3<I>(1, 1, sqrt2 + 1)));
    // }
    //
    // static std::vector<Vector3<I>> truncated_cuboctahedron() {
    //     return rotations(all_flips(Vector3<I>(1, sqrt2 + 1, 2 * sqrt2 + 1)));
    // }
    //
    // static std::vector<Vector3<I>> icosidodecahedron() {
    //     return combine({
    //             rotations(all_flips(Vector3<I>(2 * phi, 0, 0))),
    //             rotations(all_flips(Vector3<I>(phi * phi, phi, 1)))
    //         });
    // }
    //
    // static std::vector<Vector3<I>> truncated_dodecahedron() {
    //     return combine({
    //             rotations(all_flips(Vector3<I>(phi + 2, 1 / phi, 0))),
    //             rotations(all_flips(Vector3<I>(2 * phi, phi, 1 / phi))),
    //             rotations(all_flips(Vector3<I>(phi + 1, 2, phi)))
    //         });
    // }
    //
    // // ...

    static std::vector<Vector3<I>> rhombicosidodecahedron() {
        return rotations(combine({
                all_flips(Vector3<I>(I(1), I(1), phi * phi * phi)),
                all_flips(Vector3<I>(phi * phi, phi, 2 * phi)),
                all_flips(Vector3<I>(phi + 2, I(0), phi * phi))
            }));
    }
};
