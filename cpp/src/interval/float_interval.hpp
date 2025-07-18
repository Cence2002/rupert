#pragma once

#include "interval/interval_type.hpp"
#include <cmath>
#include <algorithm>

class FloatInterval {
    double min_;
    double max_;

    explicit FloatInterval(const double min, const double max) : min_(min), max_(max) {}

    explicit FloatInterval(const double value) : FloatInterval(value, value) {}

public:
    template<IntegerType Integer>
    explicit FloatInterval(const Integer value) : FloatInterval(value, value) {}

    template<IntegerType Integer>
    explicit FloatInterval(const Integer min, const Integer max) : FloatInterval(static_cast<double>(min), static_cast<double>(max)) {
        if(min > max) {
            throw std::invalid_argument("min > max");
        }
        if(min != 0) {
            min_ -= std::numeric_limits<double>::epsilon() * 42;
        }
        if(max != 0) {
            max_ += std::numeric_limits<double>::epsilon() * 42;
        }
    }

    ~FloatInterval() = default;

    FloatInterval(const FloatInterval& interval) = default;

    FloatInterval(FloatInterval&& interval) = default;

    FloatInterval& operator=(const FloatInterval&) = delete;

    FloatInterval& operator=(FloatInterval&&) = delete;

    double to_float() const {
        return (min_ + max_) / 2;
    }

    std::pair<double, double> to_floats() const {
        return std::make_pair(min_, max_);
    }

    static FloatInterval nan() {
        return FloatInterval(std::numeric_limits<double>::quiet_NaN());
    }

    bool is_nan() const {
        return std::isnan(min_) || std::isnan(max_);
    }

    bool pos() const {
        return min_ > 0;
    }

    bool neg() const {
        return max_ < 0;
    }

    bool nonz() const {
        return min_ > 0 || max_ < 0;
    }

    bool operator>(const FloatInterval& interval) const {
        return min_ > interval.max_;
    }

    bool operator<(const FloatInterval& interval) const {
        return max_ < interval.min_;
    }

    FloatInterval min() const {
        return FloatInterval(min_);
    }

    FloatInterval max() const {
        return FloatInterval(max_);
    }

    FloatInterval mid() const {
        return FloatInterval((min_ + max_) / 2);
    }

    FloatInterval len() const {
        return FloatInterval(max_ - min_);
    }

    FloatInterval rad() const {
        return FloatInterval((max_ - min_) / 2);
    }

    FloatInterval hull(const FloatInterval& other) const {
        return FloatInterval(
            std::min(min_, other.min_),
            std::max(max_, other.max_)
        );
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

    FloatInterval operator-(const FloatInterval& interval) const {
        return FloatInterval(min_ - interval.max_, max_ - interval.min_);
    }

    FloatInterval operator*(const FloatInterval& interval) const {
        const double min_min = min_ * interval.min_;
        const double min_max = min_ * interval.max_;
        const double max_min = max_ * interval.min_;
        const double max_max = max_ * interval.max_;
        return FloatInterval(
            std::min({min_min, min_max, max_min, max_max}),
            std::max({min_min, min_max, max_min, max_max})
        );
    }

    FloatInterval operator/(const FloatInterval& interval) const {
        if(!interval.nonz()) {
            return nan();
        }
        const double min_min = min_ / interval.min_;
        const double min_max = min_ / interval.max_;
        const double max_min = max_ / interval.min_;
        const double max_max = max_ / interval.max_;
        return FloatInterval(
            std::min({min_min, min_max, max_min, max_max}),
            std::max({min_min, min_max, max_min, max_max})
        );
    }

    FloatInterval inv() const {
        if(!nonz()) {
            return nan();
        }
        return FloatInterval(1 / max_, 1 / min_);
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
        if(min_ < 0) {
            return nan();
        }
        return FloatInterval(std::sqrt(min_), std::sqrt(max_));
    }

    static FloatInterval pi() {
        return FloatInterval(std::numbers::pi_v<double>);
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
        constexpr double HALF_PI = std::numbers::pi_v<double> / 2;
        constexpr double PI = std::numbers::pi_v<double>;
        if(std::floor((min_ - HALF_PI) / PI) != std::floor((max_ - HALF_PI) / PI)) {
            return nan();
        }
        return FloatInterval(std::tan(min_), std::tan(max_));
    }

    FloatInterval acos() const {
        if(is_nan()) {
            return nan();
        }
        if(!(-1 < min_ && max_ < 1)) {
            return nan();
        }
        return FloatInterval(std::acos(max_), std::acos(min_));
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
};

static_assert(IntervalType<FloatInterval>);
