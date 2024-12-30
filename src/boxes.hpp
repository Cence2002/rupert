#pragma once

#include "base.hpp"

class Boxes {
    std::queue<Box> boxes;

public:
    Boxes(int theta_intervals, int phi_intervals) {
        for(int theta_i = 0; theta_i < theta_intervals; theta_i++) {
            for(int phi_i = 0; phi_i < phi_intervals; phi_i++) {
                const double theta_min = 2 * M_PIf * static_cast<double>(theta_i) / static_cast<double>(theta_intervals);
                const double theta_max = 2 * M_PIf * static_cast<double>(theta_i + 1) / static_cast<double>(theta_intervals);
                const double phi_min = M_PIf * static_cast<double>(phi_i) / static_cast<double>(phi_intervals);
                const double phi_max = M_PIf * static_cast<double>(phi_i + 1) / static_cast<double>(phi_intervals);
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

    static std::vector<Vector2> boundary(const Vector3 &vertex, const Box &box, double theta_step, double phi_step) {
        const double x = vertex.x;
        const double y = vertex.y;
        const double z = vertex.z;
        const double theta_min = box.theta_interval.min;
        const double theta_max = box.theta_interval.max;
        const double phi_min = box.phi_interval.min;
        const double phi_max = box.phi_interval.max;
        theta_step *= theta_max - theta_min;
        phi_step *= phi_max - phi_min;
        std::vector<Vector2> points;
        // theta = theta_min
        double x_theta_min = -x * std::sin(theta_min) + y * std::cos(theta_min);
        SineWave y_theta_min = SineWave::combine(-z, x * std::cos(theta_min) + y * std::sin(theta_min));
        for(double phi = phi_min; phi <= phi_max; phi += phi_step) {
            points.emplace_back(x_theta_min, y_theta_min(phi));
        }
        // phi = phi_max
        SineWave x_phi_max = SineWave::combine(-x, y);
        SineWave y_phi_max = SineWave::combine(y * std::cos(phi_max), x * std::cos(phi_max));
        double y_phi_max_constant = -z * std::sin(phi_max);
        for(double theta = theta_min; theta <= theta_max; theta += theta_step) {
            points.emplace_back(x_phi_max(theta), y_phi_max(theta) + y_phi_max_constant);
        }
        // theta = theta_max
        double x_theta_max = -x * std::sin(theta_max) + y * std::cos(theta_max);
        SineWave y_theta_max = SineWave::combine(-z, x * std::cos(theta_max) + y * std::sin(theta_max));
        for(double phi = phi_max; phi >= phi_min; phi -= phi_step) {
            points.emplace_back(x_theta_max, y_theta_max(phi));
        }
        // phi = phi_min
        SineWave x_phi_min = SineWave::combine(-x, y);
        SineWave y_phi_min = SineWave::combine(y * std::cos(phi_min), x * std::cos(phi_min));
        double y_phi_min_constant = -z * std::sin(phi_min);
        for(double theta = theta_max; theta >= theta_min; theta -= theta_step) {
            points.emplace_back(x_phi_min(theta), y_phi_min(theta) + y_phi_min_constant);
        }
        return points;
    }
};
