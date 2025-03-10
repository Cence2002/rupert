#pragma once

#include "interval/interval.hpp"
#include "pipeline/id.hpp"
#include <array>

struct Box {
private:
    Id theta_id_;
    Id phi_id_;
    Id alpha_id_;

public:
    explicit Box() : theta_id_(), phi_id_(), alpha_id_() {}

    explicit Box(const Id& theta_id, const Id& phi_id, const Id& alpha_id) : theta_id_(theta_id), phi_id_(phi_id), alpha_id_(alpha_id) {}

    Id theta_id() const {
        return theta_id_;
    }

    Id phi_id() const {
        return phi_id_;
    }

    Id alpha_id() const {
        return alpha_id_;
    }

    bool is_overflow() const {
        return theta_id_.is_overflow() || phi_id_.is_overflow() || alpha_id_.is_overflow();
    }

    std::array<Box, 8> split() const {
        const auto [theta_id_min, theta_id_max] = theta_id_.split();
        const auto [phi_id_min, phi_id_max] = phi_id_.split();
        const auto [alpha_id_min, alpha_id_max] = alpha_id_.split();
        return std::array<Box, 8>{
            Box(theta_id_min, phi_id_min, alpha_id_min),
            Box(theta_id_min, phi_id_min, alpha_id_max),
            Box(theta_id_min, phi_id_max, alpha_id_min),
            Box(theta_id_min, phi_id_max, alpha_id_max),
            Box(theta_id_max, phi_id_min, alpha_id_min),
            Box(theta_id_max, phi_id_min, alpha_id_max),
            Box(theta_id_max, phi_id_max, alpha_id_min),
            Box(theta_id_max, phi_id_max, alpha_id_max)
        };
    }

    double size() const {
        return theta_id_.len() * phi_id_.len() * alpha_id_.len();
    }

    template<IntervalType Interval>
    Interval theta() const {
        return theta_id_.interval<Interval>() * 2 * Interval::pi();
    }

    template<IntervalType Interval>
    Interval phi() const {
        return phi_id_.interval<Interval>() * Interval::pi();
    }

    template<IntervalType Interval>
    Interval alpha() const {
        return alpha_id_.interval<Interval>() * 2 * Interval::pi();
    }

    friend std::ostream& operator<<(std::ostream& os, const Box& box) {
        return os << "T" << box.theta_id_ << " P" << box.phi_id_ << " A" << box.alpha_id_;
    }
};