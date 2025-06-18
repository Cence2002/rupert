#pragma once

#include "number/number_type.hpp"

struct MpfrNumber {
private:
    mpfr_t value_;

    void assert_same_precision(const mpfr_t value) const {
        if(mpfr_get_prec(value_) != mpfr_get_prec(value)) {
            throw std::invalid_argument("Precisions of numbers are different");
        }
    }

    void assert_same_precision(const MpfrNumber& number) const {
        if(mpfr_get_prec(value_) != mpfr_get_prec(number.value_)) {
            throw std::invalid_argument("Precisions of numbers are different");
        }
    }

    explicit MpfrNumber(const mpfr_t value) {
        mpfr_init(value_);
        assert_same_precision(value);
        mpfr_set(value_, value, MPFR_RNDN);
    }

public:
    using Value = mpfr_t;

    template<IntegerType Integer>
    explicit MpfrNumber(const Integer integer) {
        mpfr_init(value_);
        mpfr_set_si(value_, integer, MPFR_RNDN);
    }

    ~MpfrNumber() {
        mpfr_clear(value_);
    }

    MpfrNumber(const MpfrNumber& number) {
        mpfr_init(value_);
        assert_same_precision(number);
        mpfr_set(value_, number.value_, MPFR_RNDN);
    }

    MpfrNumber(MpfrNumber&& number) {
        mpfr_init(value_);
        assert_same_precision(number);
        mpfr_swap(value_, number.value_);
    }

    const mpfr_t& value() const {
        return value_;
    }

    static MpfrNumber nan() {
        mpfr_t value;
        mpfr_init(value);
        mpfr_set_nan(value);
        return MpfrNumber(value);
    }

    double float_value() const {
        return mpfr_get_d(value_, MPFR_RNDN);
    }

    bool is_nan() const {
        return mpfr_nan_p(value_);
    }

    bool is_positive() const {
        if(is_nan()) {
            return false;
        }
        return mpfr_sgn(value_) > 0;
    }

    bool is_negative() const {
        if(is_nan()) {
            return false;
        }
        return mpfr_sgn(value_) < 0;
    }

    bool is_nonzero() const {
        if(is_nan()) {
            return false;
        }
        return mpfr_zero_p(value_) == 0;
    }

    bool operator>(const MpfrNumber& number) const {
        if(is_nan() || number.is_nan()) {
            return false;
        }
        return mpfr_cmp(value_, number.value_) > 0;
    }

    bool operator<(const MpfrNumber& number) const {
        if(is_nan() || number.is_nan()) {
            return false;
        }
        return mpfr_cmp(value_, number.value_) < 0;
    }

    friend std::ostream& operator<<(std::ostream& ostream, const MpfrNumber& number) {
        char *number_str = new char[number_print_precision + 16];
        mpfr_sprintf(number_str, "%.*Rg", number_print_precision, number.value_);
        return ostream << number_str;
    }

    static inline const std::string name = "MpfrNumber";

    friend struct MpfiInterval;
};
