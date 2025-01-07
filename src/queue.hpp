#pragma once

#include "interval.hpp"

struct Queue2 : std::queue<Interval2> {
    Queue2(const int theta_parts, const int phi_parts) {
        const Interval theta_interval(0, boost::numeric::interval_lib::constants::pi_twice_upper<double>());
        const Interval phi_interval(0, boost::numeric::interval_lib::constants::pi_upper<double>());
        for(const Interval &theta: divide(theta_interval, theta_parts)) {
            for(const Interval &phi: divide(phi_interval, phi_parts)) {
                emplace(theta, phi);
            }
        }
    }

    Interval2 fetch() {
        const Interval2 interval2 = front();
        pop();
        return interval2;
    }

    std::vector<Interval2> get_vector() const {
        std::vector<Interval2> intervals;
        for(const Interval2 &interval2: c) {
            intervals.push_back(interval2);
        }
        return intervals;
    }

    void push_parts(const Interval2 &interval2) {
        for(const Interval2 &quadrant: interval2.parts()) {
            push(quadrant);
        }
    }
};

struct Queue3 : std::queue<Interval3> {
    Queue3(const int theta_parts, const int phi_parts, const int alpha_parts) {
        const Interval theta_interval(0, boost::numeric::interval_lib::constants::pi_twice_upper<double>());
        const Interval phi_interval(0, boost::numeric::interval_lib::constants::pi_upper<double>());
        const Interval alpha_interval(0, boost::numeric::interval_lib::constants::pi_twice_upper<double>());
        for(const Interval &theta: divide(theta_interval, theta_parts)) {
            for(const Interval &phi: divide(phi_interval, phi_parts)) {
                for(const Interval &alpha: divide(alpha_interval, alpha_parts)) {
                    emplace(theta, phi, alpha);
                }
            }
        }
    }

    Interval3 fetch() {
        const Interval3 interval3 = front();
        pop();
        return interval3;
    }

    std::vector<Interval3> get_vector() const {
        std::vector<Interval3> intervals;
        for(const Interval3 &interval3: c) {
            intervals.push_back(interval3);
        }
        return intervals;
    }

    void push_parts(const Interval3 &interval3) {
        for(const Interval3 &octant: interval3.parts()) {
            push(octant);
        }
    }
};
