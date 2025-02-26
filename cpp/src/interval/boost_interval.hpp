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

    static inline IntervalPrintMode print_mode_ = IntervalPrintMode::MIN_AND_MAX;

    explicit BoostInterval(const BoostIntervalType& interval) : interval_(interval) {}

public:
    static inline const std::string name = "BoostInterval";

    using Number = FloatNumber;

    explicit BoostInterval() : BoostInterval(nan()) {}

    explicit BoostInterval(const Number& number) : interval_(number.value()) {}

    explicit BoostInterval(const Number& min, const Number& max) : interval_(min.value(), max.value()) {}

    template<IntegerType Integer>
    explicit BoostInterval(const Integer integer) : interval_(integer) {}

    template<IntegerType Integer>
    explicit BoostInterval(const Integer min, const Integer max) : interval_(min, max) {}

    ~BoostInterval() = default;

    BoostInterval(const BoostInterval& interval) = default;

    BoostInterval& operator=(const BoostInterval& interval) = default;

    BoostInterval(BoostInterval&& interval) = default;

    BoostInterval& operator=(BoostInterval&& interval) = default;

    Number min() const {
        Number min;
        min.value() = boost::numeric::lower(interval_);
        return min;
    }

    Number max() const {
        Number max;
        max.value() = boost::numeric::upper(interval_);
        return max;
    }

    Number mid() const {
        Number mid;
        mid.value() = boost::numeric::median(interval_);
        return mid;
    }

    Number len() const {
        if(is_nan()) {
            return Number::nan();
        }
        Number len;
        len.value() = boost::numeric::width(interval_);
        return len;
    }

    Number rad() const {
        if(is_nan()) {
            return Number::nan();
        }
        Number rad;
        rad.value() = boost::numeric::width(interval_) / 2;
        return rad;
    }

    bool is_pos() const {
        return boost::numeric::interval_lib::cergt(interval_, 0.0);
    }

    bool is_neg() const {
        return boost::numeric::interval_lib::cerlt(interval_, 0.0);
    }

    bool is_nan() const {
        return std::isnan(boost::numeric::lower(interval_)) || std::isnan(boost::numeric::upper(interval_));
    }

    bool operator>(const BoostInterval& interval) const {
        return boost::numeric::interval_lib::cergt(interval_, interval.interval_);
    }

    bool operator>(const Number& number) const {
        return boost::numeric::interval_lib::cergt(interval_, number.value());
    }

    friend bool operator>(const Number& number, const BoostInterval& interval) {
        return boost::numeric::interval_lib::cergt(number.value(), interval.interval_);
    }

    template<IntegerType Integer>
    bool operator>(const Integer integer) const {
        return boost::numeric::interval_lib::cergt(interval_, static_cast<double>(integer));
    }

    template<IntegerType Integer>
    friend bool operator>(const Integer integer, const BoostInterval& interval) {
        return boost::numeric::interval_lib::cergt(static_cast<double>(integer), interval.interval_);
    }

    bool operator<(const BoostInterval& interval) const {
        return boost::numeric::interval_lib::cerlt(interval_, interval.interval_);
    }

    bool operator<(const Number& number) const {
        return boost::numeric::interval_lib::cerlt(interval_, number.value());
    }

    friend bool operator<(const Number& number, const BoostInterval& interval) {
        return boost::numeric::interval_lib::cerlt(number.value(), interval.interval_);
    }

    template<IntegerType Integer>
    bool operator<(const Integer integer) const {
        return boost::numeric::interval_lib::cerlt(interval_, static_cast<double>(integer));
    }

    template<IntegerType Integer>
    friend bool operator<(const Integer n, const BoostInterval& interval) {
        return boost::numeric::interval_lib::cerlt(static_cast<double>(n), interval.interval_);
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

    BoostInterval operator+(const Number& number) const {
        return BoostInterval(interval_ + number.value());
    }

    friend BoostInterval operator+(const Number& number, const BoostInterval& interval) {
        return BoostInterval(number.value() + interval.interval_);
    }

    template<IntegerType Integer>
    BoostInterval operator+(const Integer integer) const {
        return BoostInterval(interval_ + static_cast<double>(integer));
    }

    template<IntegerType Integer>
    friend BoostInterval operator+(const Integer integer, const BoostInterval& interval) {
        return BoostInterval(static_cast<double>(integer) + interval.interval_);
    }

    BoostInterval operator-(const BoostInterval& interval) const {
        return BoostInterval(interval_ - interval.interval_);
    }

    BoostInterval operator-(const Number& number) const {
        return BoostInterval(interval_ - number.value());
    }

    friend BoostInterval operator-(const Number& number, const BoostInterval& interval) {
        return BoostInterval(number.value() - interval.interval_);
    }

    template<IntegerType Integer>
    BoostInterval operator-(const Integer integer) const {
        return BoostInterval(interval_ - static_cast<double>(integer));
    }

    template<IntegerType Integer>
    friend BoostInterval operator-(const Integer integer, const BoostInterval& interval) {
        return BoostInterval(static_cast<double>(integer) - interval.interval_);
    }

    BoostInterval operator*(const BoostInterval& interval) const {
        return BoostInterval(interval_ * interval.interval_);
    }

    BoostInterval operator*(const Number& number) const {
        return BoostInterval(interval_ * number.value());
    }

    friend BoostInterval operator*(const Number& number, const BoostInterval& interval) {
        return BoostInterval(number.value() * interval.interval_);
    }

    template<IntegerType Integer>
    BoostInterval operator*(const Integer integer) const {
        return BoostInterval(interval_ * static_cast<double>(integer));
    }

    template<IntegerType Integer>
    friend BoostInterval operator*(const Integer integer, const BoostInterval& interval) {
        return BoostInterval(static_cast<double>(integer) * interval.interval_);
    }

    BoostInterval operator/(const BoostInterval& interval) const {
        if(!interval.is_pos() && !interval.is_neg()) {
            return nan();
        }
        return BoostInterval(interval_ / interval.interval_);
    }

    BoostInterval operator/(const Number& number) const {
        if(!number.is_pos() && !number.is_neg()) {
            return nan();
        }
        return BoostInterval(interval_ / number.value());
    }

    friend BoostInterval operator/(const Number& number, const BoostInterval& interval) {
        if(!interval.is_pos() && !interval.is_neg()) {
            return nan();
        }
        return BoostInterval(number.value() / interval.interval_);
    }

    template<IntegerType Integer>
    BoostInterval operator/(const Integer integer) const {
        if(integer == 0) {
            return nan();
        }
        return BoostInterval(interval_ / static_cast<double>(integer));
    }

    template<IntegerType Integer>
    friend BoostInterval operator/(const Integer integer, const BoostInterval& interval) {
        if(!interval.is_pos() && !interval.is_neg()) {
            return nan();
        }
        return BoostInterval(static_cast<double>(integer) / interval.interval_);
    }

    BoostInterval inv() const {
        if(!is_pos() && !is_neg()) {
            return nan();
        }
        return BoostInterval(boost::numeric::interval_lib::multiplicative_inverse(interval_));
    }

    BoostInterval sqr() const {
        return BoostInterval(boost::numeric::square(interval_));
    }

    BoostInterval sqrt() const {
        if(min() < 0) {
            return nan();
        }
        return BoostInterval(boost::numeric::sqrt(interval_));
    }

    BoostInterval cos() const {
        return BoostInterval(boost::numeric::cos(interval_));
    }

    BoostInterval sin() const {
        return BoostInterval(boost::numeric::sin(interval_));
    }

    BoostInterval atan() const {
        return BoostInterval(boost::numeric::atan(interval_));
    }

    static BoostInterval nan() {
        return BoostInterval(BoostIntervalType::empty());
    }

    static BoostInterval unit() {
        return BoostInterval(0, 1);
    }

    static BoostInterval pi() {
        return BoostInterval(boost::numeric::interval_lib::pi<BoostIntervalType>());
    }

    static void set_print_precision(const int print_precision) {
        Number::set_print_precision(print_precision);
    }

    static void set_print_mode(const IntervalPrintMode print_mode) {
        print_mode_ = print_mode;
    }

    friend std::ostream& operator<<(std::ostream& ostream, const BoostInterval& interval) {
        switch(print_mode_) {
            case IntervalPrintMode::MIN_AND_MAX: {
                return ostream << "[" << interval.min().value() << " : " << interval.max().value() << "]";
            }
            case IntervalPrintMode::MID_AND_RAD: {
                return ostream << "[" << interval.mid().value() << " ~ " << interval.rad().value() << "]";
            }
            default: throw std::invalid_argument("Invalid print type");
        }
    }
};
