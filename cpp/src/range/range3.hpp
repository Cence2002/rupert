#pragma once

#include "range/range_type.hpp"
#include "range/range.hpp"

struct Range3 {
private:
    Range theta_range_;
    Range phi_range_;
    Range alpha_range_;

public:
    explicit Range3(const Range& theta_range, const Range& phi_range, const Range& alpha_range) noexcept : theta_range_(theta_range), phi_range_(phi_range), alpha_range_(alpha_range) {}

    explicit Range3() : theta_range_(), phi_range_(), alpha_range_() {}

    ~Range3() = default;

    Range3(const Range3& range3) = default;

    Range3(Range3&& range3) = default;

    Range3& operator=(const Range3&) = delete;

    Range3& operator=(Range3&&) = delete;

    Range theta_range() const {
        return theta_range_;
    }

    Range phi_range() const {
        return phi_range_;
    }

    Range alpha_range() const {
        return alpha_range_;
    }

    bool is_valid() const {
        return theta_range_.is_valid() && phi_range_.is_valid() && alpha_range_.is_valid();
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
    Interval theta_interval() const {
        return theta_range_.angle_interval<Interval>();
    }

    template<IntervalType Interval>
    Interval phi_interval() const {
        return phi_range_.angle_interval<Interval>();
    }

    template<IntervalType Interval>
    Interval alpha_interval() const {
        return alpha_range_.angle_interval<Interval>();
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Range3& box) {
        return ostream << "T" << box.theta_range_ << "P" << box.phi_range_ << "A" << box.alpha_range_;
    }
};

static_assert(RangeType<Range3>);
