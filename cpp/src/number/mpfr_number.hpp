#pragma once

#include "number/number_type.hpp"
#include <mpfr.h>

struct MpfrNumber {
private:
    mpfr_t value_;

    static inline size_t print_precision_ = 6;

    void assert_same_precision(const MpfrNumber& number) const {
        if(mpfr_get_prec(value_) != mpfr_get_prec(number.value_)) {
            throw std::invalid_argument("Precisions of numbers are different");
        }
    }

public:
    static inline const std::string name = "MpfrNumber";

    using Value = mpfr_t;

    explicit MpfrNumber() {
        mpfr_init(value_);
    }

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

    MpfrNumber& operator=(const MpfrNumber& number) {
        if(this != &number) {
            assert_same_precision(number);
            mpfr_set(value_, number.value_, MPFR_RNDN);
        }
        return *this;
    }

    MpfrNumber(MpfrNumber&& number) {
        mpfr_init(value_);
        assert_same_precision(number);
        mpfr_swap(value_, number.value_);
    }

    MpfrNumber& operator=(MpfrNumber&& number) {
        if(this != &number) {
            mpfr_clear(value_);
            mpfr_init(value_);
            assert_same_precision(number);
            mpfr_swap(value_, number.value_);
        }
        return *this;
    }

    mpfr_t& value() {
        return value_;
    }

    const mpfr_t& value() const {
        return value_;
    }

    double float_value() const {
        return mpfr_get_d(value_, MPFR_RNDN);
    }

    bool is_positive() const {
        return mpfr_sgn(value_) > 0;
    }

    bool is_negative() const {
        return mpfr_sgn(value_) < 0;
    }

    bool is_nonzero() const {
        return mpfr_zero_p(value_) == 0;
    }

    bool is_nan() const {
        return mpfr_nan_p(value_);
    }

    static MpfrNumber nan() {
        MpfrNumber number;
        mpfr_set_nan(number.value_);
        return number;
    }

    static void set_print_precision(const size_t print_precision) {
        print_precision_ = print_precision;
    }

    friend std::ostream& operator<<(std::ostream& ostream, const MpfrNumber& number) {
        char* number_str = new char[print_precision_ + 10];
        mpfr_sprintf(number_str, "%.*Rg", print_precision_, number.value());
        return ostream << number_str;
    }
};
