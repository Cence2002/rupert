#pragma once

#include <iostream>
#include <boost/numeric/interval.hpp>
#include <mpfr.h>
#include <mpfi.h>

enum class IntervalPrintMode {
    MIN_AND_MAX,
    MID_AND_RAD,
};

template<typename Int>
concept IntegerType = std::is_integral_v<Int>;

template<typename I>
concept IntervalType =
        std::is_default_constructible_v<I> &&
        std::is_copy_constructible_v<I> &&
        std::is_copy_assignable_v<I> &&
        std::is_destructible_v<I> &&

        std::is_constructible_v<I, int> &&
        std::is_constructible_v<I, int, int> &&

        requires(const I interval, const I i, const int n) {
            { interval.min() } -> std::same_as<I>;
            { interval.max() } -> std::same_as<I>;
            { interval.mid() } -> std::same_as<I>;
            { interval.len() } -> std::same_as<I>;

            { interval.is_pos() } -> std::same_as<bool>;
            { interval.is_neg() } -> std::same_as<bool>;
            { interval.has_zero() } -> std::same_as<bool>;

            { interval > i } -> std::same_as<bool>;
            { interval > n } -> std::same_as<bool>;
            { interval < i } -> std::same_as<bool>;
            { interval < n } -> std::same_as<bool>;

            { +interval } -> std::same_as<I>;
            { -interval } -> std::same_as<I>;

            { interval + i } -> std::same_as<I>;
            { interval + n } -> std::same_as<I>;
            { n + interval } -> std::same_as<I>;

            { interval - i } -> std::same_as<I>;
            { interval - n } -> std::same_as<I>;
            { n - interval } -> std::same_as<I>;

            { interval * i } -> std::same_as<I>;
            { interval * n } -> std::same_as<I>;
            { n * interval } -> std::same_as<I>;

            { interval / i } -> std::same_as<I>;
            { interval / n } -> std::same_as<I>;
            { n / interval } -> std::same_as<I>;

            { interval.inv() } -> std::same_as<I>;
            { interval.sqr() } -> std::same_as<I>;
            { interval.sqrt() } -> std::same_as<I>;
            { interval.cos() } -> std::same_as<I>;
            { interval.sin() } -> std::same_as<I>;

            { I::unit() } -> std::same_as<I>;
            { I::pi() } -> std::same_as<I>;
        } &&

        requires(const IntervalPrintMode print_mode) {
            { I::set_print_mode(print_mode) };
        } &&

        requires(const I a, std::ostream &os) {
            { os << a } -> std::same_as<std::ostream&>;
        };

struct FastInterval {
private:
    double left, right;

    static inline IntervalPrintMode print_mode = IntervalPrintMode::MIN_AND_MAX;

    explicit FastInterval(const double left, const double right) : left(left), right(right) {}

public:
    explicit FastInterval() : left(std::numeric_limits<double>::quiet_NaN()), right(std::numeric_limits<double>::quiet_NaN()) {}

    FastInterval(const FastInterval &i) = default;

    FastInterval &operator=(const FastInterval &i) = default;

    ~FastInterval() = default;

    template<IntegerType Int>
    explicit FastInterval(const Int val) : left(static_cast<double>(val)), right(static_cast<double>(val)) {}

    template<IntegerType Int>
    explicit FastInterval(const Int min, const Int max) : left(static_cast<double>(min)), right(static_cast<double>(max)) {}

    FastInterval min() const {
        return FastInterval(left, left);
    }

    FastInterval max() const {
        return FastInterval(right, right);
    }

    FastInterval mid() const {
        return FastInterval((left + right) / 2, (left + right) / 2);
    }

    FastInterval len() const {
        return FastInterval(right - left, right - left);
    }

    bool is_pos() const {
        return left > 0;
    }

    bool is_neg() const {
        return right < 0;
    }

    bool has_zero() const {
        return left <= 0 && 0 <= right;
    }

    bool operator>(const FastInterval &i) const {
        return left > i.right;
    }

    template<IntegerType Int>
    bool operator>(const Int n) const {
        return left > static_cast<double>(n);
    }

    bool operator<(const FastInterval &i) const {
        return right < i.left;
    }

    template<IntegerType Int>
    bool operator<(const Int n) const {
        return right < static_cast<double>(n);
    }

    FastInterval operator+() const {
        return FastInterval(+left, +right);
    }

    FastInterval operator-() const {
        return FastInterval(-right, -left);
    }

    FastInterval operator+(const FastInterval &i) const {
        return FastInterval(left + i.left, right + i.right);
    }

    template<IntegerType Int>
    FastInterval operator+(const Int n) const {
        return FastInterval(left + static_cast<double>(n), right + static_cast<double>(n));
    }

    template<IntegerType Int>
    friend FastInterval operator+(const Int n, const FastInterval &i) {
        return FastInterval(static_cast<double>(n) + i.left, static_cast<double>(n) + i.right);
    }

    FastInterval operator-(const FastInterval &i) const {
        return FastInterval(left - i.right, right - i.left);
    }

    template<IntegerType Int>
    FastInterval operator-(const Int n) const {
        return FastInterval(left - static_cast<double>(n), right - static_cast<double>(n));
    }

    template<IntegerType Int>
    friend FastInterval operator-(const Int n, const FastInterval &i) {
        return FastInterval(static_cast<double>(n) - i.right, static_cast<double>(n) - i.left);
    }

    FastInterval operator*(const FastInterval &i) const {
        const double left_left = left * i.left;
        const double left_right = left * i.right;
        const double right_left = right * i.left;
        const double right_right = right * i.right;
        return FastInterval(std::min({left_left, left_right, right_left, right_right}),
                            std::max({left_left, left_right, right_left, right_right}));
    }

    template<IntegerType Int>
    FastInterval operator*(const Int n) const {
        const double left_n = left * static_cast<double>(n);
        const double right_n = right * static_cast<double>(n);
        return FastInterval(std::min(left_n, right_n), std::max(left_n, right_n));
    }

    template<IntegerType Int>
    friend FastInterval operator*(const Int n, const FastInterval &i) {
        const double n_left = static_cast<double>(n) * i.left;
        const double n_right = static_cast<double>(n) * i.right;
        return FastInterval(std::min(n_left, n_right), std::max(n_left, n_right));
    }

    FastInterval operator/(const FastInterval &i) const {
        if(i.has_zero()) {
            return FastInterval(-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
        }
        const double left_left = left / i.left;
        const double left_right = left / i.right;
        const double right_left = right / i.left;
        const double right_right = right / i.right;
        return FastInterval(std::min({left_left, left_right, right_left, right_right}),
                            std::max({left_left, left_right, right_left, right_right}));
    }

    template<IntegerType Int>
    FastInterval operator/(const Int n) const {
        if(n == 0) {
            return FastInterval(-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
        }
        const double left_n = left / static_cast<double>(n);
        const double right_n = right / static_cast<double>(n);
        return FastInterval(std::min(left_n, right_n), std::max(left_n, right_n));
    }

    template<IntegerType Int>
    friend FastInterval operator/(const Int n, const FastInterval &i) {
        if(i.has_zero()) {
            return FastInterval(-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
        }
        const double n_left = static_cast<double>(n) / i.left;
        const double n_right = static_cast<double>(n) / i.right;
        return FastInterval(std::min(n_left, n_right), std::max(n_left, n_right));
    }

    FastInterval inv() const {
        if(has_zero()) {
            return FastInterval(-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
        }
        return FastInterval(1.0 / right, 1.0 / left);
    }

    FastInterval sqr() const {
        if(right < 0) {
            return FastInterval(right * right, left * left);
        } else if(left < 0) {
            return FastInterval(0, std::max(left * left, right * right));
        } else {
            return FastInterval(left * left, right * right);
        }
    }

    FastInterval sqrt() const {
        if(has_zero()) {
            return FastInterval();
        }
        return FastInterval(std::sqrt(left), std::sqrt(right));
    }

    FastInterval cos() const {
        constexpr double two_pi = 2 * M_PI;
        constexpr double pi = M_PI;
        const double cos_left = std::cos(left);
        const double cos_right = std::cos(right);
        double new_left = std::min(cos_left, cos_right);
        double new_right = std::max(cos_left, cos_right);
        if(std::floor(left / two_pi) < std::floor(right / two_pi)) {
            new_right = 1.0;
        }
        if(std::floor((left - pi) / two_pi) < std::floor((right - pi) / two_pi)) {
            new_left = -1.0;
        }
        return FastInterval(new_left, new_right);
    }

    FastInterval sin() const {
        constexpr double two_pi = 2 * M_PI;
        constexpr double half_pi = M_PI / 2;
        const double sin_left = std::sin(left);
        const double sin_right = std::sin(right);
        double new_left = std::min(sin_left, sin_right);
        double new_right = std::max(sin_left, sin_right);
        if(std::floor((left - half_pi) / two_pi) < std::floor((right - half_pi) / two_pi)) {
            new_right = 1.0;
        }
        if(std::floor((left + half_pi) / two_pi) < std::floor((right + half_pi) / two_pi)) {
            new_left = -1.0;
        }
        return FastInterval(new_left, new_right);
    }

    static FastInterval unit() {
        return FastInterval(0, 1);
    }

    static FastInterval pi() {
        return FastInterval(M_PI, M_PI);
    }

    static void set_print_mode(const IntervalPrintMode print_mode) {
        FastInterval::print_mode = print_mode;
    }

    friend std::ostream &operator<<(std::ostream &os, const FastInterval &interval) {
        switch(print_mode) {
            case IntervalPrintMode::MIN_AND_MAX: {
                const double min = interval.left;
                const double max = interval.right;
                return os << "[" << min << " | " << max << "]";
            }
            case IntervalPrintMode::MID_AND_RAD: {
                const double mid = (interval.left + interval.right) / 2;
                const double rad = (interval.right - interval.left) / 2;
                return os << "[" << mid << " | " << rad << "]";
            }
        }
        return os;
    }
};

struct BoostInterval {
private:
    using BoostIntervalType = boost::numeric::interval<
        double,
        boost::numeric::interval_lib::policies<
            boost::numeric::interval_lib::save_state<
                boost::numeric::interval_lib::rounded_transc_std<double>>,
            boost::numeric::interval_lib::checking_base<double>>>;

    BoostIntervalType interval{};

    static inline IntervalPrintMode print_mode = IntervalPrintMode::MIN_AND_MAX;

    explicit BoostInterval(const BoostIntervalType &interval) : interval(interval) {}

public:
    explicit BoostInterval() : interval(BoostIntervalType::empty()) {}

    BoostInterval(const BoostInterval &i) = default;

    BoostInterval &operator=(const BoostInterval &i) = default;

    ~BoostInterval() = default;

    template<IntegerType Int>
    explicit BoostInterval(const Int val) : interval(val) {}

    template<IntegerType Int>
    explicit BoostInterval(const Int min, const Int max) : interval(min, max) {}

    BoostInterval min() const {
        return BoostInterval(BoostIntervalType(boost::numeric::lower(interval)));
    }

    BoostInterval max() const {
        return BoostInterval(BoostIntervalType(boost::numeric::upper(interval)));
    }

    BoostInterval mid() const {
        return BoostInterval(BoostIntervalType(boost::numeric::median(interval)));
    }

    BoostInterval len() const {
        return BoostInterval(BoostIntervalType(boost::numeric::width(interval)));
    }

    bool is_pos() const {
        return boost::numeric::interval_lib::cergt(interval, 0.0);
    }

    bool is_neg() const {
        return boost::numeric::interval_lib::cerlt(interval, 0.0);
    }

    bool has_zero() const {
        return boost::numeric::zero_in(interval);
    }

    bool operator>(const BoostInterval &i) const {
        return boost::numeric::interval_lib::cergt(interval, i.interval);
    }

    template<IntegerType Int>
    bool operator>(const Int n) const {
        return boost::numeric::interval_lib::cergt(interval, static_cast<double>(n));
    }

    bool operator<(const BoostInterval &i) const {
        return boost::numeric::interval_lib::cerlt(interval, i.interval);
    }

    template<IntegerType Int>
    bool operator<(const Int n) const {
        return boost::numeric::interval_lib::cerlt(interval, static_cast<double>(n));
    }

    BoostInterval operator+() const {
        return BoostInterval(+interval);
    }

    BoostInterval operator-() const {
        return BoostInterval(-interval);
    }

    BoostInterval operator+(const BoostInterval &i) const {
        return BoostInterval(interval + i.interval);
    }

    template<IntegerType Int>
    BoostInterval operator+(const Int n) const {
        return BoostInterval(interval + static_cast<double>(n));
    }

    template<IntegerType Int>
    friend BoostInterval operator+(const Int n, const BoostInterval &i) {
        return BoostInterval(static_cast<double>(n) + i.interval);
    }

    BoostInterval operator-(const BoostInterval &i) const {
        return BoostInterval(interval - i.interval);
    }

    template<IntegerType Int>
    BoostInterval operator-(const Int n) const {
        return BoostInterval(interval - static_cast<double>(n));
    }

    template<IntegerType Int>
    friend BoostInterval operator-(const Int n, const BoostInterval &i) {
        return BoostInterval(static_cast<double>(n) - i.interval);
    }

    BoostInterval operator*(const BoostInterval &i) const {
        return BoostInterval(interval * i.interval);
    }

    template<IntegerType Int>
    BoostInterval operator*(const Int n) const {
        return BoostInterval(interval * static_cast<double>(n));
    }

    template<IntegerType Int>
    friend BoostInterval operator*(const Int n, const BoostInterval &i) {
        return BoostInterval(static_cast<double>(n) * i.interval);
    }

    BoostInterval operator/(const BoostInterval &i) const {
        return BoostInterval(interval / i.interval);
    }

    template<IntegerType Int>
    BoostInterval operator/(const Int n) const {
        return BoostInterval(interval / static_cast<double>(n));
    }

    template<IntegerType Int>
    friend BoostInterval operator/(const Int n, const BoostInterval &i) {
        return BoostInterval(static_cast<double>(n) / i.interval);
    }

    BoostInterval inv() const {
        return BoostInterval(boost::numeric::interval_lib::multiplicative_inverse(interval));
    }

    BoostInterval sqr() const {
        return BoostInterval(boost::numeric::square(interval));
    }

    BoostInterval sqrt() const {
        return BoostInterval(boost::numeric::sqrt(interval));
    }

    BoostInterval cos() const {
        return BoostInterval(boost::numeric::cos(interval));
    }

    BoostInterval sin() const {
        return BoostInterval(boost::numeric::sin(interval));
    }

    static BoostInterval unit() {
        return BoostInterval(0, 1);
    }

    static BoostInterval pi() {
        return BoostInterval(boost::numeric::interval_lib::pi<BoostIntervalType>());
    }

    static void set_print_mode(const IntervalPrintMode print_mode) {
        BoostInterval::print_mode = print_mode;
    }

    friend std::ostream &operator<<(std::ostream &os, const BoostInterval &interval) {
        switch(print_mode) {
            case IntervalPrintMode::MIN_AND_MAX: {
                const double min = boost::numeric::lower(interval.interval);
                const double max = boost::numeric::upper(interval.interval);
                return os << "[" << min << " | " << max << "]";
            }
            case IntervalPrintMode::MID_AND_RAD: {
                const double mid = boost::numeric::median(interval.interval);
                const double rad = boost::numeric::width(interval.interval) / 2;
                return os << "[" << mid << " ~ " << rad << "]";
            }
            default: throw std::invalid_argument("Invalid print type");
        }
    }
};

struct PreciseInterval {
private:
    static inline IntervalPrintMode print_mode = IntervalPrintMode::MIN_AND_MAX;

    mpfi_t interval{};

    explicit PreciseInterval(const mpfi_t &interval) {
        mpfi_init_set(this->interval, interval);
    }

public:
    explicit PreciseInterval() {
        mpfi_init(interval);
    }

    PreciseInterval(const PreciseInterval &i) {
        mpfi_init_set(interval, i.interval);
    }

    PreciseInterval &operator=(const PreciseInterval &i) {
        mpfi_set(interval, i.interval);
        return *this;
    }

    ~PreciseInterval() {
        mpfi_clear(interval);
    }

    template<IntegerType Int>
    explicit PreciseInterval(const Int val) {
        mpfi_init_set_si(interval, val);
    }

    template<IntegerType Int>
    explicit PreciseInterval(const Int min, const Int max) {
        mpfi_init(interval);
        mpfi_interv_si(interval, min, max);
    }

    PreciseInterval min() const {
        mpfr_t min;
        mpfr_init(min);
        mpfi_get_left(min, interval);
        mpfi_t min_interval;
        mpfi_init_set_fr(min_interval, min);
        mpfr_clear(min);
        return PreciseInterval(min_interval);
    }

    PreciseInterval max() const {
        mpfr_t max;
        mpfr_init(max);
        mpfi_get_right(max, interval);
        mpfi_t max_interval;
        mpfi_init_set_fr(max_interval, max);
        mpfr_clear(max);
        return PreciseInterval(max_interval);
    }

    PreciseInterval mid() const {
        return (min() + max()) / 2;
    }

    PreciseInterval len() const {
        return max() - min();
    }

    bool is_pos() const {
        return mpfi_is_strictly_pos(interval);
    }

    bool is_neg() const {
        return mpfi_is_strictly_neg(interval);
    }

    bool has_zero() const {
        return mpfi_has_zero(interval);
    }

    bool operator>(const PreciseInterval &i) const {
        return mpfi_cmp(interval, i.interval) > 0;
    }

    template<IntegerType Int>
    bool operator>(const Int n) const {
        return mpfi_cmp_si(interval, n) > 0;
    }

    bool operator<(const PreciseInterval &i) const {
        return mpfi_cmp(interval, i.interval) < 0;
    }

    template<IntegerType Int>
    bool operator<(const Int n) const {
        return mpfi_cmp_si(interval, n) < 0;
    }

    PreciseInterval operator+() const {
        mpfi_t pos;
        mpfi_init(pos);
        mpfi_set(pos, interval);
        return PreciseInterval(pos);
    }

    PreciseInterval operator-() const {
        mpfi_t neg;
        mpfi_init(neg);
        mpfi_neg(neg, interval);
        return PreciseInterval(neg);
    }

    PreciseInterval operator+(const PreciseInterval &i) const {
        mpfi_t add;
        mpfi_init(add);
        mpfi_add(add, interval, i.interval);
        return PreciseInterval(add);
    }

    template<IntegerType Int>
    PreciseInterval operator+(const Int n) const {
        mpfi_t add;
        mpfi_init(add);
        mpfi_add_si(add, interval, n);
        return PreciseInterval(add);
    }

    template<IntegerType Int>
    friend PreciseInterval operator+(const Int n, const PreciseInterval &i) {
        mpfi_t add;
        mpfi_init(add);
        mpfi_add_si(add, i.interval, n);
        return PreciseInterval(add);
    }

    PreciseInterval operator-(const PreciseInterval &i) const {
        mpfi_t sub;
        mpfi_init(sub);
        mpfi_sub(sub, interval, i.interval);
        return PreciseInterval(sub);
    }

    template<IntegerType Int>
    PreciseInterval operator-(const Int n) const {
        mpfi_t sub;
        mpfi_init(sub);
        mpfi_sub_si(sub, interval, n);
        return PreciseInterval(sub);
    }

    template<IntegerType Int>
    friend PreciseInterval operator-(const Int n, const PreciseInterval &i) {
        mpfi_t sub;
        mpfi_init(sub);
        mpfi_d_sub(sub, n, i.interval);
        return PreciseInterval(sub);
    }

    PreciseInterval operator*(const PreciseInterval &i) const {
        mpfi_t mul;
        mpfi_init(mul);
        mpfi_mul(mul, interval, i.interval);
        return PreciseInterval(mul);
    }

    template<IntegerType Int>
    PreciseInterval operator*(const Int n) const {
        mpfi_t mul;
        mpfi_init(mul);
        mpfi_mul_si(mul, interval, n);
        return PreciseInterval(mul);
    }

    template<IntegerType Int>
    friend PreciseInterval operator*(const Int n, const PreciseInterval &i) {
        mpfi_t mul;
        mpfi_init(mul);
        mpfi_mul_si(mul, i.interval, n);
        return PreciseInterval(mul);
    }

    PreciseInterval operator/(const PreciseInterval &i) const {
        mpfi_t div;
        mpfi_init(div);
        mpfi_div(div, interval, i.interval);
        return PreciseInterval(div);
    }

    template<IntegerType Int>
    PreciseInterval operator/(const Int n) const {
        mpfi_t div;
        mpfi_init(div);
        mpfi_div_si(div, interval, n);
        return PreciseInterval(div);
    }

    template<IntegerType Int>
    friend PreciseInterval operator/(const Int n, const PreciseInterval &i) {
        mpfi_t div;
        mpfi_init(div);
        mpfi_d_div(div, n, i.interval);
        return PreciseInterval(div);
    }

    PreciseInterval inv() const {
        mpfi_t inv;
        mpfi_init(inv);
        mpfi_inv(inv, interval);
        return PreciseInterval(inv);
    }

    PreciseInterval sqr() const {
        mpfi_t sqr;
        mpfi_init(sqr);
        mpfi_sqr(sqr, interval);
        return PreciseInterval(sqr);
    }

    PreciseInterval sqrt() const {
        mpfi_t sqrt;
        mpfi_init(sqrt);
        mpfi_sqrt(sqrt, interval);
        return PreciseInterval(sqrt);
    }

    PreciseInterval cos() const {
        mpfi_t cos;
        mpfi_init(cos);
        mpfi_cos(cos, interval);
        return PreciseInterval(cos);
    }

    PreciseInterval sin() const {
        mpfi_t sin;
        mpfi_init(sin);
        mpfi_sin(sin, interval);
        return PreciseInterval(sin);
    }

    static PreciseInterval unit() {
        mpfi_t unit;
        mpfi_init(unit);
        mpfi_interv_si(unit, 0, 1);
        return PreciseInterval(unit);
    }

    static PreciseInterval pi() {
        mpfi_t pi;
        mpfi_init(pi);
        mpfi_const_pi(pi);
        return PreciseInterval(pi);
    }

    static void set_print_mode(const IntervalPrintMode print_mode) {
        PreciseInterval::print_mode = print_mode;
    }

    friend std::ostream &operator<<(std::ostream &os, const PreciseInterval &interval) {
        const std::streamsize precision = os.precision();
        switch(print_mode) {
            case IntervalPrintMode::MIN_AND_MAX: {
                mpfr_t min, max;
                mpfr_init(min);
                mpfr_init(max);
                mpfi_get_left(min, interval.interval);
                mpfi_get_right(max, interval.interval);
                char min_str[precision + 10];
                char max_str[precision + 10];
                mpfr_sprintf(min_str, "%.*Rg", precision, min);
                mpfr_sprintf(max_str, "%.*Rg", precision, max);
                mpfr_clear(min);
                mpfr_clear(max);
                return os << "[" << min_str << " | " << max_str << "]";
            }
            case IntervalPrintMode::MID_AND_RAD: {
                mpfr_t mid, rad;
                mpfr_init(mid);
                mpfr_init(rad);
                mpfi_mid(mid, interval.interval);
                mpfi_diam_abs(rad, interval.interval);
                mpfr_div_ui(rad, rad, 2, MPFR_RNDU);
                char mid_str[precision + 10];
                char rad_str[precision + 10];
                mpfr_sprintf(mid_str, "%.*Rg", precision, mid);
                mpfr_sprintf(rad_str, "%.*Rg", precision, rad);
                mpfr_clear(mid);
                mpfr_clear(rad);
                return os << "[" << mid_str << " ~ " << rad_str << "]";
            }
            default: throw std::invalid_argument("Invalid print type");
        }
    }
};
