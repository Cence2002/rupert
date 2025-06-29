#pragma once

#include "space/id.hpp"
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

    static Rectangle invalid() {
        return Rectangle(Id::invalid(), Id::invalid());
    }

    bool is_invalid() const {
        return theta_id_.is_invalid() || phi_id_.is_invalid();
    }

    static size_t size() {
        return Id::size() * 2;
    }

    std::array<Rectangle, 4> parts() const {
        const auto [theta_id_min_part, theta_id_max_part] = theta_id_.parts();
        const auto [phi_id_min_part, phi_id_max_part] = phi_id_.parts();
        return std::array<Rectangle, 4>{
            Rectangle(theta_id_min_part, phi_id_min_part),
            Rectangle(theta_id_min_part, phi_id_max_part),
            Rectangle(theta_id_max_part, phi_id_min_part),
            Rectangle(theta_id_max_part, phi_id_max_part)
        };
    }

    template<IntervalType Interval>
    Interval theta() const {
        return theta_id_.interval<Interval>() * Interval(2) * Interval::pi();
    }

    template<IntervalType Interval>
    Interval phi() const {
        return phi_id_.interval<Interval>() * Interval::pi();
    }

    friend std::ostream& operator<<(std::ostream& os, const Rectangle& rectangle) {
        return os << "T" << rectangle.theta_id_ << " P" << rectangle.phi_id_;
    }
};
