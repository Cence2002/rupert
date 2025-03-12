#pragma once

#include "geometry/vertex.hpp"
#include <vector>

template<IntervalType Interval>
struct Polyhedron {
private:
    std::vector<Vertex<Interval>> vertices_;

public:
    explicit Polyhedron(const std::vector<Vertex<Interval>>& vertices) : vertices_(vertices) {}

    const std::vector<Vertex<Interval>>& vertices() const {
        return vertices_;
    }

private:
    static std::vector<Vertex<Interval>> permutations(const std::vector<Vertex<Interval>>& vertices) {
        std::vector<Vertex<Interval>> permutations;
        for(const Vertex<Interval>& vertex: vertices) {
            permutations.emplace_back(vertex.x(), vertex.y(), vertex.z());
            permutations.emplace_back(vertex.y(), vertex.z(), vertex.x());
            permutations.emplace_back(vertex.z(), vertex.x(), vertex.y());
            permutations.emplace_back(vertex.x(), vertex.z(), vertex.y());
            permutations.emplace_back(vertex.z(), vertex.y(), vertex.x());
            permutations.emplace_back(vertex.y(), vertex.x(), vertex.z());
        }
        return permutations;
    }

    static std::vector<Vertex<Interval>> rotations(const std::vector<Vertex<Interval>>& vertices) {
        std::vector<Vertex<Interval>> rotations;
        for(const Vertex<Interval>& vertex: vertices) {
            rotations.emplace_back(vertex.x(), vertex.y(), vertex.z());
            rotations.emplace_back(vertex.y(), vertex.z(), vertex.x());
            rotations.emplace_back(vertex.z(), vertex.x(), vertex.y());
        }
        return rotations;
    }

    static std::vector<Vertex<Interval>> flips(const std::vector<Vertex<Interval>>& vertices, const bool flip_x, const bool flip_y, const bool flip_z) {
        std::vector<Vertex<Interval>> flips;
        for(const bool sign_x: flip_x ? std::vector<bool>{false, true} : std::vector<bool>{false}) {
            for(const bool sign_y: flip_y ? std::vector<bool>{false, true} : std::vector<bool>{false}) {
                for(const bool sign_z: flip_z ? std::vector<bool>{false, true} : std::vector<bool>{false}) {
                    for(const Vertex<Interval>& vertex: vertices) {
                        flips.emplace_back(
                            sign_x ? -vertex.x() : vertex.x(),
                            sign_y ? -vertex.y() : vertex.y(),
                            sign_z ? -vertex.z() : vertex.z()
                        );
                    }
                }
            }
        }
        return flips;
    }

    static std::vector<Vertex<Interval>> flips(const Vertex<Interval>& vertex, const bool flip_x, const bool flip_y, const bool flip_z) {
        return flips(std::vector<Vertex<Interval>>{vertex}, flip_x, flip_y, flip_z);
    }

    static std::vector<Vertex<Interval>> combine(const std::vector<std::vector<Vertex<Interval>>>& vertices) {
        std::vector<Vertex<Interval>> combine;
        for(const std::vector<Vertex<Interval>>& vertex: vertices) {
            combine.insert(combine.end(), vertex.begin(), vertex.end());
        }
        return combine;
    }

    static inline const Interval phi = (1 + Interval(5).sqrt()) / 2;
    static inline const Interval sqrt2 = Interval(2).sqrt();

public:
    static Polyhedron cube() {
        return Polyhedron(flips(Vertex<Interval>(Interval(1), Interval(1), Interval(1)), true, true, true));
    }

    static Polyhedron octahedron() {
        return Polyhedron(rotations(flips(Vertex<Interval>(Interval(0), Interval(0), Interval(1)), false, false, true)));
    }

    static Polyhedron dodecahedron() {
        return Polyhedron(combine({
            flips(Vertex<Interval>(1, 1, 1), true, true, true),
            rotations(flips(Vertex<Interval>(0, 1 / phi, phi), false, true, true))
        }));
    }

    static Polyhedron icosahedron() {
        return Polyhedron(rotations(flips(Vertex<Interval>(0, phi, 1), false, true, true)));
    }

    static Polyhedron rhombicosidodecahedron() {
        return Polyhedron(rotations(combine({
            flips(Vertex<Interval>(Interval(1), Interval(1), phi * phi * phi), true, true, true),
            flips(Vertex<Interval>(phi * phi, phi, 2 * phi), true, true, true),
            flips(Vertex<Interval>(2 + phi, Interval(0), phi * phi), true, false, true)
        })));
    }
};
