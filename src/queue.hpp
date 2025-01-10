#pragma once

#include "interval.hpp"
#include <optional>
#include <thread>
#include <boost/lockfree/queue.hpp>

class Queue2 {
    boost::lockfree::queue<TrivialBox2> queue;
    std::atomic<int> size = 0;

public:
    explicit Queue2(const int size, const int theta_parts = 1, const int phi_parts = 1) : queue(size) {
        for(const Interval &theta: divide(Box2::theta_interval, theta_parts)) {
            for(const Interval &phi: divide(Box2::phi_interval, phi_parts)) {
                const Box2 box(theta, phi);
                push(box.trivial());
            }
        }
    }

    bool empty() const {
        return size == 0;
    }

    std::optional<TrivialBox2> pop(const bool decrement = true) {
        TrivialBox2 box;
        while(!queue.pop(box)) {
            if(empty()) {
                return std::nullopt;
            }
        }
        if(decrement) {
            --size;
        }
        return box;
    }

    void push(const TrivialBox2 &box) {
        ++size;
        while(!queue.push(box)) {
            std::cout << "Queue2 push failed" << std::endl;
        }
    }

    void push_sub_boxes(const Box2 &box) {
        for(const Box2 &sub_box: box.sub_boxes()) {
            push(sub_box.trivial());
        }
    }

    bool process(const std::vector<Vector3d> &polyhedron, const std::vector<Vector2d> &hole, const bool check_fit = false) {
        if(empty()) {
            return false;
        }
        const std::optional<TrivialBox2> optional_box = pop(false);
        if(!optional_box.has_value()) {
            return false;
        }
        const Box2 box(optional_box.value());
        for(const Vector3d &vertex: polyhedron) {
            if(!box_intersects_polygon(box, vertex, hole)) {
                --size;
                return false;
            }
        }
        push_sub_boxes(box);
        --size;

        if(!check_fit) {
            return false;
        }
        for(const Vector3d &vertex: polyhedron) {
            if(!point_inside_polygon(vertex.project(median(box.theta), median(box.phi)), hole)) {
                return false;
            }
        }
        return true;
    }
};

class Queue3 {
    std::queue<Box3> queue;

public:
    explicit Queue3(const int theta_parts = 1, const int phi_parts = 1, const int alpha_parts = 1) {
        for(const Interval &theta: divide(Box3::theta_interval, theta_parts)) {
            for(const Interval &phi: divide(Box3::phi_interval, phi_parts)) {
                for(const Interval &alpha: divide(Box3::alpha_interval, alpha_parts)) {
                    queue.push({theta, phi, alpha});
                }
            }
        }
    }

    bool empty() const {
        return queue.empty();
    }

    Box3 pop() {
        const Box3 box = queue.front();
        queue.pop();
        return box;
    }

    void push_sub_boxes(const Box3 &box) {
        for(const Box3 &sub_box: box.sub_boxes()) {
            queue.push(sub_box);
        }
    }

    void process(const std::vector<Vector3d> &polyhedron, const int resolution = 2, const int iterations = 10000, const int frequency = 10, const int thread_count = 10) {
        if(empty()) {
            return;
        }
        const Box3 box = pop();
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

        Queue2 queue2(iterations * 8);

        std::vector<std::thread> threads;
        bool done = false;
        for(int i = 0; i < thread_count; i++) {
            threads.emplace_back([&] {
                for(int t = 0; !done && t < iterations / thread_count + 1; t++) {
                    if(queue2.process(polyhedron, hole, t % thread_count == 0)) {
                        done = true;
                    }
                }
            });
        }
        for(std::thread &thread: threads) {
            thread.join();
        }

        if(!queue2.empty()) {
            push_sub_boxes(box);
            std::cout << "Divided Box3 " << box << std::endl;
        } else {
            std::cout << "Empty Box3 " << box << " volume " << width(box.theta) * width(box.phi) * width(box.alpha) << std::endl;
        }
    }
};
