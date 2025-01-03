#pragma once

#include "base.hpp"

struct Intervals : std::queue<Interval2> {
    Intervals(const int theta_parts, const int phi_parts) {
        for(const Interval &theta: divide(Interval(0, 2 * M_PI),theta_parts)) {
            for(const Interval &phi: divide(Interval(0, M_PI),phi_parts)) {
                emplace(theta, phi);
            }
        }
    }

    std::vector<Interval2> get_vector() const {
        std::queue<Interval2> copy = *this;
        std::vector<Interval2> vector;
        while(!copy.empty()) {
            vector.push_back(copy.front());
            copy.pop();
        }
        return vector;
    }

    void push_quad(const Interval2 &box) {
        push({{lower(box.theta), median(box.theta)}, {lower(box.phi), median(box.phi)}});
        push({{lower(box.theta), median(box.theta)}, {median(box.phi), upper(box.phi)}});
        push({{median(box.theta), upper(box.theta)}, {lower(box.phi), median(box.phi)}});
        push({{median(box.theta), upper(box.theta)}, {median(box.phi), upper(box.phi)}});
    }
};
