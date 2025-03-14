#pragma once

#include "geometry/vertex.hpp"
#include <vector>

template<IntervalType Interval>
struct Polyhedron {
private:
    std::vector<Vertex<Interval>> vertices_;

    static inline const Interval phi = (1 + Interval(5).sqrt()) / 2;
    static inline const Interval sqrt2 = Interval(2).sqrt();

public:
    explicit Polyhedron(const std::vector<Vertex<Interval>>& vertices) : vertices_(vertices) {}

    const std::vector<Vertex<Interval>>& vertices() const {
        return vertices_;
    }

    static std::vector<Vertex<Interval>> permutations(const std::vector<Vertex<Interval>>& vertices) {
        std::vector<Vertex<Interval>> permutations;
        for(const Vertex<Interval>& vertex: vertices) {
            permutations.emplace_back(vertex.x(), vertex.y(), vertex.z());
            permutations.emplace_back(vertex.z(), vertex.x(), vertex.y());
            permutations.emplace_back(vertex.y(), vertex.z(), vertex.x());
            permutations.emplace_back(vertex.z(), vertex.y(), vertex.x());
            permutations.emplace_back(vertex.x(), vertex.z(), vertex.y());
            permutations.emplace_back(vertex.y(), vertex.x(), vertex.z());
        }
        return permutations;
    }

    static std::vector<Vertex<Interval>> rotations(const std::vector<Vertex<Interval>>& vertices) {
        std::vector<Vertex<Interval>> rotations;
        for(const Vertex<Interval>& vertex: vertices) {
            rotations.emplace_back(vertex.x(), vertex.y(), vertex.z());
            rotations.emplace_back(vertex.z(), vertex.x(), vertex.y());
            rotations.emplace_back(vertex.y(), vertex.z(), vertex.x());
        }
        return rotations;
    }

    static std::vector<Vertex<Interval>> flips(const Vertex<Interval>& vertex, const bool flip_x, const bool flip_y, const bool flip_z) {
        std::vector<Vertex<Interval>> flips;
        for(const bool sign_x: flip_x ? std::vector<bool>{false, true} : std::vector<bool>{false}) {
            for(const bool sign_y: flip_y ? std::vector<bool>{false, true} : std::vector<bool>{false}) {
                for(const bool sign_z: flip_z ? std::vector<bool>{false, true} : std::vector<bool>{false}) {
                    flips.emplace_back(
                        sign_x ? -vertex.x() : vertex.x(),
                        sign_y ? -vertex.y() : vertex.y(),
                        sign_z ? -vertex.z() : vertex.z()
                    );
                }
            }
        }
        return flips;
    }

    static std::vector<Vertex<Interval>> flips(const Vertex<Interval>& vertex) {
        return flips({vertex}, vertex.x().is_nonzero(), vertex.y().is_nonzero(), vertex.z().is_nonzero());
    }

    static std::vector<Vertex<Interval>> combined(const std::vector<std::vector<Vertex<Interval>>>& vertices) {
        std::vector<Vertex<Interval>> combined;
        for(const std::vector<Vertex<Interval>>& vertex: vertices) {
            combined.insert(combined.end(), vertex.begin(), vertex.end());
        }
        return combined;
    }

    Polyhedron scale(const Interval& scale) const {
        std::vector<Vertex<Interval>> scaled_vertices;
        for(const Vertex<Interval>& vertex: vertices_) {
            scaled_vertices.push_back(vertex * scale);
        }
        return Polyhedron(scaled_vertices);
    }

    static Polyhedron octahedron() {
        return Polyhedron(rotations(flips(Vertex<Interval>(0, 0, 1))));
    }

    static Polyhedron cube() {
        return Polyhedron(flips(Vertex<Interval>(1, 1, 1)));
    }

    static Polyhedron dodecahedron() {
        const Interval c0 = (1 + Interval(5).sqrt()) / 4;
        const Interval c1 = (3 + Interval(5).sqrt()) / 4;
        return Polyhedron(combined({
            rotations(flips(Vertex<Interval>(Interval(0), Interval(1) / 2, c1))),
            flips(Vertex<Interval>(c0, c0, c0))
        }));
    }

    static Polyhedron icosahedron() {
        return Polyhedron(rotations(flips(Vertex<Interval>(0, phi, 1))));
    }

    static Polyhedron rhombicosidodecahedron() {
        return Polyhedron(rotations(combined({
            flips(Vertex<Interval>(1, 1, phi * phi * phi)),
            flips(Vertex<Interval>(phi * phi, phi, 2 * phi)),
            flips(Vertex<Interval>(2 + phi, 0, phi * phi))
        })));
    }
};

namespace Polyhedra {
    template<IntervalType I>
    static Polyhedron<I> icosahedron() {
        const I c0 = (I(1) + I(5).sqrt()) / 4;
        return Polyhedron<I>(Polyhedron<I>::rotations(Polyhedron<I>::flips(Vertex<I>(I(1) / 2, I(0), c0))));
    }

    template<IntervalType I>
    static Polyhedron<I> dodecahedron() {
        const I c0 = (1 + I(5).sqrt()) / 4;
        const I c1 = (3 + I(5).sqrt()) / 4;
        return Polyhedron(Polyhedron<I>::combined({
            Polyhedron<I>::rotations(Polyhedron<I>::flips(Vertex<I>(I(0), I(1) / 2, c1))),
            Polyhedron<I>::flips(Vertex<I>(c0, c0, c0))
        }));
    }

    template<IntervalType I>
    static Polyhedron<I> rhombicosidodecahedron() {
        const I c0 = (I(1) + I(5).sqrt()) / 4;
        const I c1 = (I(3) + I(5).sqrt()) / 4;
        const I c2 = (I(1) + I(5).sqrt()) / 2;
        const I c3 = (I(5) + I(5).sqrt()) / 4;
        const I c4 = (I(2) + I(5).sqrt()) / 2;
        return Polyhedron<I>(Polyhedron<I>::combined({
            Polyhedron<I>::rotations(Polyhedron<I>::flips(Vertex<I>(I(1) / 2, I(1) / 2, c4))),
            Polyhedron<I>::rotations(Polyhedron<I>::flips(Vertex<I>(I(0), c1, c3))),
            Polyhedron<I>::rotations(Polyhedron<I>::flips(Vertex<I>(c1, c0, c2)))
        }));
    }
}
