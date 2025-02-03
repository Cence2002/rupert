#pragma once

#include "number/number_type.hpp"
#include <mpfr.h>
#include <vector>

struct MpfrNumber {
private:
    mpfr_t value_;

    static inline int print_precision_ = 10;

    void assert_same_precision(const MpfrNumber& number) const {
        if(mpfr_get_prec(value_) != mpfr_get_prec(number.value_)) {
            throw std::invalid_argument("Precisions of numbers are different");
        }
    }

public:
    using Value = mpfr_t;

    explicit MpfrNumber() {
        mpfr_init(value_);
    }

    explicit MpfrNumber(const int value) {
        mpfr_init(value_);
        mpfr_set_si(value_, value, MPFR_RNDN);
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

    static void set_print_precision(const int print_precision) {
        print_precision_ = print_precision;
    }

    friend std::ostream& operator<<(std::ostream& os, const MpfrNumber& number) {
        std::vector<char> str(static_cast<size_t>(print_precision_ + 10));
        mpfr_sprintf(str.data(), "%.*Rg", print_precision_, number.value());
        return os << str.data();
    }
};
