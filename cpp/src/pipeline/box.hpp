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

    bool invalid() const {
        return theta_id_.invalid() || phi_id_.invalid() || alpha_id_.invalid();
    }

    size_t size() const {
        return theta_id_.size() + phi_id_.size() + alpha_id_.size();
    }

    std::array<Box, 8> subdivide() const {
        const auto [theta_id_min_half, theta_id_max_half] = theta_id_.subdivide();
        const auto [phi_id_min_half, phi_id_max_half] = phi_id_.subdivide();
        const auto [alpha_id_min_half, alpha_id_max_half] = alpha_id_.subdivide();
        return std::array<Box, 8>{
            Box(theta_id_min_half, phi_id_min_half, alpha_id_min_half),
            Box(theta_id_min_half, phi_id_min_half, alpha_id_max_half),
            Box(theta_id_min_half, phi_id_max_half, alpha_id_min_half),
            Box(theta_id_min_half, phi_id_max_half, alpha_id_max_half),
            Box(theta_id_max_half, phi_id_min_half, alpha_id_min_half),
            Box(theta_id_max_half, phi_id_min_half, alpha_id_max_half),
            Box(theta_id_max_half, phi_id_max_half, alpha_id_min_half),
            Box(theta_id_max_half, phi_id_max_half, alpha_id_max_half)
        };
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
