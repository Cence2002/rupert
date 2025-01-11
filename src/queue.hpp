#pragma once

#include "interval.hpp"
#include <optional>
#include <random>
#include <thread>
#include <boost/lockfree/queue.hpp>

template<typename T>
class Queue {
    std::queue<T> queue;
    std::mutex mutex;

public:
    void push(const T &value) {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(value);
    }

    std::optional<T> pop() {
        std::lock_guard<std::mutex> lock(mutex);
        if(queue.empty()) {
            return std::nullopt;
        }
        const T value = queue.front();
        queue.pop();
        return std::make_optional(value);
    }

    std::vector<T> vector() {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<T> values;
        for(const T &value: queue.c) {
            values.push_back(value);
        }
        return values;
    }
};

class Queue2 {
    std::queue<Box2> queue;

public:
    explicit Queue2(const int theta_parts = 1, const int phi_parts = 1) {
        for(const BoostInterval &theta: divide(Box2::theta_interval, theta_parts)) {
            for(const BoostInterval &phi: divide(Box2::phi_interval, phi_parts)) {
                queue.push({theta, phi});
            }
        }
    }

    bool empty() const {
        return queue.empty();
    }

    void process(const std::vector<Vector3d> &polyhedron, const std::vector<Vector2d> &hole) {
        if(queue.empty()) {
            return;
        }
        const Box2 box = queue.front();
        queue.pop();
        for(const Vector3d &vertex: polyhedron) {
            if(!box_intersects_polygon(box, vertex, hole)) {
                return;
            }
        }
        for(const Box2 &sub_box: box.sub_boxes()) {
            queue.push(sub_box);
        }
    }

    bool check(const std::vector<Vector3d> &polyhedron, const std::vector<Vector2d> &hole) const {
        const Box2 box = queue.front();
        for(const Vector3d &vertex: polyhedron) {
            if(!point_inside_polygon(vertex.project(median(box.theta), median(box.phi)), hole)) {
                return false;
            }
        }
        return true;
    }
};

class Queue3 {
    Queue<Box3> queue;

public:
    explicit Queue3(const int theta_parts = 1, const int phi_parts = 1, const int alpha_parts = 1) {
        std::vector<Box3> boxes;
        for(const BoostInterval &theta: divide(Box3::theta_interval, theta_parts)) {
            for(const BoostInterval &phi: divide(Box3::phi_interval, phi_parts)) {
                for(const BoostInterval &alpha: divide(Box3::alpha_interval, alpha_parts)) {
                    boxes.push_back({theta, phi, alpha});
                }
            }
        }
        std::shuffle(boxes.begin(), boxes.end(), std::mt19937(std::random_device()()));
        for(const Box3 &box: boxes) {
            queue.push(box);
        }
    }

    void process(const std::vector<Vector3d> &polyhedron, const int resolution = 2, const int iterations = 10000, const int frequency = 10) {
        const std::optional<Box3> optional_box = queue.pop();
        if(!optional_box.has_value()) {
            return;
        }
        const Box3 &box = optional_box.value();
        std::vector<Vector2d> hole_all;
        for(const BoostInterval &theta: divide(box.theta, resolution)) {
            for(const BoostInterval &phi: divide(box.phi, resolution)) {
                for(const BoostInterval &alpha: divide(box.alpha, resolution)) {
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
        for(int i = 0; i < iterations; i++) {
            if(iterations % frequency == 0) {
                if(queue2.check(polyhedron, hole)) {
                    // break;
                }
            }
            queue2.process(polyhedron, hole);
        }

        if(!queue2.empty()) {
            for(const Box3 &sub_box: box.sub_boxes()) {
                queue.push(sub_box);
            }
            std::cout << "Divided Box3 " << box << std::endl;
        } else {
            std::cout << "Empty Box3 " << box << " volume " << width(box.theta) * width(box.phi) * width(box.alpha) << std::endl;
        }
    }
};
