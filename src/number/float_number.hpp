#pragma once

#include "number/number_type.hpp"
#include <limits>
#include <sstream>

struct FloatNumber {
private:
    double value_;

    static inline int print_precision_ = 10;

public:
    using Value = double;

    explicit FloatNumber() : value_(std::numeric_limits<double>::quiet_NaN()) {}

    template<IntegerType Integer>
    explicit FloatNumber(const Integer integer) : value_(static_cast<double>(integer)) {}

    ~FloatNumber() = default;

    FloatNumber(const FloatNumber& number) = default;

    FloatNumber& operator=(const FloatNumber& number) = default;

    FloatNumber(FloatNumber&& number) = default;

    FloatNumber& operator=(FloatNumber&& number) = default;

    double& value() {
        return value_;
    }

    const double& value() const {
        return value_;
    }

    bool operator>(const FloatNumber& number) const {
        return value_ > number.value_;
    }

    bool operator<(const FloatNumber& number) const {
        return value_ < number.value_;
    }

    static void set_print_precision(const int print_precision) {
        print_precision_ = print_precision;
    }

    friend std::ostream& operator<<(std::ostream& os, const FloatNumber& number) {
        std::ostringstream ss;
        ss.precision(print_precision_);
        ss << number.value_;
        return os << ss.str();
    }
};
