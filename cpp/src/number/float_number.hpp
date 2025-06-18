#pragma once

#include "number/number_type.hpp"
#include <cmath>
#include <sstream>

struct FloatNumber {
private:
    double value_;

    explicit FloatNumber(const double value) : value_(value) {}

public:
    using Value = double;

    template<IntegerType Integer>
    explicit FloatNumber(const Integer integer) : value_(static_cast<double>(integer)) {}

    ~FloatNumber() = default;

    FloatNumber(const FloatNumber& number) = default;

    FloatNumber(FloatNumber&& number) = default;

    const double& value() const {
        return value_;
    }

    double float_value() const {
        return value_;
    }

    static FloatNumber nan() {
        return FloatNumber(std::numeric_limits<double>::quiet_NaN());
    }

    bool is_nan() const {
        return std::isnan(value_);
    }

    bool is_positive() const {
        return value_ > 0;
    }

    bool is_negative() const {
        return value_ < 0;
    }

    bool is_nonzero() const {
        return value_ != 0;
    }

    bool operator>(const FloatNumber& number) const {
        return value_ > number.value_;
    }

    bool operator<(const FloatNumber& number) const {
        return value_ < number.value_;
    }

    friend std::ostream& operator<<(std::ostream& ostream, const FloatNumber& number) {
        std::ostringstream number_str;
        number_str.precision(number_print_precision);
        number_str << number.value_;
        return ostream << number_str.str();
    }

    static inline const std::string name = "FloatNumber";

    friend struct FloatInterval;
    friend struct BoostInterval;
};
