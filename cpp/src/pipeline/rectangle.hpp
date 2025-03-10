#pragma once

#include "interval/interval.hpp"
#include "pipeline/id.hpp"
#include <array>

struct Rectangle {
private:
    Id theta_id_;
    Id phi_id_;

public:
    explicit Rectangle() : theta_id_(), phi_id_() {}

    explicit Rectangle(const Id& theta_id, const Id& phi_id) : theta_id_(theta_id), phi_id_(phi_id) {}

    Id theta_id() const {
        return theta_id_;
    }

    Id phi_id() const {
        return phi_id_;
    }

    bool invalid() const {
        return theta_id_.invalid() || phi_id_.invalid();
    }

    size_t size() const {
        return theta_id_.size() + phi_id_.size();
    }

    std::array<Rectangle, 4> subdivide() const {
        const auto [theta_id_min_half, theta_id_max_half] = theta_id_.subdivide();
        const auto [phi_id_min_half, phi_id_max_half] = phi_id_.subdivide();
        return std::array<Rectangle, 4>{
            Rectangle(theta_id_min_half, phi_id_min_half),
            Rectangle(theta_id_min_half, phi_id_max_half),
            Rectangle(theta_id_max_half, phi_id_min_half),
            Rectangle(theta_id_max_half, phi_id_max_half)
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

    friend std::ostream& operator<<(std::ostream& os, const Rectangle& rectangle) {
        return os << "T" << rectangle.theta_id_ << " P" << rectangle.phi_id_;
    }
};
