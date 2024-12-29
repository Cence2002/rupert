#pragma once

#include "helper.hpp"

/*
    std::queue<Box> boxes;
    for(int theta_i = 0; theta_i < 20; theta_i++) {
        for(int phi_i = 0; phi_i < 10; phi_i++) {
            float theta_min = theta_i * 2 * M_PI / 20;
            float theta_max = (theta_i + 1) * 2 * M_PI / 20;
            float phi_min = phi_i * M_PI / 10;
            float phi_max = (phi_i + 1) * M_PI / 10;
            boxes.push({
                    Interval(theta_min, theta_max),
                    Interval(phi_min, phi_max)
                });
        }
    }
    while(!boxes.empty()) {
        Box box = boxes.front();
        Box shifted_box = {
                    Interval(box.theta_interval.min / M_PI - 1, box.theta_interval.max / M_PI - 1),
                    Interval(box.phi_interval.min / M_PI - 2, box.phi_interval.max / M_PI - 2)
                };
        plot.box(shifted_box, Color::BLUE);
        boxes.pop();
    }
 */

class Cover {
    std::queue<Box> boxes;

public:
    Cover(int theta_intervals, int phi_intervals) {
        for(int theta_i = 0; theta_i < theta_intervals; theta_i++) {
            for(int phi_i = 0; phi_i < phi_intervals; phi_i++) {
                float theta_min = theta_i * 2 * M_PIf / theta_intervals;
                float theta_max = (theta_i + 1) * 2 * M_PIf / theta_intervals;
                float phi_min = phi_i * M_PIf / phi_intervals;
                float phi_max = (phi_i + 1) * M_PIf / phi_intervals;
                boxes.push(Box{
                        Interval(theta_min, theta_max),
                        Interval(phi_min, phi_max)
                    });
            }
        }
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

    static void plot_box(Plot &plot, const Box &box, int thickness = 1) {
        Box shifted_box = {
                    Interval(box.theta_interval.min / M_PIf - 1, box.theta_interval.max / M_PIf - 1),
                    Interval(box.phi_interval.min / M_PI_2f - 1, box.phi_interval.max / M_PI_2f - 1)
                };
        float x = (box.theta_interval.min + box.theta_interval.max) / 2 / (2 * M_PIf);
        float y = (box.phi_interval.min + box.phi_interval.max) / 2 / M_PIf;
        cv::Scalar color = Color::gradient(y,
                                           Color::gradient(x, Color::bottom_left, Color::bottom_right),
                                           Color::gradient(x, Color::top_left, Color::top_right));
        plot.filled_box(shifted_box, color);
        if(thickness > 0) {
            plot.box(shifted_box, Color::GRAY, thickness);
        }
    }

    void plot_boxes(Plot &plot, int thickness = 1) {
        for(const Box &box: get_boxes()) {
            plot_box(plot, box, thickness);
        }
        if(thickness > 0) {
            plot.box(Box{Interval(-1, 1), Interval(-1, 1)}, Color::WHITE, thickness);
        }
    }

    Box pop() {
        Box box = boxes.front();
        boxes.pop();
        return box;
    }

    void push(const Box &box) {
        boxes.push(box);
    }

    void push_quadrants(const Box &box) {
        float theta_mid = (box.theta_interval.min + box.theta_interval.max) / 2;
        float phi_mid = (box.phi_interval.min + box.phi_interval.max) / 2;
        boxes.push({
                Interval(box.theta_interval.min, theta_mid),
                Interval(box.phi_interval.min, phi_mid)
            });
        boxes.push({
                Interval(theta_mid, box.theta_interval.max),
                Interval(box.phi_interval.min, phi_mid)
            });
        boxes.push({
                Interval(box.theta_interval.min, theta_mid),
                Interval(phi_mid, box.phi_interval.max)
            });
        boxes.push({
                Interval(theta_mid, box.theta_interval.max),
                Interval(phi_mid, box.phi_interval.max)
            });
    }
};
