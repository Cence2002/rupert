#pragma once

#include "interval/interval_type.hpp"
#include <boost/numeric/interval.hpp>

class BoostInterval {
    using BoostIntervalType = boost::numeric::interval<
        double,
        boost::numeric::interval_lib::policies<
            boost::numeric::interval_lib::save_state<
                boost::numeric::interval_lib::rounded_transc_std<double>>,
            boost::numeric::interval_lib::checking_base<double>>>;

    BoostIntervalType interval_;

    explicit BoostInterval(const BoostIntervalType& interval) : interval_(interval) {}

    explicit BoostInterval(const double min, const double max) : interval_(min, max) {}

    explicit BoostInterval(const double value) : interval_(value) {}

public:
    explicit BoostInterval(const int min, const int max) : interval_(min, max) {
        if(min > max) {
            throw std::invalid_argument("min > max");
        }
    }

    explicit BoostInterval(const int value) : interval_(value) {}

    ~BoostInterval() = default;

    BoostInterval(const BoostInterval& interval) = default;

    BoostInterval(BoostInterval&& interval) = default;

    BoostInterval& operator=(const BoostInterval&) = delete;

    BoostInterval& operator=(BoostInterval&&) = delete;

    double to_float() const {
        return boost::numeric::median(interval_);
    }

    std::pair<double, double> to_floats() const {
        return std::make_pair(boost::numeric::lower(interval_), boost::numeric::upper(interval_));
    }

    static BoostInterval nan() {
        return BoostInterval(BoostIntervalType::empty());
    }

    bool is_nan() const {
        return std::isnan(boost::numeric::lower(interval_)) || std::isnan(boost::numeric::upper(interval_));
    }

    bool pos() const {
        return boost::numeric::interval_lib::cergt(interval_, 0.0);
    }

    bool neg() const {
        return boost::numeric::interval_lib::cerlt(interval_, 0.0);
    }

    bool nonz() const {
        return !is_nan() && !boost::numeric::zero_in(interval_);
    }

    bool operator>(const BoostInterval& interval) const {
        return boost::numeric::interval_lib::cergt(interval_, interval.interval_);
    }

    bool operator<(const BoostInterval& interval) const {
        return boost::numeric::interval_lib::cerlt(interval_, interval.interval_);
    }

    BoostInterval min() const {
        if(is_nan()) {
            return nan();
        }
        return BoostInterval(boost::numeric::lower(interval_));
    }

    BoostInterval max() const {
        if(is_nan()) {
            return nan();
        }
        return BoostInterval(boost::numeric::upper(interval_));
    }

    BoostInterval mid() const {
        if(is_nan()) {
            return nan();
        }
        return BoostInterval(boost::numeric::median(interval_));
    }

    BoostInterval len() const {
        if(is_nan()) {
            return nan();
        }
        return BoostInterval(boost::numeric::width(interval_));
    }

    BoostInterval rad() const {
        if(is_nan()) {
            return nan();
        }
        return BoostInterval(boost::numeric::width(interval_) / 2);
    }

    BoostInterval hull(const BoostInterval& other) const {
        return BoostInterval(boost::numeric::hull(interval_, other.interval_));
    }

    BoostInterval operator+() const {
        return BoostInterval(+interval_);
    }

    BoostInterval operator-() const {
        return BoostInterval(-interval_);
    }

    BoostInterval operator+(const BoostInterval& interval) const {
        return BoostInterval(interval_ + interval.interval_);
    }

    BoostInterval operator-(const BoostInterval& interval) const {
        return BoostInterval(interval_ - interval.interval_);
    }

    BoostInterval operator*(const BoostInterval& interval) const {
        return BoostInterval(interval_ * interval.interval_);
    }

    BoostInterval operator/(const BoostInterval& interval) const {
        if(!interval.nonz()) {
            return nan();
        }
        return BoostInterval(interval_ / interval.interval_);
    }

    BoostInterval inv() const {
        if(!nonz()) {
            return nan();
        }
        return BoostInterval(boost::numeric::interval_lib::multiplicative_inverse(interval_));
    }

    BoostInterval sqr() const {
        return BoostInterval(boost::numeric::square(interval_));
    }

    BoostInterval sqrt() const {
        if(min().neg()) {
            return nan();
        }
        return BoostInterval(boost::numeric::sqrt(interval_));
    }

    static BoostInterval pi() {
        return BoostInterval(boost::numeric::interval_lib::pi<BoostIntervalType>());
    }

    static BoostInterval tau() {
        return BoostInterval(2.0 * boost::numeric::interval_lib::pi<BoostIntervalType>());
    }

    BoostInterval cos() const {
        return BoostInterval(boost::numeric::cos(interval_));
    }

    BoostInterval sin() const {
        return BoostInterval(boost::numeric::sin(interval_));
    }

    BoostInterval tan() const {
        if(!cos().nonz()) {
            return nan();
        }
        return BoostInterval(boost::numeric::tan(interval_));
    }

    BoostInterval acos() const {
        if(!(BoostInterval(-1) < min() && max() < BoostInterval(1))) {
            return nan();
        }
        return BoostInterval(boost::numeric::acos(interval_));
    }

    BoostInterval asin() const {
        if(!(BoostInterval(-1) < min() && max() < BoostInterval(1))) {
            return nan();
        }
        return BoostInterval(boost::numeric::asin(interval_));
    }

    BoostInterval atan() const {
        return BoostInterval(boost::numeric::atan(interval_));
    }
};

static_assert(IntervalType<BoostInterval>);
