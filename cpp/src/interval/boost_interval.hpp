#pragma once

#include "interval/interval_type.hpp"
#include <boost/numeric/interval.hpp>

struct BoostInterval {
private:
    using BoostIntervalType = boost::numeric::interval<
        double,
        boost::numeric::interval_lib::policies<
            boost::numeric::interval_lib::save_state<
                boost::numeric::interval_lib::rounded_transc_std<double>>,
            boost::numeric::interval_lib::checking_base<double>>>;

    BoostIntervalType interval_;

    explicit BoostInterval(const BoostIntervalType& interval) : interval_(interval) {}

public:
    using Number = FloatNumber;

    template<IntegerType Integer>
    explicit BoostInterval(const Integer integer) : interval_(integer) {}

    template<IntegerType Integer>
    explicit BoostInterval(const Integer min, const Integer max) : interval_(min, max) {}

    explicit BoostInterval(const Number& number) : interval_(number.value()) {}

    explicit BoostInterval(const Number& min, const Number& max) : interval_(min.value(), max.value()) {}

    ~BoostInterval() = default;

    BoostInterval(const BoostInterval& interval) = default;

    BoostInterval(BoostInterval&& interval) = default;

    static BoostInterval nan() {
        return BoostInterval(BoostIntervalType::empty());
    }

    bool is_nan() const {
        return std::isnan(boost::numeric::lower(interval_)) || std::isnan(boost::numeric::upper(interval_));
    }

    bool is_positive() const {
        return boost::numeric::interval_lib::cergt(interval_, 0.0);
    }

    bool is_negative() const {
        return boost::numeric::interval_lib::cerlt(interval_, 0.0);
    }

    bool is_nonzero() const {
        return !is_nan() && !boost::numeric::zero_in(interval_);
    }

    bool operator>(const BoostInterval& interval) const {
        return boost::numeric::interval_lib::cergt(interval_, interval.interval_);
    }

    bool operator<(const BoostInterval& interval) const {
        return boost::numeric::interval_lib::cerlt(interval_, interval.interval_);
    }

    Number min() const {
        return Number(boost::numeric::lower(interval_));
    }

    Number max() const {
        return Number(boost::numeric::upper(interval_));
    }

    Number mid() const {
        return Number(boost::numeric::median(interval_));
    }

    Number len() const {
        return Number(boost::numeric::width(interval_));
    }

    Number rad() const {
        return Number(boost::numeric::width(interval_) / 2);
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
        if(!interval.is_nonzero()) {
            return nan();
        }
        return BoostInterval(interval_ / interval.interval_);
    }

    BoostInterval inv() const {
        if(!is_nonzero()) {
            return nan();
        }
        return BoostInterval(boost::numeric::interval_lib::multiplicative_inverse(interval_));
    }

    BoostInterval sqr() const {
        return BoostInterval(boost::numeric::square(interval_));
    }

    BoostInterval sqrt() const {
        if(min().is_negative()) {
            return nan();
        }
        return BoostInterval(boost::numeric::sqrt(interval_));
    }

    static BoostInterval pi() {
        return BoostInterval(boost::numeric::interval_lib::pi<BoostIntervalType>());
    }

    BoostInterval cos() const {
        return BoostInterval(boost::numeric::cos(interval_));
    }

    BoostInterval sin() const {
        return BoostInterval(boost::numeric::sin(interval_));
    }

    BoostInterval tan() const {
        return BoostInterval(boost::numeric::tan(interval_));
    }

    BoostInterval acos() const {
        return BoostInterval(boost::numeric::acos(interval_));
    }

    BoostInterval asin() const {
        return BoostInterval(boost::numeric::asin(interval_));
    }

    BoostInterval atan() const {
        return BoostInterval(boost::numeric::atan(interval_));
    }

    friend std::ostream& operator<<(std::ostream& os, const BoostInterval& interval);

    static inline const std::string name = "BoostInterval";
};
