#pragma once

#include "range/range_type.hpp"
#include "range/range.hpp"

struct Range2 {
private:
    Range theta_range_;
    Range phi_range_;

public:
    explicit Range2(const Range& theta_range, const Range& phi_range) noexcept : theta_range_(theta_range), phi_range_(phi_range) {}

    ~Range2() = default;

    Range2(const Range2& range2) = default;

    Range2(Range2&& range2) = default;

    Range2& operator=(const Range2&) = default;

    Range2& operator=(Range2&&) = default;

    bool operator<(const Range2& other) const {
        if(theta_range_ < other.theta_range_) {
            return true;
        }
        if(other.theta_range_ < theta_range_) {
            return false;
        }
        return phi_range_ < other.phi_range_;
    }

    Range theta_range() const {
        return theta_range_;
    }

    Range phi_range() const {
        return phi_range_;
    }

    std::vector<Range2> parts() const {
        std::vector<Range2> parts;
        for(const Range& theta_part: theta_range_.parts()) {
            for(const Range& phi_part: phi_range_.parts()) {
                parts.emplace_back(theta_part, phi_part);
            }
        }
        return parts;
    }

    template<IntervalType Interval>
    Interval theta_interval() const {
        return theta_range_.angle_interval<Interval>();
    }

    template<IntervalType Interval>
    Interval phi_interval() const {
        return phi_range_.angle_interval<Interval>();
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Range2& range2) {
        return ostream << range2.theta_range_ << range2.phi_range_;
    }
};

static_assert(RangeType<Range2>);
