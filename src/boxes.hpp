#pragma once

#include "base.hpp"

class Boxes {
    std::queue<Box> boxes;

public:
    Boxes(const int theta_intervals, const int phi_intervals) {
        for(int theta_i = 0; theta_i < theta_intervals; theta_i++) {
            for(int phi_i = 0; phi_i < phi_intervals; phi_i++) {
                const double theta_min = 2 * M_PI * static_cast<double>(theta_i) / static_cast<double>(theta_intervals);
                const double theta_max = 2 * M_PI * static_cast<double>(theta_i + 1) / static_cast<double>(theta_intervals);
                const double phi_min = M_PI * static_cast<double>(phi_i) / static_cast<double>(phi_intervals);
                const double phi_max = M_PI * static_cast<double>(phi_i + 1) / static_cast<double>(phi_intervals);
                push(Box{
                        Interval(theta_min, theta_max),
                        Interval(phi_min, phi_max)
                    });
            }
        }
    }

    bool empty() const {
        return boxes.empty();
    }

    Box pop() {
        const Box box = boxes.front();
        boxes.pop();
        return box;
    }

    void push(const Box &box) {
        boxes.push(box);
    }

    std::vector<Box> get_boxes() const {
        std::vector<Box> boxes_vector;
        std::queue<Box> boxes_copy = boxes;
        while(!boxes_copy.empty()) {
            boxes_vector.push_back(boxes_copy.front());
            boxes_copy.pop();
        }
        return boxes_vector;
    }

    void push_quadrants(const Box &box) {
        const double theta_min = box.theta_interval.min;
        const double theta_max = box.theta_interval.max;
        const double phi_min = box.phi_interval.min;
        const double phi_max = box.phi_interval.max;
        const double theta_mid = (theta_min + theta_max) / 2;
        const double phi_mid = (phi_min + phi_max) / 2;
        boxes.push({
                Interval(theta_min, theta_mid),
                Interval(phi_min, phi_mid)
            });
        boxes.push({
                Interval(theta_min, theta_mid),
                Interval(phi_mid, phi_max)
            });
        boxes.push({
                Interval(theta_mid, theta_max),
                Interval(phi_min, phi_mid)
            });
        boxes.push({
                Interval(theta_mid, theta_max),
                Interval(phi_mid, phi_max)
            });
    }
};
