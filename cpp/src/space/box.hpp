#pragma once

#include "space/id.hpp"
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

    static Box invalid() {
        return Box(Id::invalid(), Id::invalid(), Id::invalid());
    }

    bool is_invalid() const {
        return theta_id_.is_invalid() || phi_id_.is_invalid() || alpha_id_.is_invalid();
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

    void to_stream(std::ostream& os) const {
        theta_id_.to_stream(os);
        phi_id_.to_stream(os);
        alpha_id_.to_stream(os);
    }

    static Box from_bytes(std::istream& is) {
        const Id theta_id = Id::from_stream(is);
        const Id phi_id = Id::from_stream(is);
        const Id alpha_id = Id::from_stream(is);
        return Box(theta_id, phi_id, alpha_id);
    }
};
