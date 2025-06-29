#pragma once

#include "interval/interval_type.hpp"
#include <mpfi.h>

struct MpfiInterval {
private:
    mpfi_t interval_;

    void assert_same_precision(const mpfi_t interval) const {
        if(mpfi_get_prec(interval_) != mpfi_get_prec(interval)) {
            throw std::invalid_argument("Precisions of intervals are different");
        }
    }

    void assert_same_precision(const MpfiInterval& interval) const {
        if(mpfi_get_prec(interval_) != mpfi_get_prec(interval.interval_)) {
            throw std::invalid_argument("Precisions of intervals are different");
        }
    }

    void assert_same_precision(const mpfr_t number) const {
        if(mpfi_get_prec(interval_) != mpfr_get_prec(number)) {
            throw std::invalid_argument("Precisions of interval and number are different");
        }
    }

    explicit MpfiInterval(const mpfr_t number) {
        mpfi_init(interval_);
        assert_same_precision(number);
        mpfi_set_fr(interval_, number);
    }

    explicit MpfiInterval(const mpfi_t interval) {
        mpfi_init(interval_);
        assert_same_precision(interval);
        mpfi_set(interval_, interval);
    }

public:
    template<IntegerType Integer>
    explicit MpfiInterval(const Integer integer) {
        mpfi_init(interval_);
        mpfi_set_si(interval_, integer);
    }

    template<IntegerType Integer>
    explicit MpfiInterval(const Integer min, const Integer max) {
        mpfi_init(interval_);
        mpfi_interv_si(interval_, min, max);
    }

    ~MpfiInterval() {
        mpfi_clear(interval_);
    }

    MpfiInterval(const MpfiInterval& interval) {
        mpfi_init(interval_);
        assert_same_precision(interval);
        mpfi_set(interval_, interval.interval_);
    }

    MpfiInterval(MpfiInterval&& interval) {
        mpfi_init(interval_);
        assert_same_precision(interval);
        mpfi_swap(interval_, interval.interval_);
    }

    static MpfiInterval nan() {
        mpfi_t interval;
        mpfi_init(interval);
        return MpfiInterval(interval);
    }

    bool is_nan() const {
        return mpfi_nan_p(interval_);
    }

    bool is_positive() const {
        return mpfi_is_strictly_pos(interval_);
    }

    bool is_negative() const {
        return mpfi_is_strictly_neg(interval_);
    }

    bool is_nonzero() const {
        return !mpfi_has_zero(interval_);
    }

    bool operator>(const MpfiInterval& interval) const {
        if(is_nan() || interval.is_nan()) {
            return false;
        }
        return mpfi_cmp(interval_, interval.interval_) > 0;
    }

    bool operator<(const MpfiInterval& interval) const {
        if(is_nan() || interval.is_nan()) {
            return false;
        }
        return mpfi_cmp(interval_, interval.interval_) < 0;
    }

    MpfiInterval min() const {
        mpfr_t min;
        mpfr_init(min);
        mpfi_get_left(min, interval_);
        return MpfiInterval(min);
    }

    MpfiInterval max() const {
        mpfr_t max;
        mpfr_init(max);
        mpfi_get_right(max, interval_);
        return MpfiInterval(max);
    }

    MpfiInterval mid() const {
        mpfr_t mid;
        mpfr_init(mid);
        mpfi_mid(mid, interval_);
        return MpfiInterval(mid);
    }

    MpfiInterval len() const {
        mpfr_t len;
        mpfr_init(len);
        mpfi_diam_abs(len, interval_);
        return MpfiInterval(len);
    }

    MpfiInterval rad() const {
        mpfr_t rad;
        mpfr_init(rad);
        mpfi_diam_abs(rad, interval_);
        mpfr_div_ui(rad, rad, 2, MPFR_RNDU);
        return MpfiInterval(rad);
    }

    MpfiInterval hull(const MpfiInterval& other) const {
        mpfi_t uni;
        mpfi_init(uni);
        mpfi_union(uni, interval_, other.interval_);
        return MpfiInterval(uni);
    }

    MpfiInterval operator+() const {
        return MpfiInterval(interval_);
    }

    MpfiInterval operator-() const {
        mpfi_t neg;
        mpfi_init(neg);
        mpfi_neg(neg, interval_);
        return MpfiInterval(neg);
    }

    MpfiInterval operator+(const MpfiInterval& interval) const {
        mpfi_t add;
        mpfi_init(add);
        mpfi_add(add, interval_, interval.interval_);
        return MpfiInterval(add);
    }

    MpfiInterval operator-(const MpfiInterval& interval) const {
        mpfi_t sub;
        mpfi_init(sub);
        mpfi_sub(sub, interval_, interval.interval_);
        return MpfiInterval(sub);
    }

    MpfiInterval operator*(const MpfiInterval& interval) const {
        mpfi_t mul;
        mpfi_init(mul);
        mpfi_mul(mul, interval_, interval.interval_);
        return MpfiInterval(mul);
    }

    MpfiInterval operator/(const MpfiInterval& interval) const {
        if(!interval.is_nonzero()) {
            return nan();
        }
        mpfi_t div;
        mpfi_init(div);
        mpfi_div(div, interval_, interval.interval_);
        return MpfiInterval(div);
    }

    MpfiInterval inv() const {
        if(!is_nonzero()) {
            return nan();
        }
        mpfi_t inv;
        mpfi_init(inv);
        mpfi_inv(inv, interval_);
        return MpfiInterval(inv);
    }

    MpfiInterval sqr() const {
        mpfi_t sqr;
        mpfi_init(sqr);
        mpfi_sqr(sqr, interval_);
        return MpfiInterval(sqr);
    }

    MpfiInterval sqrt() const {
        if(min().is_negative()) {
            return nan();
        }
        mpfi_t sqrt;
        mpfi_init(sqrt);
        mpfi_sqrt(sqrt, interval_);
        return MpfiInterval(sqrt);
    }

    static MpfiInterval pi() {
        mpfi_t pi;
        mpfi_init(pi);
        mpfi_const_pi(pi);
        return MpfiInterval(pi);
    }

    MpfiInterval cos() const {
        mpfi_t cos;
        mpfi_init(cos);
        mpfi_cos(cos, interval_);
        return MpfiInterval(cos);
    }

    MpfiInterval sin() const {
        mpfi_t sin;
        mpfi_init(sin);
        mpfi_sin(sin, interval_);
        return MpfiInterval(sin);
    }

    MpfiInterval tan() const {
        mpfi_t tan;
        mpfi_init(tan);
        mpfi_tan(tan, interval_);
        return MpfiInterval(tan);
    }

    MpfiInterval acos() const {
        mpfi_t acos;
        mpfi_init(acos);
        mpfi_acos(acos, interval_);
        return MpfiInterval(acos);
    }

    MpfiInterval asin() const {
        mpfi_t asin;
        mpfi_init(asin);
        mpfi_asin(asin, interval_);
        return MpfiInterval(asin);
    }

    MpfiInterval atan() const {
        mpfi_t atan;
        mpfi_init(atan);
        mpfi_atan(atan, interval_);
        return MpfiInterval(atan);
    }

    double to_float() const {
        mpfr_t mid;
        mpfr_init2(mid, mpfi_get_prec(interval_));
        mpfi_mid(mid, interval_);
        const double mid_float = mpfr_get_d(mid, MPFR_RNDU);
        mpfr_clear(mid);
        return mid_float;
    }

    static inline const std::string name = "MpfiInterval";
};
