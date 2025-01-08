#pragma once

#include "interval.hpp"

struct Queue2 : std::queue<Box2> {
    explicit Queue2(const int theta_parts = 1, const int phi_parts = 1) {
        for(const Interval &theta: divide(Box2::theta_interval, theta_parts)) {
            for(const Interval &phi: divide(Box2::phi_interval, phi_parts)) {
                emplace(theta, phi);
            }
        }
    }

    Box2 fetch() {
        const Box2 box = front();
        pop();
        return box;
    }

    std::vector<Box2> vector() const {
        std::vector<Box2> boxes;
        for(const Box2 &box: c) {
            boxes.push_back(box);
        }
        return boxes;
    }

    void push_sub_boxes(const Box2 &box) {
        for(const Box2 &sub_box: box.sub_boxes()) {
            push(sub_box);
        }
    }

    void process(const std::vector<Vector3d> &polyhedron, const std::vector<Vector2d> &hole) {
        if(empty()) {
            return;
        }
        const Box2 box = fetch();
        for(const Vector3d &vertex: polyhedron) {
            if(!box_intersects_polygon(vertex, box, hole)) {
                return;
            }
        }
        push_sub_boxes(box);
    }
};

struct Queue3 : std::queue<Box3> {
    explicit Queue3(const int theta_parts = 1, const int phi_parts = 1, const int alpha_parts = 1) {
        for(const Interval &theta: divide(Box3::theta_interval / 4.0 + 1.0, theta_parts)) {
            for(const Interval &phi: divide(Box3::phi_interval / 4.0 + 1.0, phi_parts)) {
                for(const Interval &alpha: divide(Box3::alpha_interval / 4.0 + 1.0, alpha_parts)) {
                    emplace(theta, phi, alpha);
                }
            }
        }
    }

    Box3 fetch() {
        const Box3 box = front();
        pop();
        return box;
    }

    std::vector<Box3> vector() const {
        std::vector<Box3> boxes;
        for(const Box3 &box: c) {
            boxes.push_back(box);
        }
        return boxes;
    }

    void push_sub_boxes(const Box3 &box) {
        for(const Box3 &sub_box: box.sub_boxes()) {
            push(sub_box);
        }
    }

    void process(const std::vector<Vector3d> &polyhedron, const int resolution = 10, const int iterations = 10000) {
        if(empty()) {
            return;
        }
        const Box3 box = fetch();
        std::vector<Vector2d> hole_all;
        for(const Interval &theta: divide(box.theta, resolution)) {
            for(const Interval &phi: divide(box.phi, resolution)) {
                for(const Interval &alpha: divide(box.alpha, resolution)) {
                    for(const auto &[x, y, z]: polyhedron) {
                        const auto [x_interval, y_interval] = Vector3I(x, y, z).project(theta, phi).rotate(alpha);
                        hole_all.emplace_back(lower(x_interval), lower(y_interval));
                        hole_all.emplace_back(lower(x_interval), upper(y_interval));
                        hole_all.emplace_back(upper(x_interval), lower(y_interval));
                        hole_all.emplace_back(upper(x_interval), upper(y_interval));
                    }
                }
            }
        }
        std::vector<Vector2d> hole = convex_hull(hole_all);
        std::ranges::sort(hole, [](const Vector2d &vertex_0, const Vector2d &vertex_1) {
            return vertex_0.angle() < vertex_1.angle();
        });

        Queue2 queue2;
        for(int t = 0; t < iterations; t++) {
            queue2.process(polyhedron, hole);
        }
        if(!queue2.empty()) {
            push_sub_boxes(box);
        }
        if(!queue2.empty()) {
            std::cout << "Divided Box3 " << box << std::endl;
        } else {
            std::cout << "Empty Box3 " << box << std::endl;
        }
    }
};
