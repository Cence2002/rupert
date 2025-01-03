#pragma once

#include "base.hpp"

struct Intervals : std::queue<Interval2> {
    Intervals(const int theta_parts, const int phi_parts) {
        for(const Interval &theta: Interval(0, 2 * M_PI).divide(theta_parts)) {
            for(const Interval &phi: Interval(0, M_PI).divide(phi_parts)) {
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
        push({{box.theta.lower(), box.theta.center()}, {box.phi.lower(), box.phi.center()}});
        push({{box.theta.lower(), box.theta.center()}, {box.phi.center(), box.phi.upper()}});
        push({{box.theta.center(), box.theta.upper()}, {box.phi.lower(), box.phi.center()}});
        push({{box.theta.center(), box.theta.upper()}, {box.phi.center(), box.phi.upper()}});
    }
};
