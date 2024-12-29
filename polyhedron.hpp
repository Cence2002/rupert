#pragma once

#include "helper.hpp"

struct Polyhedron {
    static constexpr float PHI = (1 + std::sqrt(5.0f)) / 2;

    static std::vector<Vector3f> cube() {
        std::vector<Vector3f> vertices = {
                    {1, 1, 1},
                    {1, 1, -1},
                    {1, -1, 1},
                    {1, -1, -1},
                    {-1, 1, 1},
                    {-1, 1, -1},
                    {-1, -1, 1},
                    {-1, -1, -1}
                };
        for(Vector3f &vertex: vertices) {
            vertex.normalize();
        }
        return vertices;
    }

    static std::vector<Vector3f> octahedron() {
        std::vector<Vector3f> vertices = {
                    {1, 0, 0},
                    {-1, 0, 0},
                    {0, 1, 0},
                    {0, -1, 0},
                    {0, 0, 1},
                    {0, 0, -1}
                };
        for(Vector3f &vertex: vertices) {
            vertex.normalize();
        }
        return vertices;
    }

    static std::vector<Vector3f> icosahedron() {
        std::vector<Vector3f> vertices = {
                    {PHI, 0, 1},
                    {-PHI, 0, 1},
                    {PHI, 0, -1},
                    {-PHI, 0, -1},
                    {1, PHI, 0},
                    {-1, PHI, 0},
                    {1, -PHI, 0},
                    {-1, -PHI, 0},
                    {0, 1, PHI},
                    {0, -1, PHI},
                    {0, 1, -PHI},
                    {0, -1, -PHI}
                };
        for(Vector3f &vertex: vertices) {
            vertex.normalize();
        }
        return vertices;
    }

    static std::vector<Vector3f> dodecahedron() {
        std::vector<Vector3f> vertices = {
                    {1, 1, 1},
                    {1, 1, -1},
                    {1, -1, 1},
                    {1, -1, -1},
                    {-1, 1, 1},
                    {-1, 1, -1},
                    {-1, -1, 1},
                    {-1, -1, -1},
                    {0, 1 / PHI, PHI},
                    {0, 1 / PHI, -PHI},
                    {0, -1 / PHI, PHI},
                    {0, -1 / PHI, -PHI},
                    {1 / PHI, PHI, 0},
                    {1 / PHI, -PHI, 0},
                    {-1 / PHI, PHI, 0},
                    {-1 / PHI, -PHI, 0},
                    {PHI, 0, 1 / PHI},
                    {PHI, 0, -1 / PHI},
                    {-PHI, 0, 1 / PHI},
                    {-PHI, 0, -1 / PHI}
                };
        for(Vector3f &vertex: vertices) {
            vertex.normalize();
        }
        return vertices;
    }
};
