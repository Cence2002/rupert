#pragma once

#include "range/range_type.hpp"
#include "range/range.hpp"

class Range3 {
    Range theta_range_;
    Range phi_range_;
    Range alpha_range_;

public:
    explicit Range3(const Range& theta_range, const Range& phi_range, const Range& alpha_range) : theta_range_(theta_range), phi_range_(phi_range), alpha_range_(alpha_range) {}

    ~Range3() = default;

    Range3(const Range3& range3) = default;

    Range3(Range3&& range3) = default;

    Range3& operator=(const Range3&) = default;

    Range3& operator=(Range3&&) = default;

    bool operator<(const Range3& other) const {
        if(theta_range_ < other.theta_range_) {
            return true;
        }
        if(other.theta_range_ < theta_range_) {
            return false;
        }
        if(phi_range_ < other.phi_range_) {
            return true;
        }
        if(other.phi_range_ < phi_range_) {
            return false;
        }
        return alpha_range_ < other.alpha_range_;
    }

    Range theta_range() const {
        return theta_range_;
    }

    Range phi_range() const {
        return phi_range_;
    }

    Range alpha_range() const {
        return alpha_range_;
    }

    bool terminal() const {
        return theta_range_.terminal() || phi_range_.terminal() || alpha_range_.terminal();
    }

    std::vector<Range3> parts() const {
        std::vector<Range3> parts;
        for(const Range& theta_part: theta_range_.parts()) {
            for(const Range& phi_part: phi_range_.parts()) {
                for(const Range& alpha_part: alpha_range_.parts()) {
                    parts.emplace_back(theta_part, phi_part, alpha_part);
                }
            }
        }
        return parts;
    }

    template<IntervalType Interval>
    Interval theta() const {
        return theta_range_.angle<Interval>();
    }

    template<IntervalType Interval>
    Interval phi() const {
        return phi_range_.angle<Interval>();
    }

    template<IntervalType Interval>
    Interval alpha() const {
        return alpha_range_.angle<Interval>();
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Range3& box) {
        return ostream << box.theta_range_ << box.phi_range_ << box.alpha_range_;
    }
};

static_assert(RangeType<Range3>);
