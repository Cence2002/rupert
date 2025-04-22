#pragma once

#include "geometry/vertex.hpp"
#include <symmetry.hpp>
#include <vector>

template<IntervalType Interval>
struct Polyhedron {
private:
    std::vector<Vertex<Interval>> vertices_;

    std::vector<Matrix3<Interval>> rotations_;
    std::vector<Matrix3<Interval>> reflections_;

public:
    explicit Polyhedron(const std::vector<Vertex<Interval>>& vertices) : vertices_(vertices), rotations_(), reflections_() {
        rotations_ = symmetries(vertices_, true, Interval(1) / 1000);
        reflections_ = symmetries(vertices_, false, Interval(1) / 1000);
    }

    const std::vector<Vertex<Interval>>& vertices() const {
        return vertices_;
    }

    const std::vector<Matrix3<Interval>>& rotations() const {
        return rotations_;
    }

    const std::vector<Matrix3<Interval>>& reflections() const {
        return reflections_;
    }

    Polyhedron scale(const Interval& scale) const {
        std::vector<Vertex<Interval>> scaled_vertices;
        for(const Vertex<Interval>& vertex: vertices_) {
            scaled_vertices.push_back(vertex * scale);
        }
        return Polyhedron(scaled_vertices);
    }

    Polyhedron normalise() const {
        Interval radius = Interval(0);
        for(const Vertex<Interval>& vertex: vertices_) {
            const Interval len = vertex.len();
            if(len > radius) {
                radius = len;
            }
        }
        return scale(radius.inv());
    }
};

namespace PolyhedronHelpers {
    template<IntervalType Interval>
    std::vector<Vertex<Interval>> rotations(const std::vector<Vertex<Interval>>& vertices) {
        std::vector<Vertex<Interval>> rotations;
        for(const Vertex<Interval>& vertex: vertices) {
            rotations.emplace_back(vertex.x(), vertex.y(), vertex.z());
            rotations.emplace_back(vertex.z(), vertex.x(), vertex.y());
            rotations.emplace_back(vertex.y(), vertex.z(), vertex.x());
        }
        return rotations;
    }

    template<IntervalType Interval>
    std::vector<Vertex<Interval>> permutations(const std::vector<Vertex<Interval>>& vertices) {
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

    template<IntervalType Interval>
    std::vector<Vertex<Interval>> flips(const Vertex<Interval>& vertex, const bool flip_x, const bool flip_y, const bool flip_z) {
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

    template<IntervalType Interval>
    std::vector<Vertex<Interval>> flips(const Vertex<Interval>& vertex) {
        return flips<Interval>({vertex}, vertex.x().is_nonzero(), vertex.y().is_nonzero(), vertex.z().is_nonzero());
    }

    template<IntervalType Interval>
    std::vector<Vertex<Interval>> combine(const std::vector<std::vector<Vertex<Interval>>>& vertices) {
        std::vector<Vertex<Interval>> combined;
        for(const std::vector<Vertex<Interval>>& vertex: vertices) {
            combined.insert(combined.end(), vertex.begin(), vertex.end());
        }
        return combined;
    }
}

namespace Platonic {
    using namespace PolyhedronHelpers;

    template<IntervalType Interval>
    static Polyhedron<Interval> octahedron() {
        const Interval c0 = Interval(2).sqrt() / 2;
        return Polyhedron<Interval>(permutations<Interval>(flips<Interval>(Vertex<Interval>(c0, Interval(0), Interval(0)))));
    }

    template<IntervalType Interval>
    static Polyhedron<Interval> cube() {
        return Polyhedron<Interval>(
            flips<Interval>(
                Vertex<Interval>(Interval(1) / 2, Interval(1) / 2, Interval(1) / 2)
            )
        );
    }

    template<IntervalType Interval>
    static Polyhedron<Interval> icosahedron() {
        const Interval c0 = (Interval(1) + Interval(5).sqrt()) / 4;
        return Polyhedron<Interval>(rotations<Interval>(flips<Interval>(Vertex<Interval>(Interval(1) / 2, Interval(0), c0))));
    }

    template<IntervalType Interval>
    static Polyhedron<Interval> dodecahedron() {
        const Interval c0 = (Interval(1) + Interval(5).sqrt()) / 4;
        const Interval c1 = (Interval(3) + Interval(5).sqrt()) / 4;
        return Polyhedron<Interval>(combine<Interval>({
            rotations<Interval>(flips<Interval>(Vertex<Interval>(Interval(0), Interval(1) / 2, c1))),
            flips<Interval>(Vertex<Interval>(c0, c0, c0))
        }));
    }
}

namespace Archimedean {
    using namespace PolyhedronHelpers;

    template<IntervalType Interval>
    static Polyhedron<Interval> cuboctahedron() {
        const Interval c0 = Interval(2).sqrt() / 2;
        return Polyhedron<Interval>(
            rotations<Interval>(
                flips<Interval>(Vertex<Interval>(c0, Interval(0), c0))
            )
        );
    }

    template<IntervalType Interval>
    static Polyhedron<Interval> truncated_octahedron() {
        const Interval c0 = Interval(2).sqrt() / 2;
        const Interval c1 = Interval(2).sqrt();
        return Polyhedron<Interval>(permutations<Interval>(flips<Interval>(Vertex<Interval>(c0, Interval(0), c1))));
    }

    template<IntervalType Interval>
    static Polyhedron<Interval> truncated_cube() {
        const Interval c0 = (Interval(1) + Interval(2).sqrt()) / 2;
        return Polyhedron<Interval>(rotations<Interval>(flips<Interval>(Vertex<Interval>(c0, Interval(1) / 2, c0))));
    }

    template<IntervalType Interval>
    static Polyhedron<Interval> rhombicuboctahedron() {
        const Interval c0 = (Interval(1) + Interval(2).sqrt()) / 2;
        return Polyhedron<Interval>(rotations<Interval>(flips<Interval>(Vertex<Interval>(Interval(1) / 2, Interval(1) / 2, c0))));
    }

    template<IntervalType Interval>
    static Polyhedron<Interval> icosidodecahedron() {
        const Interval c0 = (Interval(1) + Interval(5).sqrt()) / 4;
        const Interval c1 = (Interval(3) + Interval(5).sqrt()) / 4;
        const Interval c2 = (Interval(1) + Interval(5).sqrt()) / 2;
        return Polyhedron<Interval>(combine<Interval>({
            permutations<Interval>(
                flips<Interval>(Vertex<Interval>(c2, Interval(0), Interval(0)))
            ),
            rotations<Interval>(
                flips<Interval>(Vertex<Interval>(Interval(1) / 2, c0, c1))
            )
        }));
    }

    template<IntervalType Interval>
    static Polyhedron<Interval> truncated_cuboctahedron() {
        const Interval c0 = (Interval(1) + Interval(2).sqrt()) / 2;
        const Interval c1 = (Interval(1) + Interval(2) * Interval(2).sqrt()) / 2;
        return Polyhedron<Interval>(permutations<Interval>(
            flips<Interval>(Vertex<Interval>(c0, Interval(1) / 2, c1))
        ));
    }

    template<IntervalType Interval>
    static Polyhedron<Interval> truncated_icosahedron() {
        const Interval c0 = (Interval(1) + Interval(5).sqrt()) / Interval(4);
        const Interval c1 = (Interval(1) + Interval(5).sqrt()) / Interval(2);
        const Interval c2 = (Interval(5) + Interval(5).sqrt()) / Interval(4);
        const Interval c3 = (Interval(2) + Interval(5).sqrt()) / Interval(2);
        const Interval c4 = Interval(3) * (Interval(1) + Interval(5).sqrt()) / Interval(4);
        return Polyhedron<Interval>(combine<Interval>({
            rotations<Interval>(flips<Interval>(Vertex<Interval>(Interval(1) / 2, Interval(0), c4))),
            rotations<Interval>(flips<Interval>(Vertex<Interval>(Interval(1), c0, c3))),
            rotations<Interval>(flips<Interval>(Vertex<Interval>(Interval(1) / 2, c1, c2)))
        }));
    }

    template<IntervalType Interval>
    static Polyhedron<Interval> truncated_dodecahedron() {
        const Interval c0 = (Interval(3) + Interval(5).sqrt()) / 4;
        const Interval c1 = (Interval(1) + Interval(5).sqrt()) / 2;
        const Interval c2 = (Interval(2) + Interval(5).sqrt()) / 2;
        const Interval c3 = (Interval(3) + Interval(5).sqrt()) / 2;
        const Interval c4 = (Interval(5) + Interval(3) * Interval(5).sqrt()) / 4;
        return Polyhedron<Interval>(combine<Interval>({
            rotations<Interval>(flips<Interval>(Vertex<Interval>(Interval(0), Interval(1) / 2, c4))),
            rotations<Interval>(flips<Interval>(Vertex<Interval>(Interval(1) / 2, c0, c3))),
            rotations<Interval>(flips<Interval>(Vertex<Interval>(c0, c1, c2)))
        }));
    }

    template<IntervalType Interval>
    static Polyhedron<Interval> rhombicosidodecahedron() {
        const Interval c0 = (Interval(1) + Interval(5).sqrt()) / 4;
        const Interval c1 = (Interval(3) + Interval(5).sqrt()) / 4;
        const Interval c2 = (Interval(1) + Interval(5).sqrt()) / 2;
        const Interval c3 = (Interval(5) + Interval(5).sqrt()) / 4;
        const Interval c4 = (Interval(2) + Interval(5).sqrt()) / 2;
        return Polyhedron<Interval>(combine<Interval>({
            rotations<Interval>(flips<Interval>(Vertex<Interval>(Interval(1) / 2, Interval(1) / 2, c4))),
            rotations<Interval>(flips<Interval>(Vertex<Interval>(Interval(0), c1, c3))),
            rotations<Interval>(flips<Interval>(Vertex<Interval>(c1, c0, c2)))
        }));
    }

    template<IntervalType Interval>
    static Polyhedron<Interval> truncated_icosidodecahedron() {
        const Interval c0 = (Interval(3) + Interval(5).sqrt()) / 4;
        const Interval c1 = (Interval(1) + Interval(5).sqrt()) / 2;
        const Interval c2 = (Interval(5) + Interval(5).sqrt()) / 4;
        const Interval c3 = (Interval(2) + Interval(5).sqrt()) / 2;
        const Interval c4 = Interval(3) * (Interval(1) + Interval(5).sqrt()) / 4;
        const Interval c5 = (Interval(3) + Interval(5).sqrt()) / 2;
        const Interval c6 = (Interval(5) + Interval(3) * Interval(5).sqrt()) / 4;
        const Interval c7 = (Interval(4) + Interval(5).sqrt()) / 2;
        const Interval c8 = (Interval(7) + Interval(3) * Interval(5).sqrt()) / 4;
        const Interval c9 = (Interval(3) + Interval(2) * Interval(5).sqrt()) / 2;
        return Polyhedron<Interval>(combine<Interval>({
            rotations<Interval>(flips<Interval>(Vertex<Interval>(Interval(1) / 2, Interval(1) / 2, c9))),
            rotations<Interval>(flips<Interval>(Vertex<Interval>(Interval(1), c0, c8))),
            rotations<Interval>(flips<Interval>(Vertex<Interval>(Interval(1) / 2, c3, c7))),
            rotations<Interval>(flips<Interval>(Vertex<Interval>(c2, c1, c6))),
            rotations<Interval>(flips<Interval>(Vertex<Interval>(c0, c4, c5)))
        }));
    }
}

namespace Catalan {
    using namespace PolyhedronHelpers;

    template<IntervalType Interval>
    static Polyhedron<Interval> rhombic_dodecahedron() {
        const Interval c0 = (Interval(3) * Interval(2).sqrt()) / Interval(8);
        const Interval c1 = (Interval(3) * Interval(2).sqrt()) / Interval(4);
        return Polyhedron<Interval>(combine<Interval>({
            rotations<Interval>(flips<Interval>(Vertex<Interval>(Interval(0), Interval(0), c1))),
            flips<Interval>(Vertex<Interval>(c0, c0, c0))
        }));
    }

    template<IntervalType Interval>
    static Polyhedron<Interval> tetrakis_hexahedron() {
        const Interval c0 = (Interval(3) * Interval(2).sqrt()) / 4;
        const Interval c1 = (Interval(9) * Interval(2).sqrt()) / 8;
        return Polyhedron<Interval>(combine<Interval>({
            rotations<Interval>(flips<Interval>(Vertex<Interval>(Interval(0), Interval(0), c1))),
            flips<Interval>(Vertex<Interval>(c0, c0, c0))
        }));
    }

    template<IntervalType Interval>
    static Polyhedron<Interval> triakis_octahedron() {
        const Interval c0 = Interval(1) + Interval(2).sqrt();
        return Polyhedron<Interval>(combine<Interval>({
            permutations<Interval>(flips<Interval>(Vertex<Interval>(c0, Interval(0), Interval(0)))),
            flips<Interval>(Vertex<Interval>(Interval(1), Interval(1), Interval(1)))
        }));
    }

    template<IntervalType Interval>
    static Polyhedron<Interval> deltoidal_icositetrahedron() {
        const Interval c0 = (Interval(4) + Interval(2).sqrt()) / 7;
        const Interval c1 = Interval(2).sqrt();
        return Polyhedron<Interval>(combine<Interval>({
            permutations<Interval>(flips<Interval>(Vertex<Interval>(c1, Interval(0), Interval(0)))),
            permutations<Interval>(flips<Interval>(Vertex<Interval>(Interval(1), Interval(1), Interval(0)))),
            flips<Interval>(Vertex<Interval>(c0, c0, c0))
        }));
    }

    template<IntervalType Interval>
    static Polyhedron<Interval> rhombic_triacontahedron() {
        const Interval c0 = Interval(5).sqrt() / Interval(4);
        const Interval c1 = (Interval(5) + Interval(5).sqrt()) / Interval(8);
        const Interval c2 = (Interval(5) + Interval(3) * Interval(5).sqrt()) / Interval(8);
        return Polyhedron<Interval>(combine<Interval>({
            rotations<Interval>(flips<Interval>(Vertex<Interval>(c1, Interval(0), c2))),
            rotations<Interval>(flips<Interval>(Vertex<Interval>(Interval(0), c0, c2))),
            flips<Interval>(Vertex<Interval>(c1, c1, c1))
        }));
    }

    template<IntervalType Interval>
    static Polyhedron<Interval> disdyakis_dodecahedron() {
        const Interval c0 = Interval(2).sqrt();
        const Interval c1 = (Interval(3) * (Interval(1) + Interval(2) * Interval(2).sqrt())) / 7;
        const Interval c2 = (Interval(3) * (Interval(2) + Interval(3) * Interval(2).sqrt())) / 7;
        return Polyhedron<Interval>(combine<Interval>({
            rotations<Interval>(flips<Interval>(Vertex<Interval>(c2, Interval(0), Interval(0)))),
            permutations<Interval>(flips<Interval>(Vertex<Interval>(c1, c1, Interval(0)))),
            flips<Interval>(Vertex<Interval>(c0, c0, c0))
        }));
    }

    template<IntervalType Interval>
    static Polyhedron<Interval> pentakis_dodecahedron() {
        const Interval c0 = (Interval(3) * (Interval(5).sqrt() - Interval(1))) / Interval(4);
        const Interval c1 = (Interval(9) * (Interval(9) + Interval(5).sqrt())) / Interval(76);
        const Interval c2 = (Interval(9) * (Interval(7) + Interval(5) * Interval(5).sqrt())) / Interval(76);
        const Interval c3 = (Interval(3) * (Interval(1) + Interval(5).sqrt())) / Interval(4);
        return Polyhedron<Interval>(combine<Interval>({
            rotations<Interval>(flips<Interval>(Vertex<Interval>(Interval(0), c0, c3))),
            rotations<Interval>(flips<Interval>(Vertex<Interval>(c1, Interval(0), c2))),
            flips<Interval>(Vertex<Interval>(Interval(3) / 2, Interval(3) / 2, Interval(3) / 2))
        }));
    }

    template<IntervalType Interval>
    static Polyhedron<Interval> triakis_icosahedron() {
        const Interval c0 = (Interval(5) * (Interval(7) + Interval(5).sqrt())) / Interval(44);
        const Interval c1 = (Interval(5) * (Interval(3) + Interval(2) * Interval(5).sqrt())) / Interval(22);
        const Interval c2 = (Interval(5) + Interval(5).sqrt()) / Interval(4);
        const Interval c3 = (Interval(5) * (Interval(13) + Interval(5) * Interval(5).sqrt())) / Interval(44);
        const Interval c4 = (Interval(5) + Interval(3) * Interval(5).sqrt()) / Interval(4);
        return Polyhedron<Interval>(combine<Interval>({
            rotations<Interval>(flips<Interval>(Vertex<Interval>(c2, Interval(0), c4))),
            rotations<Interval>(flips<Interval>(Vertex<Interval>(Interval(0), c0, c3))),
            flips<Interval>(Vertex<Interval>(c1, c1, c1))
        }));
    }

    template<IntervalType Interval>
    static Polyhedron<Interval> deltoidal_hexecontahedron() {
        const Interval c0 = (Interval(5) - Interval(5).sqrt()) / 4;
        const Interval c1 = (Interval(15) + Interval(5).sqrt()) / 22;
        const Interval c2 = Interval(5).sqrt() / 2;
        const Interval c3 = (Interval(5) + Interval(5).sqrt()) / 6;
        const Interval c4 = (Interval(5) + Interval(4) * Interval(5).sqrt()) / 11;
        const Interval c5 = (Interval(5) + Interval(5).sqrt()) / 4;
        const Interval c6 = (Interval(5) + Interval(3) * Interval(5).sqrt()) / 6;
        const Interval c7 = (Interval(25) + Interval(9) * Interval(5).sqrt()) / 22;
        const Interval c8 = Interval(5).sqrt();
        return Polyhedron<Interval>(combine<Interval>({
            rotations<Interval>(flips<Interval>(Vertex<Interval>(Interval(0), Interval(0), c8))),
            rotations<Interval>(flips<Interval>(Vertex<Interval>(Interval(0), c1, c7))),
            rotations<Interval>(flips<Interval>(Vertex<Interval>(c3, Interval(0), c6))),
            rotations<Interval>(flips<Interval>(Vertex<Interval>(c0, c2, c5))),
            flips<Interval>(Vertex<Interval>(c4, c4, c4))
        }));
    }

    template<IntervalType Interval>
    static Polyhedron<Interval> disdyakis_triacontahedron() {
        const Interval c0 = Interval(3) * (Interval(15) + Interval(5).sqrt()) / Interval(44);
        const Interval c1 = (Interval(5) - Interval(5).sqrt()) / Interval(2);
        const Interval c2 = Interval(3) * (Interval(5) + Interval(4) * Interval(5).sqrt()) / Interval(22);
        const Interval c3 = Interval(3) * (Interval(5) + Interval(5).sqrt()) / Interval(10);
        const Interval c4 = Interval(5).sqrt();
        const Interval c5 = Interval(3) * (Interval(25) + Interval(9) * Interval(5).sqrt()) / Interval(44);
        const Interval c6 = Interval(3) * (Interval(5) + Interval(3) * Interval(5).sqrt()) / Interval(10);
        const Interval c7 = (Interval(5) + Interval(5).sqrt()) / Interval(2);
        const Interval c8 = Interval(3) * (Interval(5) + Interval(4) * Interval(5).sqrt()) / Interval(11);
        return Polyhedron<Interval>(combine<Interval>({
            rotations<Interval>(flips<Interval>(Vertex<Interval>(c8, Interval(0), Interval(0)))),
            rotations<Interval>(flips<Interval>(Vertex<Interval>(c1, c7, Interval(0)))),
            rotations<Interval>(flips<Interval>(Vertex<Interval>(c6, c3, Interval(0)))),
            rotations<Interval>(flips<Interval>(Vertex<Interval>(c5, c0, c2))),
            flips<Interval>(Vertex<Interval>(c4, c4, c4))
        }));
    }
}
