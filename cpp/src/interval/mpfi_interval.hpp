#pragma once

#include "interval/interval_type.hpp"
#include <mpfi.h>

struct MpfiInterval {
private:
    mpfi_t interval_;

    static inline IntervalPrintMode print_mode_ = IntervalPrintMode::MIN_AND_MAX;

    void assert_same_precision(const MpfiInterval& interval) const {
        if(mpfi_get_prec(interval_) != mpfi_get_prec(interval.interval_)) {
            throw std::invalid_argument("Precisions of intervals are different");
        }
    }

    void assert_same_precision(const MpfrNumber& number) const {
        if(mpfi_get_prec(interval_) != mpfr_get_prec(number.value())) {
            throw std::invalid_argument("Precisions of interval and number are different");
        }
    }

public:
    static inline const std::string name = "MpfiInterval";

    using Number = MpfrNumber;

    explicit MpfiInterval() {
        mpfi_init(interval_);
    }

    explicit MpfiInterval(const Number& number) {
        mpfi_init(interval_);
        assert_same_precision(number);
        mpfi_set_fr(interval_, number.value());
    }

    explicit MpfiInterval(const Number& min, const Number& max) {
        mpfi_init(interval_);
        assert_same_precision(min);
        assert_same_precision(max);
        mpfi_interv_fr(interval_, min.value(), max.value());
    }

    template<IntegerType Integer>
    explicit MpfiInterval(const Integer integer) {
        mpfi_init(interval_);
        assert_same_precision(integer);
        mpfi_set_si(interval_, integer);
    }

    template<IntegerType Integer>
    explicit MpfiInterval(const Integer min, const Integer max) {
        mpfi_init(interval_);
        assert_same_precision(min);
        assert_same_precision(max);
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

    MpfiInterval& operator=(const MpfiInterval& interval) {
        if(this != &interval) {
            assert_same_precision(interval);
            mpfi_set(interval_, interval.interval_);
        }
        return *this;
    }

    MpfiInterval(MpfiInterval&& interval) noexcept {
        mpfi_init(interval_);
        assert_same_precision(interval);
        mpfi_swap(interval_, interval.interval_);
    }

    MpfiInterval& operator=(MpfiInterval&& interval) noexcept {
        if(this != &interval) {
            mpfi_clear(interval_);
            mpfi_init(interval_);
            assert_same_precision(interval);
            mpfi_swap(interval_, interval.interval_);
        }
        return *this;
    }

    Number min() const {
        Number min;
        mpfi_get_left(min.value(), interval_);
        return min;
    }

    Number max() const {
        Number max;
        mpfi_get_right(max.value(), interval_);
        return max;
    }

    Number mid() const {
        Number mid;
        mpfi_mid(mid.value(), interval_);
        return mid;
    }

    Number len() const {
        Number len;
        mpfi_diam_abs(len.value(), interval_);
        return len;
    }

    Number rad() const {
        Number rad;
        mpfi_diam_abs(rad.value(), interval_);
        mpfr_div_ui(rad.value(), rad.value(), 2, MPFR_RNDU);
        return rad;
    }

    bool is_pos() const {
        return mpfi_is_strictly_pos(interval_);
    }

    bool is_neg() const {
        return mpfi_is_strictly_neg(interval_);
    }

    bool is_nan() const {
        return mpfi_nan_p(interval_);
    }

    bool operator>(const MpfiInterval& interval) const {
        if(is_nan() || interval.is_nan()) {
            return false;
        }
        return mpfi_cmp(interval_, interval.interval_) > 0;
    }

    bool operator>(const Number& number) const {
        if(is_nan() || number.is_nan()) {
            return false;
        }
        return mpfi_cmp_fr(interval_, number.value()) > 0;
    }

    friend bool operator>(const Number& number, const MpfiInterval& interval) {
        if(number.is_nan() || interval.is_nan()) {
            return false;
        }
        return mpfi_cmp_fr(interval.interval_, number.value()) < 0;
    }

    template<IntegerType Integer>
    bool operator>(const Integer integer) const {
        if(is_nan()) {
            return false;
        }
        return mpfi_cmp_si(interval_, integer) > 0;
    }

    template<IntegerType Integer>
    friend bool operator>(const Integer integer, const MpfiInterval& interval) {
        if(interval.is_nan()) {
            return false;
        }
        return mpfi_cmp_si(interval.interval_, integer) < 0;
    }

    bool operator<(const MpfiInterval& interval) const {
        if(is_nan() || interval.is_nan()) {
            return false;
        }
        return mpfi_cmp(interval_, interval.interval_) < 0;
    }

    bool operator<(const Number& number) const {
        if(is_nan() || number.is_nan()) {
            return false;
        }
        return mpfi_cmp_fr(interval_, number.value()) < 0;
    }

    friend bool operator<(const Number& number, const MpfiInterval& interval) {
        if(number.is_nan() || interval.is_nan()) {
            return false;
        }
        return mpfi_cmp_fr(interval.interval_, number.value()) > 0;
    }

    template<IntegerType Integer>
    bool operator<(const Integer integer) const {
        if(is_nan()) {
            return false;
        }
        return mpfi_cmp_si(interval_, integer) < 0;
    }

    template<IntegerType Integer>
    friend bool operator<(const Integer integer, const MpfiInterval& interval) {
        if(interval.is_nan()) {
            return false;
        }
        return mpfi_cmp_si(interval.interval_, integer) > 0;
    }

    MpfiInterval operator+() const {
        MpfiInterval pos;
        mpfi_set(pos.interval_, interval_);
        return pos;
    }

    MpfiInterval operator-() const {
        MpfiInterval neg;
        mpfi_neg(neg.interval_, interval_);
        return neg;
    }

    MpfiInterval operator+(const MpfiInterval& interval) const {
        MpfiInterval add;
        mpfi_add(add.interval_, interval_, interval.interval_);
        return add;
    }

    MpfiInterval operator+(const Number& number) const {
        MpfiInterval add;
        mpfi_add_fr(add.interval_, interval_, number.value());
        return add;
    }

    friend MpfiInterval operator+(const Number& number, const MpfiInterval& interval) {
        MpfiInterval add;
        mpfi_add_fr(add.interval_, interval.interval_, number.value());
        return add;
    }

    template<IntegerType Integer>
    MpfiInterval operator+(const Integer integer) const {
        MpfiInterval add;
        mpfi_add_si(add.interval_, interval_, integer);
        return add;
    }

    template<IntegerType Integer>
    friend MpfiInterval operator+(const Integer integer, const MpfiInterval& interval) {
        MpfiInterval add;
        mpfi_add_si(add.interval_, interval.interval_, integer);
        return add;
    }

    MpfiInterval operator-(const MpfiInterval& interval) const {
        MpfiInterval sub;
        mpfi_sub(sub.interval_, interval_, interval.interval_);
        return sub;
    }

    MpfiInterval operator-(const Number& number) const {
        if(number.is_nan()) {
            return nan();
        }
        MpfiInterval sub;
        mpfi_sub_fr(sub.interval_, interval_, number.value());
        return sub;
    }

    friend MpfiInterval operator-(const Number& number, const MpfiInterval& interval) {
        if(number.is_nan()) {
            return nan();
        }
        MpfiInterval sub;
        mpfi_fr_sub(sub.interval_, number.value(), interval.interval_);
        return sub;
    }

    template<IntegerType Integer>
    MpfiInterval operator-(const Integer integer) const {
        MpfiInterval sub;
        mpfi_sub_si(sub.interval_, interval_, integer);
        return sub;
    }

    template<IntegerType Integer>
    friend MpfiInterval operator-(const Integer integer, const MpfiInterval& interval) {
        MpfiInterval sub;
        mpfi_si_sub(sub.interval_, integer, interval.interval_);
        return sub;
    }

    MpfiInterval operator*(const MpfiInterval& interval) const {
        MpfiInterval mul;
        mpfi_mul(mul.interval_, interval_, interval.interval_);
        return mul;
    }

    MpfiInterval operator*(const Number& number) const {
        MpfiInterval mul;
        mpfi_mul_fr(mul.interval_, interval_, number.value());
        return mul;
    }

    friend MpfiInterval operator*(const Number& number, const MpfiInterval& interval) {
        MpfiInterval mul;
        mpfi_mul_fr(mul.interval_, interval.interval_, number.value());
        return mul;
    }

    template<IntegerType Integer>
    MpfiInterval operator*(const Integer integer) const {
        MpfiInterval mul;
        mpfi_mul_si(mul.interval_, interval_, integer);
        return mul;
    }

    template<IntegerType Integer>
    friend MpfiInterval operator*(const Integer integer, const MpfiInterval& interval) {
        MpfiInterval mul;
        mpfi_mul_si(mul.interval_, interval.interval_, integer);
        return mul;
    }

    MpfiInterval operator/(const MpfiInterval& interval) const {
        if(!interval.is_pos() && !interval.is_neg()) {
            return nan();
        }
        MpfiInterval div;
        mpfi_div(div.interval_, interval_, interval.interval_);
        return div;
    }

    MpfiInterval operator/(const Number& number) const {
        if(!number.is_pos() && !number.is_neg()) {
            return nan();
        }
        MpfiInterval div;
        mpfi_div_fr(div.interval_, interval_, number.value());
        return div;
    }

    friend MpfiInterval operator/(const Number& number, const MpfiInterval& interval) {
        if(!interval.is_pos() && !interval.is_neg()) {
            return nan();
        }
        MpfiInterval div;
        mpfi_fr_div(div.interval_, number.value(), interval.interval_);
        return div;
    }

    template<IntegerType Integer>
    MpfiInterval operator/(const Integer integer) const {
        if(integer == 0) {
            return nan();
        }
        MpfiInterval div;
        mpfi_div_si(div.interval_, interval_, integer);
        return div;
    }

    template<IntegerType Integer>
    friend MpfiInterval operator/(const Integer integer, const MpfiInterval& interval) {
        if(!interval.is_pos() && !interval.is_neg()) {
            return nan();
        }
        MpfiInterval div;
        mpfi_si_div(div.interval_, integer, interval.interval_);
        return div;
    }

    MpfiInterval inv() const {
        if(!is_pos() && !is_neg()) {
            return nan();
        }
        MpfiInterval inv;
        mpfi_inv(inv.interval_, interval_);
        return inv;
    }

    MpfiInterval sqr() const {
        MpfiInterval sqr;
        mpfi_sqr(sqr.interval_, interval_);
        return sqr;
    }

    MpfiInterval sqrt() const {
        if(min() < 0) {
            return nan();
        }
        MpfiInterval sqrt;
        mpfi_sqrt(sqrt.interval_, interval_);
        return sqrt;
    }

    MpfiInterval cos() const {
        MpfiInterval cos;
        mpfi_cos(cos.interval_, interval_);
        return cos;
    }

    MpfiInterval sin() const {
        MpfiInterval sin;
        mpfi_sin(sin.interval_, interval_);
        return sin;
    }

    static MpfiInterval nan() {
        return MpfiInterval();
    }

    static MpfiInterval unit() {
        MpfiInterval unit;
        mpfi_interv_si(unit.interval_, 0, 1);
        return unit;
    }

    static MpfiInterval pi() {
        MpfiInterval pi;
        mpfi_const_pi(pi.interval_);
        return pi;
    }

    static void set_print_precision(const int print_precision) {
        Number::set_print_precision(print_precision);
    }

    static void set_print_mode(const IntervalPrintMode print_mode) {
        print_mode_ = print_mode;
    }

    friend std::ostream& operator<<(std::ostream& ostream, const MpfiInterval& interval) {
        switch(print_mode_) {
            case IntervalPrintMode::MIN_AND_MAX: {
                return ostream << "[" << interval.min() << " : " << interval.max() << "]";
            }
            case IntervalPrintMode::MID_AND_RAD: {
                return ostream << "[" << interval.mid() << " ~ " << interval.rad() << "]";
            }
            default: throw std::invalid_argument("Invalid print type");
        }
    }
};
