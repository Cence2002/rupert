#pragma once

#include "geometry/vector_3.hpp"
#include <vector>

template<IntervalType I>
struct Polyhedron {
private:
    static std::vector<Vector3<I>> permutations(const std::vector<Vector3<I>>& vertices) {
        std::vector<Vector3<I>> permutations;
        for(const auto& [x, y, z]: vertices) {
            permutations.push_back(Vector3<I>(x, y, z));
            permutations.push_back(Vector3<I>(z, x, y));
            permutations.push_back(Vector3<I>(y, z, x));
            permutations.push_back(Vector3<I>(x, z, y));
            permutations.push_back(Vector3<I>(y, x, z));
            permutations.push_back(Vector3<I>(z, y, x));
        }
        return permutations;
    }

    static std::vector<Vector3<I>> rotations(const std::vector<Vector3<I>>& vertices) {
        std::vector<Vector3<I>> rotations;
        for(const Vector3<I>& vertex: vertices) {
            rotations.emplace_back(vertex.x(), vertex.y(), vertex.z());
            rotations.emplace_back(vertex.z(), vertex.x(), vertex.y());
            rotations.emplace_back(vertex.y(), vertex.z(), vertex.x());
        }
        return rotations;
    }

    static std::vector<Vector3<I>> flips(const std::vector<Vector3<I>>& vertices, const bool flip_x, const bool flip_y, const bool flip_z) {
        std::vector<Vector3<I>> flips;
        for(const int sign_x: flip_x ? std::vector<int>{1, -1} : std::vector<int>{1}) {
            for(const int sign_y: flip_y ? std::vector<int>{1, -1} : std::vector<int>{1}) {
                for(const int sign_z: flip_z ? std::vector<int>{1, -1} : std::vector<int>{1}) {
                    for(const Vector3<I>& vertex: vertices) {
                        flips.emplace_back(vertex.x() * sign_x, vertex.y() * sign_y, vertex.z() * sign_z);
                    }
                }
            }
        }
        return flips;
    }

    static std::vector<Vector3<I>> all_flips(const Vector3<I>& vertex) {
        const bool x_zero = !vertex.x().pos() && !vertex.x().neg();
        const bool y_zero = !vertex.y().pos() && !vertex.y().neg();
        const bool z_zero = !vertex.z().pos() && !vertex.z().neg();
        return flips({vertex}, !x_zero, !y_zero, !z_zero);
    }

    static std::vector<Vector3<I>> combine(const std::vector<std::vector<Vector3<I>>>& vertices) {
        std::vector<Vector3<I>> combine;
        for(const std::vector<Vector3<I>>& vertex: vertices) {
            combine.insert(combine.end(), vertex.begin(), vertex.end());
        }
        return combine;
    }

    static inline const I phi = 1 + I(5).sqrt() / 2;
    static inline const I sqrt2 = I(2).sqrt();

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

    /*
    static std::vector<Vector3<I>> cuboctahedron() {
        return rotations(all_flips(Vector3<I>(0, 1, 1)));
    }

    static std::vector<Vector3<I>> truncated_cube() {
        return rotations(all_flips(Vector3<I>(1, 1, sqrt2 - 1)));
    }

    static std::vector<Vector3<I>> truncated_octahedron() {
        return permutations(all_flips(Vector3<I>(0,1,2)));
    }

    static std::vector<Vector3<I>> rhombicuboctahedron() {
        return rotations(all_flips(Vector3<I>(1, 1, sqrt2 + 1)));
    }

    static std::vector<Vector3<I>> truncated_cuboctahedron() {
        return rotations(all_flips(Vector3<I>(1, sqrt2 + 1, 2 * sqrt2 + 1)));
    }

    static std::vector<Vector3<I>> icosidodecahedron() {
        return combine({
                rotations(all_flips(Vector3<I>(2 * phi, 0, 0))),
                rotations(all_flips(Vector3<I>(phi * phi, phi, 1)))
            });
    }

    static std::vector<Vector3<I>> truncated_dodecahedron() {
        return combine({
                rotations(all_flips(Vector3<I>(phi + 2, 1 / phi, 0))),
                rotations(all_flips(Vector3<I>(2 * phi, phi, 1 / phi))),
                rotations(all_flips(Vector3<I>(phi + 1, 2, phi)))
            });
    }
    */

    static std::vector<Vector3<I>> rhombicosidodecahedron() {
        return rotations(combine({
                all_flips(Vector3<I>(I(1), I(1), phi * phi * phi)),
                all_flips(Vector3<I>(phi * phi, phi, 2 * phi)),
                all_flips(Vector3<I>(phi + 2, I(0), phi * phi))
            }));
    }
};
