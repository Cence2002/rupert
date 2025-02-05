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

    bool operator>(const MpfrNumber& number) const {
        return mpfr_cmp(value_, number.value_) > 0;
    }

    template<IntegerType Integer>
    bool operator>(const Integer integer) const {
        return mpfr_cmp_si(value_, integer) > 0;
    }

    template<IntegerType Integer>
    friend bool operator>(const Integer integer, const MpfrNumber& number) {
        return mpfr_cmp_si(number.value_, integer) < 0;
    }

    bool operator<(const MpfrNumber& number) const {
        return mpfr_cmp(value_, number.value_) < 0;
    }

    template<IntegerType Integer>
    bool operator<(const Integer integer) const {
        return mpfr_cmp_si(value_, integer) < 0;
    }

    template<IntegerType Integer>
    friend bool operator<(const Integer integer, const MpfrNumber& number) {
        return mpfr_cmp_si(number.value_, integer) > 0;
    }

    bool is_pos() const {
        return mpfr_sgn(value_) > 0;
    }

    bool is_neg() const {
        return mpfr_sgn(value_) < 0;
    }

    bool is_nan() const {
        return mpfr_nan_p(value_);
    }

    static MpfrNumber nan() {
        MpfrNumber number;
        mpfr_set_nan(number.value_);
        return number;
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
