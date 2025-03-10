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

    bool is_overflow() const {
        return theta_id_.is_overflow() || phi_id_.is_overflow();
    }

    std::array<Rectangle, 4> split() const {
        const auto [theta_id_min, theta_id_max] = theta_id_.split();
        const auto [phi_id_min, phi_id_max] = phi_id_.split();
        return std::array<Rectangle, 4>{
            Rectangle(theta_id_min, phi_id_min),
            Rectangle(theta_id_min, phi_id_max),
            Rectangle(theta_id_max, phi_id_min),
            Rectangle(theta_id_max, phi_id_max)
        };
    }

    double size() const {
        return theta_id_.len() * phi_id_.len();
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