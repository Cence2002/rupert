#pragma once

#include "helper.hpp"

class Boxes {
    std::queue<Box> boxes;

public:
    Boxes(int theta_intervals, int phi_intervals) {
        for(int theta_i = 0; theta_i < theta_intervals; theta_i++) {
            for(int phi_i = 0; phi_i < phi_intervals; phi_i++) {
                const float theta_min = 2 * M_PIf * static_cast<float>(theta_i) / static_cast<float>(theta_intervals);
                const float theta_max = 2 * M_PIf * static_cast<float>(theta_i + 1) / static_cast<float>(theta_intervals);
                const float phi_min = M_PIf * static_cast<float>(phi_i) / static_cast<float>(phi_intervals);
                const float phi_max = M_PIf * static_cast<float>(phi_i + 1) / static_cast<float>(phi_intervals);
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
        Box box = boxes.front();
        boxes.pop();
        return box;
    }

    void push(const Box &box) {
        boxes.push(box);
    }

    std::vector<Box> get_boxes() {
        std::vector<Box> boxes_vector;
        std::queue<Box> boxes_copy = boxes;
        while(!boxes_copy.empty()) {
            boxes_vector.push_back(boxes_copy.front());
            boxes_copy.pop();
        }
        return boxes_vector;
    }

    void push_quadrants(const Box &box) {
        const float theta_min = box.theta_interval.min;
        const float theta_max = box.theta_interval.max;
        const float phi_min = box.phi_interval.min;
        const float phi_max = box.phi_interval.max;
        const float theta_mid = (theta_min + theta_max) / 2;
        const float phi_mid = (phi_min + phi_max) / 2;
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

    static std::vector<Vector2f> boundary(Vector3f vertex, Box box, float theta_step, float phi_step) {
        const float x = vertex.x;
        const float y = vertex.y;
        const float z = vertex.z;
        const float theta_min = box.theta_interval.min;
        const float theta_max = box.theta_interval.max;
        const float phi_min = box.phi_interval.min;
        const float phi_max = box.phi_interval.max;
        theta_step *= theta_max - theta_min;
        phi_step *= phi_max - phi_min;
        std::vector<Vector2f> points;
        // theta = theta_min
        float x_theta_min = -x * std::sin(theta_min) + y * std::cos(theta_min);
        SineWave y_theta_min = SineWave::combine(-z, x * std::cos(theta_min) + y * std::sin(theta_min));
        for(float phi = phi_min; phi <= phi_max; phi += phi_step) {
            points.emplace_back(x_theta_min, y_theta_min(phi));
        }
        // phi = phi_max
        SineWave x_phi_max = SineWave::combine(-x, y);
        SineWave y_phi_max = SineWave::combine(y * std::cos(phi_max), x * std::cos(phi_max));
        float y_phi_max_constant = -z * std::sin(phi_max);
        for(float theta = theta_min; theta <= theta_max; theta += theta_step) {
            points.emplace_back(x_phi_max(theta), y_phi_max(theta) + y_phi_max_constant);
        }
        // theta = theta_max
        float x_theta_max = -x * std::sin(theta_max) + y * std::cos(theta_max);
        SineWave y_theta_max = SineWave::combine(-z, x * std::cos(theta_max) + y * std::sin(theta_max));
        for(float phi = phi_max; phi >= phi_min; phi -= phi_step) {
            points.emplace_back(x_theta_max, y_theta_max(phi));
        }
        // phi = phi_min
        SineWave x_phi_min = SineWave::combine(-x, y);
        SineWave y_phi_min = SineWave::combine(y * std::cos(phi_min), x * std::cos(phi_min));
        float y_phi_min_constant = -z * std::sin(phi_min);
        for(float theta = theta_max; theta >= theta_min; theta -= theta_step) {
            points.emplace_back(x_phi_min(theta), y_phi_min(theta) + y_phi_min_constant);
        }
        return points;
    }
};
