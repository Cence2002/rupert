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
        std::is_constructible_v<I, int> &&
        std::is_constructible_v<I, int, int> &&

        std::is_destructible_v<I> &&
        std::is_copy_constructible_v<I> &&
        std::is_copy_assignable_v<I> &&
        std::is_move_constructible_v<I> &&
        std::is_move_assignable_v<I> &&

        requires(const I interval, const I i, const int n) {
            { interval.min() } -> std::same_as<I>;
            { interval.max() } -> std::same_as<I>;
            { interval.mid() } -> std::same_as<I>;
            { interval.len() } -> std::same_as<I>;
            { interval.rad() } -> std::same_as<I>;

            { interval.pos() } -> std::same_as<bool>;
            { interval.neg() } -> std::same_as<bool>;

            { interval > i } -> std::same_as<bool>;
            { interval > n } -> std::same_as<bool>;
            { n > interval } -> std::same_as<bool>;

            { interval < i } -> std::same_as<bool>;
            { interval < n } -> std::same_as<bool>;
            { n < interval } -> std::same_as<bool>;

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
    struct {
        double min, max;
    } interval{};

    static inline IntervalPrintMode print_mode = IntervalPrintMode::MIN_AND_MAX;

    explicit FastInterval(const double min, const double max) : interval{min, max} {}

#undef NAN
    static constexpr double NAN = std::numeric_limits<double>::quiet_NaN();
    static constexpr double INF = std::numeric_limits<double>::infinity();
    static constexpr double PI = std::numbers::pi_v<double>;
    static constexpr double TWO_PI = 2 * std::numbers::pi_v<double>;
    static constexpr double HALF_PI = std::numbers::pi_v<double> / 2;

public:
    explicit FastInterval() : interval{NAN, NAN} {}

    template<IntegerType Int>
    explicit FastInterval(const Int val) : interval{static_cast<double>(val), static_cast<double>(val)} {}

    template<IntegerType Int>
    explicit FastInterval(const Int min, const Int max) : interval{static_cast<double>(min), static_cast<double>(max)} {}

    ~FastInterval() = default;

    FastInterval(const FastInterval &i) = default;

    FastInterval &operator=(const FastInterval &i) = default;

    FastInterval(FastInterval &&i) = default;

    FastInterval &operator=(FastInterval &&i) = default;

    FastInterval min() const {
        return FastInterval(interval.min, interval.min);
    }

    FastInterval max() const {
        return FastInterval(interval.max, interval.max);
    }

    FastInterval mid() const {
        const double mid = (interval.min + interval.max) / 2;
        return FastInterval(mid, mid);
    }

    FastInterval len() const {
        const double len = interval.max - interval.min;
        return FastInterval(len, len);
    }

    FastInterval rad() const {
        const double rad = (interval.max - interval.min) / 2;
        return FastInterval(rad, rad);
    }

    bool pos() const {
        return 0 < interval.min;
    }

    bool neg() const {
        return interval.max < 0;
    }

    bool operator>(const FastInterval &i) const {
        return interval.min > i.interval.max;
    }

    template<IntegerType Int>
    bool operator>(const Int n) const {
        return interval.min > static_cast<double>(n);
    }

    template<IntegerType Int>
    friend bool operator>(const Int n, const FastInterval &i) {
        return static_cast<double>(n) > i.interval.max;
    }

    bool operator<(const FastInterval &i) const {
        return interval.max < i.interval.min;
    }

    template<IntegerType Int>
    bool operator<(const Int n) const {
        return interval.max < static_cast<double>(n);
    }

    template<IntegerType Int>
    friend bool operator<(const Int n, const FastInterval &i) {
        return static_cast<double>(n) < i.interval.min;
    }

    FastInterval operator+() const {
        return FastInterval(+interval.min, +interval.max);
    }

    FastInterval operator-() const {
        return FastInterval(-interval.max, -interval.min);
    }

    FastInterval operator+(const FastInterval &i) const {
        return FastInterval(interval.min + i.interval.min, interval.max + i.interval.max);
    }

    template<IntegerType Int>
    FastInterval operator+(const Int n) const {
        return FastInterval(interval.min + static_cast<double>(n), interval.max + static_cast<double>(n));
    }

    template<IntegerType Int>
    friend FastInterval operator+(const Int n, const FastInterval &i) {
        return FastInterval(static_cast<double>(n) + i.interval.min, static_cast<double>(n) + i.interval.max);
    }

    FastInterval operator-(const FastInterval &i) const {
        return FastInterval(interval.min - i.interval.max, interval.max - i.interval.min);
    }

    template<IntegerType Int>
    FastInterval operator-(const Int n) const {
        return FastInterval(interval.min - static_cast<double>(n), interval.max - static_cast<double>(n));
    }

    template<IntegerType Int>
    friend FastInterval operator-(const Int n, const FastInterval &i) {
        return FastInterval(static_cast<double>(n) - i.interval.max, static_cast<double>(n) - i.interval.min);
    }

    FastInterval operator*(const FastInterval &i) const {
        const double min_min = interval.min * i.interval.min;
        const double min_max = interval.min * i.interval.max;
        const double max_min = interval.max * i.interval.min;
        const double max_max = interval.max * i.interval.max;
        return FastInterval(std::min({min_min, min_max, max_min, max_max}),
                            std::max({min_min, min_max, max_min, max_max}));
    }

    template<IntegerType Int>
    FastInterval operator*(const Int n) const {
        const double min_n = interval.min * static_cast<double>(n);
        const double max_n = interval.max * static_cast<double>(n);
        return FastInterval(std::min(min_n, max_n), std::max(min_n, max_n));
    }

    template<IntegerType Int>
    friend FastInterval operator*(const Int n, const FastInterval &i) {
        const double n_min = static_cast<double>(n) * i.interval.min;
        const double n_max = static_cast<double>(n) * i.interval.max;
        return FastInterval(std::min(n_min, n_max), std::max(n_min, n_max));
    }

    FastInterval operator/(const FastInterval &i) const {
        if(i.interval.min <= 0 && 0 <= i.interval.max) {
            return FastInterval(-INF, INF);
        }
        const double min_min = interval.min / i.interval.min;
        const double min_max = interval.min / i.interval.max;
        const double max_min = interval.max / i.interval.min;
        const double max_max = interval.max / i.interval.max;
        return FastInterval(std::min({min_min, min_max, max_min, max_max}),
                            std::max({min_min, min_max, max_min, max_max}));
    }

    template<IntegerType Int>
    FastInterval operator/(const Int n) const {
        if(n == 0) {
            return FastInterval(-INF, INF);
        }
        const double min_n = interval.min / static_cast<double>(n);
        const double max_n = interval.max / static_cast<double>(n);
        return FastInterval(std::min(min_n, max_n), std::max(min_n, max_n));
    }

    template<IntegerType Int>
    friend FastInterval operator/(const Int n, const FastInterval &i) {
        if(i.interval.min <= 0 && 0 <= i.interval.max) {
            return FastInterval(-INF, INF);
        }
        const double n_min = static_cast<double>(n) / i.interval.min;
        const double n_max = static_cast<double>(n) / i.interval.max;
        return FastInterval(std::min(n_min, n_max), std::max(n_min, n_max));
    }

    FastInterval inv() const {
        if(interval.min <= 0 && 0 <= interval.max) {
            return FastInterval(-INF, INF);
        }
        return FastInterval(1.0 / interval.max, 1.0 / interval.min);
    }

    FastInterval sqr() const {
        if(0 <= interval.min) {
            return FastInterval(interval.min * interval.min, interval.max * interval.max);
        }
        if(interval.max <= 0) {
            return FastInterval(interval.max * interval.max, interval.min * interval.min);
        }
        return FastInterval(0, std::max(interval.min * interval.min, interval.max * interval.max));
    }

    FastInterval sqrt() const {
        if(interval.min < 0) {
            return FastInterval(NAN, NAN);
        }
        return FastInterval(std::sqrt(interval.min), std::sqrt(interval.max));
    }

    FastInterval cos() const {
        const double cos_min = std::cos(interval.min);
        const double cos_max = std::cos(interval.max);
        double min = std::min(cos_min, cos_max);
        double max = std::max(cos_min, cos_max);
        if(std::floor(interval.min / TWO_PI) != std::floor(interval.max / TWO_PI)) {
            max = 1.0;
        }
        if(std::floor((interval.min - PI) / TWO_PI) != std::floor((interval.max - PI) / TWO_PI)) {
            min = -1.0;
        }
        return FastInterval(min, max);
    }

    FastInterval sin() const {
        const double sin_min = std::sin(interval.min);
        const double sin_max = std::sin(interval.max);
        double min = std::min(sin_min, sin_max);
        double max = std::max(sin_min, sin_max);
        if(std::floor((interval.min - HALF_PI) / TWO_PI) < std::floor((interval.max - HALF_PI) / TWO_PI)) {
            max = 1.0;
        }
        if(std::floor((interval.min + HALF_PI) / TWO_PI) < std::floor((interval.max + HALF_PI) / TWO_PI)) {
            min = -1.0;
        }
        return FastInterval(min, max);
    }

    static FastInterval unit() {
        return FastInterval(0, 1);
    }

    static FastInterval pi() {
        return FastInterval(PI, PI);
    }

    static void set_print_mode(const IntervalPrintMode interval_print_mode) {
        print_mode = interval_print_mode;
    }

    friend std::ostream &operator<<(std::ostream &os, const FastInterval &interval) {
        switch(print_mode) {
            case IntervalPrintMode::MIN_AND_MAX: {
                const double min = interval.min().interval.min;
                const double max = interval.max().interval.min;
                return os << "[" << min << " | " << max << "]";
            }
            case IntervalPrintMode::MID_AND_RAD: {
                const double mid = interval.mid().interval.min;
                const double rad = interval.rad().interval.min;
                return os << "[" << mid << " | " << rad << "]";
            }
        }
        return os;
    }
};

static_assert(IntervalType<FastInterval>);

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

    template<IntegerType Int>
    explicit BoostInterval(const Int val) : interval(val) {}

    template<IntegerType Int>
    explicit BoostInterval(const Int min, const Int max) : interval(min, max) {}

    ~BoostInterval() = default;

    BoostInterval(const BoostInterval &i) = default;

    BoostInterval &operator=(const BoostInterval &i) = default;

    BoostInterval(BoostInterval &&i) = default;

    BoostInterval &operator=(BoostInterval &&i) = default;

    BoostInterval min() const {
        return BoostInterval(BoostIntervalType(boost::numeric::lower(interval)));
    }

    BoostInterval max() const {
        return BoostInterval(BoostIntervalType(boost::numeric::upper(interval)));
    }

    BoostInterval mid() const {
        return BoostInterval(BoostIntervalType(boost::numeric::lower(interval)) + BoostIntervalType(boost::numeric::upper(interval)) / 2.0);
    }

    BoostInterval len() const {
        return BoostInterval(BoostIntervalType(boost::numeric::width(interval)));
    }

    BoostInterval rad() const {
        return BoostInterval(BoostIntervalType(boost::numeric::width(interval)) / 2.0);
    }

    bool pos() const {
        return boost::numeric::interval_lib::cerlt(0.0, interval);
    }

    bool neg() const {
        return boost::numeric::interval_lib::cerlt(interval, 0.0);
    }

    bool operator>(const BoostInterval &i) const {
        return boost::numeric::interval_lib::cergt(interval, i.interval);
    }

    template<IntegerType Int>
    bool operator>(const Int n) const {
        return boost::numeric::interval_lib::cergt(interval, static_cast<double>(n));
    }

    template<IntegerType Int>
    friend bool operator>(const Int n, const BoostInterval &i) {
        return boost::numeric::interval_lib::cergt(static_cast<double>(n), i.interval);
    }

    bool operator<(const BoostInterval &i) const {
        return boost::numeric::interval_lib::cerlt(interval, i.interval);
    }

    template<IntegerType Int>
    bool operator<(const Int n) const {
        return boost::numeric::interval_lib::cerlt(interval, static_cast<double>(n));
    }

    template<IntegerType Int>
    friend bool operator<(const Int n, const BoostInterval &i) {
        return boost::numeric::interval_lib::cerlt(static_cast<double>(n), i.interval);
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
                const double min = interval.min().interval.lower();
                const double max = interval.max().interval.upper();
                return os << "[" << min << " | " << max << "]";
            }
            case IntervalPrintMode::MID_AND_RAD: {
                const double mid = interval.mid().interval.lower();
                const double rad = interval.rad().interval.lower();
                return os << "[" << mid << " ~ " << rad << "]";
            }
            default: throw std::invalid_argument("Invalid print type");
        }
    }
};

static_assert(IntervalType<BoostInterval>);

struct PreciseInterval {
private:
    static inline IntervalPrintMode print_mode = IntervalPrintMode::MIN_AND_MAX;

    mpfi_t interval{};

    explicit PreciseInterval(const mpfi_t &i) {
        mpfi_init_set(interval, i);
    }

public:
    explicit PreciseInterval() {
        mpfi_init(interval);
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

    ~PreciseInterval() {
        mpfi_clear(interval);
    }

    PreciseInterval(const PreciseInterval &i) {
        mpfi_init_set(interval, i.interval);
    }

    PreciseInterval &operator=(const PreciseInterval &i) {
        if(this != &i) {
            mpfi_set(interval, i.interval);
        }
        return *this;
    }

    PreciseInterval(PreciseInterval &&i) noexcept {
        mpfi_init_set(interval, i.interval);
        mpfi_set_d(i.interval, std::numeric_limits<double>::quiet_NaN());
    }

    PreciseInterval &operator=(PreciseInterval &&i) noexcept {
        if(this != &i) {
            mpfi_set(interval, i.interval);
            mpfi_set_d(i.interval, std::numeric_limits<double>::quiet_NaN());
        }
        return *this;
    }

    PreciseInterval min() const {
        mpfr_t left;
        mpfr_init(left);
        mpfi_get_left(left, interval);
        PreciseInterval min;
        mpfi_init_set_fr(min.interval, left);
        mpfr_clear(left);
        return min;
    }

    PreciseInterval max() const {
        mpfr_t right;
        mpfr_init(right);
        mpfi_get_right(right, interval);
        PreciseInterval max;
        mpfi_init_set_fr(max.interval, right);
        mpfr_clear(right);
        return max;
    }

    PreciseInterval mid() const {
        return (max() + min()) / 2;
    }

    PreciseInterval len() const {
        return max() - min();
    }

    PreciseInterval rad() const {
        return (max() - min()) / 2;
    }

    bool pos() const {
        return mpfi_is_strictly_pos(interval);
    }

    bool neg() const {
        return mpfi_is_strictly_neg(interval);
    }

    bool operator>(const PreciseInterval &i) const {
        return mpfi_cmp(interval, i.interval) > 0;
    }

    template<IntegerType Int>
    bool operator>(const Int n) const {
        return mpfi_cmp_si(interval, n) > 0;
    }

    template<IntegerType Int>
    friend bool operator>(const Int n, const PreciseInterval &i) {
        return mpfi_cmp_si(i.interval, n) < 0;
    }

    bool operator<(const PreciseInterval &i) const {
        return mpfi_cmp(interval, i.interval) < 0;
    }

    template<IntegerType Int>
    bool operator<(const Int n) const {
        return mpfi_cmp_si(interval, n) < 0;
    }

    template<IntegerType Int>
    friend bool operator<(const Int n, const PreciseInterval &i) {
        return mpfi_cmp_si(i.interval, n) > 0;
    }

    PreciseInterval operator+() const {
        PreciseInterval pos;
        mpfi_set(pos.interval, interval);
        return pos;
    }

    PreciseInterval operator-() const {
        PreciseInterval neg;
        mpfi_neg(neg.interval, interval);
        return neg;
    }

    PreciseInterval operator+(const PreciseInterval &i) const {
        PreciseInterval add;
        mpfi_add(add.interval, interval, i.interval);
        return add;
    }

    template<IntegerType Int>
    PreciseInterval operator+(const Int n) const {
        PreciseInterval add;
        mpfi_add_si(add.interval, interval, n);
        return add;
    }

    template<IntegerType Int>
    friend PreciseInterval operator+(const Int n, const PreciseInterval &i) {
        PreciseInterval add;
        mpfi_add_si(add.interval, i.interval, n);
        return add;
    }

    PreciseInterval operator-(const PreciseInterval &i) const {
        PreciseInterval sub;
        mpfi_sub(sub.interval, interval, i.interval);
        return sub;
    }

    template<IntegerType Int>
    PreciseInterval operator-(const Int n) const {
        PreciseInterval sub;
        mpfi_sub_si(sub.interval, interval, n);
        return sub;
    }

    template<IntegerType Int>
    friend PreciseInterval operator-(const Int n, const PreciseInterval &i) {
        PreciseInterval sub;
        mpfi_si_sub(sub.interval, n, i.interval);
        return sub;
    }

    PreciseInterval operator*(const PreciseInterval &i) const {
        PreciseInterval mul;
        mpfi_mul(mul.interval, interval, i.interval);
        return mul;
    }

    template<IntegerType Int>
    PreciseInterval operator*(const Int n) const {
        PreciseInterval mul;
        mpfi_mul_si(mul.interval, interval, n);
        return mul;
    }

    template<IntegerType Int>
    friend PreciseInterval operator*(const Int n, const PreciseInterval &i) {
        PreciseInterval mul;
        mpfi_mul_si(mul.interval, i.interval, n);
        return mul;
    }

    PreciseInterval operator/(const PreciseInterval &i) const {
        PreciseInterval div;
        mpfi_div(div.interval, interval, i.interval);
        return div;
    }

    template<IntegerType Int>
    PreciseInterval operator/(const Int n) const {
        PreciseInterval div;
        mpfi_div_si(div.interval, interval, n);
        return div;
    }

    template<IntegerType Int>
    friend PreciseInterval operator/(const Int n, const PreciseInterval &i) {
        PreciseInterval div;
        mpfi_si_div(div.interval, n, i.interval);
        return div;
    }

    PreciseInterval inv() const {
        PreciseInterval inv;
        mpfi_inv(inv.interval, interval);
        return inv;
    }

    PreciseInterval sqr() const {
        PreciseInterval sqr;
        mpfi_sqr(sqr.interval, interval);
        return sqr;
    }

    PreciseInterval sqrt() const {
        PreciseInterval sqrt;
        mpfi_sqrt(sqrt.interval, interval);
        return sqrt;
    }

    PreciseInterval cos() const {
        PreciseInterval cos;
        mpfi_cos(cos.interval, interval);
        return cos;
    }

    PreciseInterval sin() const {
        PreciseInterval sin;
        mpfi_sin(sin.interval, interval);
        return sin;
    }

    static PreciseInterval unit() {
        PreciseInterval unit;
        mpfi_interv_si(unit.interval, 0, 1);
        return unit;
    }

    static PreciseInterval pi() {
        PreciseInterval pi;
        mpfi_const_pi(pi.interval);
        return pi;
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

static_assert(IntervalType<PreciseInterval>);
