#pragma once

#include "interval/interval_type.hpp"
#include <algorithm>

struct FloatInterval {
private:
    double min_;
    double max_;

    static inline IntervalPrintMode print_mode_ = IntervalPrintMode::min_and_max;

    explicit FloatInterval(const double min, const double max) : min_(min), max_(max) {}

public:
    using Number = FloatNumber;

    explicit FloatInterval() : FloatInterval(nan()) {}

    explicit FloatInterval(const Number& number) : min_(number.value()), max_(number.value()) {}

    explicit FloatInterval(const Number& min, const Number& max) : min_(min.value()), max_(max.value()) {}

    template<IntegerType Integer>
    explicit FloatInterval(const Integer integer) : min_(static_cast<double>(integer)), max_(static_cast<double>(integer)) {}

    template<IntegerType Integer>
    explicit FloatInterval(const Integer min, const Integer max) : min_(static_cast<double>(min)), max_(static_cast<double>(max)) {}

    ~FloatInterval() = default;

    FloatInterval(const FloatInterval& interval) = default;

    FloatInterval& operator=(const FloatInterval& interval) = default;

    FloatInterval(FloatInterval&& interval) = default;

    FloatInterval& operator=(FloatInterval&& interval) = default;

    bool is_nan() const {
        return std::isnan(min_) || std::isnan(max_);
    }

    bool is_positive() const {
        return min_ > 0;
    }

    bool is_negative() const {
        return max_ < 0;
    }

    bool is_nonzero() const {
        return min_ > 0 || max_ < 0;
    }

    bool operator>(const FloatInterval& interval) const {
        return min_ > interval.max_;
    }

    bool operator>(const Number& number) const {
        return min_ > number.value();
    }

    friend bool operator>(const Number& number, const FloatInterval& interval) {
        return number.value() > interval.max_;
    }

    template<IntegerType Integer>
    bool operator>(const Integer integer) const {
        return min_ > static_cast<double>(integer);
    }

    template<IntegerType Integer>
    friend bool operator>(const Integer integer, const FloatInterval& interval) {
        return static_cast<double>(integer) > interval.max_;
    }

    bool operator<(const FloatInterval& interval) const {
        return max_ < interval.min_;
    }

    bool operator<(const Number& number) const {
        return max_ < number.value();
    }

    friend bool operator<(const Number& number, const FloatInterval& interval) {
        return number.value() < interval.min_;
    }

    template<IntegerType Integer>
    bool operator<(const Integer integer) const {
        return max_ < static_cast<double>(integer);
    }

    template<IntegerType Integer>
    friend bool operator<(const Integer integer, const FloatInterval& interval) {
        return static_cast<double>(integer) < interval.min_;
    }

    Number min() const {
        Number min;
        min.value() = min_;
        return min;
    }

    Number max() const {
        Number max;
        max.value() = max_;
        return max;
    }

    Number mid() const {
        Number mid;
        mid.value() = (min_ + max_) / 2;
        return mid;
    }

    Number len() const {
        Number len;
        len.value() = max_ - min_;
        return len;
    }

    Number rad() const {
        Number rad;
        rad.value() = (max_ - min_) / 2;
        return rad;
    }

    FloatInterval operator+() const {
        return FloatInterval(+min_, +max_);
    }

    FloatInterval operator-() const {
        return FloatInterval(-max_, -min_);
    }

    FloatInterval operator+(const FloatInterval& interval) const {
        return FloatInterval(min_ + interval.min_, max_ + interval.max_);
    }

    FloatInterval operator+(const Number& number) const {
        return FloatInterval(min_ + number.value(), max_ + number.value());
    }

    friend FloatInterval operator+(const Number& number, const FloatInterval& interval) {
        return FloatInterval(number.value() + interval.min_, number.value() + interval.max_);
    }

    template<IntegerType Integer>
    FloatInterval operator+(const Integer integer) const {
        return FloatInterval(min_ + static_cast<double>(integer), max_ + static_cast<double>(integer));
    }

    template<IntegerType Integer>
    friend FloatInterval operator+(const Integer integer, const FloatInterval& interval) {
        return FloatInterval(static_cast<double>(integer) + interval.min_, static_cast<double>(integer) + interval.max_);
    }

    FloatInterval operator-(const FloatInterval& interval) const {
        return FloatInterval(min_ - interval.max_, max_ - interval.min_);
    }

    FloatInterval operator-(const Number& number) const {
        return FloatInterval(min_ - number.value(), max_ - number.value());
    }

    friend FloatInterval operator-(const Number& number, const FloatInterval& interval) {
        return FloatInterval(number.value() - interval.max_, number.value() - interval.min_);
    }

    template<IntegerType Integer>
    FloatInterval operator-(const Integer integer) const {
        return FloatInterval(min_ - static_cast<double>(integer), max_ - static_cast<double>(integer));
    }

    template<IntegerType Integer>
    friend FloatInterval operator-(const Integer integer, const FloatInterval& interval) {
        return FloatInterval(static_cast<double>(integer) - interval.max_, static_cast<double>(integer) - interval.min_);
    }

    FloatInterval operator*(const FloatInterval& interval) const {
        const double min_min = min_ * interval.min_;
        const double min_max = min_ * interval.max_;
        const double max_min = max_ * interval.min_;
        const double max_max = max_ * interval.max_;
        return FloatInterval(std::min({min_min, min_max, max_min, max_max}),
                             std::max({min_min, min_max, max_min, max_max}));
    }

    FloatInterval operator*(const Number& number) const {
        const double min_n = min_ * number.value();
        const double max_n = max_ * number.value();
        return FloatInterval(std::min(min_n, max_n), std::max(min_n, max_n));
    }

    friend FloatInterval operator*(const Number& number, const FloatInterval& interval) {
        const double min = number.value() * interval.min_;
        const double max = number.value() * interval.max_;
        return FloatInterval(std::min(min, max), std::max(min, max));
    }

    template<IntegerType Integer>
    FloatInterval operator*(const Integer integer) const {
        const double min_n = min_ * static_cast<double>(integer);
        const double max_n = max_ * static_cast<double>(integer);
        return FloatInterval(std::min(min_n, max_n), std::max(min_n, max_n));
    }

    template<IntegerType Integer>
    friend FloatInterval operator*(const Integer integer, const FloatInterval& interval) {
        const double min = static_cast<double>(integer) * interval.min_;
        const double max = static_cast<double>(integer) * interval.max_;
        return FloatInterval(std::min(min, max), std::max(min, max));
    }

    FloatInterval operator/(const FloatInterval& interval) const {
        if(!interval.is_nonzero()) {
            return nan();
        }
        const double min_min = min_ / interval.min_;
        const double min_max = min_ / interval.max_;
        const double max_min = max_ / interval.min_;
        const double max_max = max_ / interval.max_;
        return FloatInterval(std::min({min_min, min_max, max_min, max_max}),
                             std::max({min_min, min_max, max_min, max_max}));
    }

    FloatInterval operator/(const Number& number) const {
        if(!number.is_nonzero()) {
            return nan();
        }
        const double min_n = min_ / number.value();
        const double max_n = max_ / number.value();
        return FloatInterval(std::min(min_n, max_n), std::max(min_n, max_n));
    }

    friend FloatInterval operator/(const Number& number, const FloatInterval& interval) {
        if(!interval.is_nonzero()) {
            return nan();
        }
        const double min = number.value() / interval.min_;
        const double max = number.value() / interval.max_;
        return FloatInterval(std::min(min, max), std::max(min, max));
    }

    template<IntegerType Integer>
    FloatInterval operator/(const Integer integer) const {
        if(integer == 0) {
            return nan();
        }
        const double min_n = min_ / static_cast<double>(integer);
        const double max_n = max_ / static_cast<double>(integer);
        return FloatInterval(std::min(min_n, max_n), std::max(min_n, max_n));
    }

    template<IntegerType Integer>
    friend FloatInterval operator/(const Integer integer, const FloatInterval& interval) {
        if(!interval.is_nonzero()) {
            return nan();
        }
        const double min = static_cast<double>(integer) / interval.min_;
        const double max = static_cast<double>(integer) / interval.max_;
        return FloatInterval(std::min(min, max), std::max(min, max));
    }

    FloatInterval& operator+=(const FloatInterval& interval) {
        min_ += interval.min_;
        max_ += interval.max_;
        return *this;
    }

    FloatInterval& operator+=(const Number& number) {
        min_ += number.value();
        max_ += number.value();
        return *this;
    }

    template<IntegerType Integer>
    FloatInterval& operator+=(const Integer integer) {
        min_ += static_cast<double>(integer);
        max_ += static_cast<double>(integer);
        return *this;
    }

    FloatInterval& operator-=(const FloatInterval& interval) {
        min_ -= interval.max_;
        max_ -= interval.min_;
        return *this;
    }

    FloatInterval& operator-=(const Number& number) {
        min_ -= number.value();
        max_ -= number.value();
        return *this;
    }

    template<IntegerType Integer>
    FloatInterval& operator-=(const Integer integer) {
        min_ -= static_cast<double>(integer);
        max_ -= static_cast<double>(integer);
        return *this;
    }

    FloatInterval& operator*=(const FloatInterval& interval) {
        const double min_min = min_ * interval.min_;
        const double min_max = min_ * interval.max_;
        const double max_min = max_ * interval.min_;
        const double max_max = max_ * interval.max_;
        min_ = std::min({min_min, min_max, max_min, max_max});
        max_ = std::max({min_min, min_max, max_min, max_max});
        return *this;
    }

    FloatInterval& operator*=(const Number& number) {
        const double min_n = min_ * number.value();
        const double max_n = max_ * number.value();
        min_ = std::min(min_n, max_n);
        max_ = std::max(min_n, max_n);
        return *this;
    }

    template<IntegerType Integer>
    FloatInterval& operator*=(const Integer integer) {
        const double min_n = min_ * static_cast<double>(integer);
        const double max_n = max_ * static_cast<double>(integer);
        min_ = std::min(min_n, max_n);
        max_ = std::max(min_n, max_n);
        return *this;
    }

    FloatInterval& operator/=(const FloatInterval& interval) {
        if(!interval.is_nonzero()) {
            return *this = nan();
        }
        const double min_min = min_ / interval.min_;
        const double min_max = min_ / interval.max_;
        const double max_min = max_ / interval.min_;
        const double max_max = max_ / interval.max_;
        min_ = std::min({min_min, min_max, max_min, max_max});
        max_ = std::max({min_min, min_max, max_min, max_max});
        return *this;
    }

    FloatInterval& operator/=(const Number& number) {
        if(!number.is_nonzero()) {
            return *this = nan();
        }
        const double min_n = min_ / number.value();
        const double max_n = max_ / number.value();
        min_ = std::min(min_n, max_n);
        max_ = std::max(min_n, max_n);
        return *this;
    }

    template<IntegerType Integer>
    FloatInterval& operator/=(const Integer integer) {
        if(integer == 0) {
            return *this = nan();
        }
        const double min_n = min_ / static_cast<double>(integer);
        const double max_n = max_ / static_cast<double>(integer);
        min_ = std::min(min_n, max_n);
        max_ = std::max(min_n, max_n);
        return *this;
    }

    FloatInterval inv() const {
        if(!is_nonzero()) {
            return nan();
        }
        return FloatInterval(1.0 / max_, 1.0 / min_);
    }

    FloatInterval sqr() const {
        if(is_nan()) {
            return nan();
        }
        if(0 <= min_) {
            return FloatInterval(min_ * min_, max_ * max_);
        }
        if(max_ <= 0) {
            return FloatInterval(max_ * max_, min_ * min_);
        }
        return FloatInterval(0, std::max(min_ * min_, max_ * max_));
    }

    FloatInterval sqrt() const {
        if(min().is_negative()) {
            return nan();
        }
        return FloatInterval(std::sqrt(min_), std::sqrt(max_));
    }

    FloatInterval cos() const {
        if(is_nan()) {
            return nan();
        }
        constexpr double PI = std::numbers::pi_v<double>;
        constexpr double TWO_PI = 2 * std::numbers::pi_v<double>;
        const double cos_min = std::cos(min_);
        const double cos_max = std::cos(max_);
        double min = std::min(cos_min, cos_max);
        double max = std::max(cos_min, cos_max);
        if(std::floor(min_ / TWO_PI) != std::floor(max_ / TWO_PI)) {
            max = 1.0;
        }
        if(std::floor((min_ - PI) / TWO_PI) != std::floor((max_ - PI) / TWO_PI)) {
            min = -1.0;
        }
        return FloatInterval(min, max);
    }

    FloatInterval sin() const {
        if(is_nan()) {
            return nan();
        }
        constexpr double TWO_PI = 2 * std::numbers::pi_v<double>;
        constexpr double HALF_PI = std::numbers::pi_v<double> / 2;
        const double sin_min = std::sin(min_);
        const double sin_max = std::sin(max_);
        double min = std::min(sin_min, sin_max);
        double max = std::max(sin_min, sin_max);
        if(std::floor((min_ - HALF_PI) / TWO_PI) < std::floor((max_ - HALF_PI) / TWO_PI)) {
            max = 1.0;
        }
        if(std::floor((min_ + HALF_PI) / TWO_PI) < std::floor((max_ + HALF_PI) / TWO_PI)) {
            min = -1.0;
        }
        return FloatInterval(min, max);
    }

    FloatInterval tan() const {
        if(is_nan()) {
            return nan();
        }
        constexpr double PI = std::numbers::pi_v<double>;
        if(std::floor(min_ / PI) != std::floor(max_ / PI)) {
            return nan();
        }
        return FloatInterval(std::tan(min_), std::tan(max_));
    }

    FloatInterval acos() const {
        if(is_nan()) {
            return nan();
        }
        if(min_ < -1 || 1 < max_) {
            return nan();
        }
        return FloatInterval(std::acos(min_), std::acos(max_));
    }

    FloatInterval asin() const {
        if(is_nan()) {
            return nan();
        }
        if(min_ < -1 || 1 < max_) {
            return nan();
        }
        return FloatInterval(std::asin(min_), std::asin(max_));
    }

    FloatInterval atan() const {
        if(is_nan()) {
            return nan();
        }
        return FloatInterval(std::atan(min_), std::atan(max_));
    }

    static FloatInterval nan() {
        return FloatInterval(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN());
    }

    static FloatInterval unit() {
        return FloatInterval(0, 1);
    }

    static FloatInterval pi() {
        constexpr double PI = std::numbers::pi_v<double>;
        return FloatInterval(PI, PI);
    }

    static void set_print_precision(const size_t print_precision) {
        Number::set_print_precision(print_precision);
    }

    static void set_print_mode(const IntervalPrintMode print_mode) {
        print_mode_ = print_mode;
    }

    friend std::ostream& operator<<(std::ostream& ostream, const FloatInterval& interval) {
        switch(print_mode_) {
            case IntervalPrintMode::min_and_max: {
                return ostream << "[" << interval.min().value() << " : " << interval.max().value() << "]";
            }
            case IntervalPrintMode::mid_and_rad: {
                return ostream << "[" << interval.mid().value() << " ~ " << interval.rad().value() << "]";
            }
            default: {
                return ostream;
            }
        }
    }

    static inline const std::string name = "FloatInterval";
};
