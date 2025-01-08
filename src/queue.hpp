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
};

struct Queue3 : std::queue<Box3> {
    explicit Queue3(const int theta_parts = 1, const int phi_parts = 1, const int alpha_parts = 1) {
        for(const Interval &theta: divide(Box3::theta_interval, theta_parts)) {
            for(const Interval &phi: divide(Box3::phi_interval, phi_parts)) {
                for(const Interval &alpha: divide(Box3::alpha_interval, alpha_parts)) {
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
};
