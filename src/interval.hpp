#pragma once

#include <iostream>
#include <boost/numeric/interval.hpp>
#include <mpfr.h>
#include <mpfi.h>

enum class IntervalPrintMode {
    MIN_AND_MAX,
    MID_AND_RAD,
};

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

        requires(const I a, std::ostream& os) {
            { os << a } -> std::same_as<std::ostream&>;
        };

template<typename Int>
concept IntegerType = std::is_integral_v<Int>;

struct FastInterval {
private:
    double min_;
    double max_;

    static inline IntervalPrintMode print_mode = IntervalPrintMode::MIN_AND_MAX;

    explicit FastInterval(const double min, const double max) : min_(min), max_(max) {}

#undef NAN
    static constexpr double NAN = std::numeric_limits<double>::quiet_NaN();
    static constexpr double INF = std::numeric_limits<double>::infinity();
    static constexpr double PI = std::numbers::pi_v<double>;
    static constexpr double TWO_PI = 2 * std::numbers::pi_v<double>;
    static constexpr double HALF_PI = std::numbers::pi_v<double> / 2;

public:
    explicit FastInterval() : min_(NAN), max_(NAN) {}

    template<IntegerType Int>
    explicit FastInterval(const Int val) : min_(static_cast<double>(val)), max_(static_cast<double>(val)) {}

    template<IntegerType Int>
    explicit FastInterval(const Int min, const Int max) : min_(static_cast<double>(min)), max_(static_cast<double>(max)) {}

    ~FastInterval() = default;

    FastInterval(const FastInterval& i) = default;

    FastInterval& operator=(const FastInterval& i) = default;

    FastInterval(FastInterval&& i) = default;

    FastInterval& operator=(FastInterval&& i) = default;

    FastInterval min() const {
        return FastInterval(min_, min_);
    }

    FastInterval max() const {
        return FastInterval(max_, max_);
    }

    FastInterval mid() const {
        const double mid = (min_ + max_) / 2;
        return FastInterval(mid, mid);
    }

    FastInterval len() const {
        const double len = max_ - min_;
        return FastInterval(len, len);
    }

    FastInterval rad() const {
        const double rad = (max_ - min_) / 2;
        return FastInterval(rad, rad);
    }

    bool pos() const {
        return 0 < min_;
    }

    bool neg() const {
        return max_ < 0;
    }

    bool operator>(const FastInterval& i) const {
        return min_ > i.max_;
    }

    template<IntegerType Int>
    bool operator>(const Int n) const {
        return min_ > static_cast<double>(n);
    }

    template<IntegerType Int>
    friend bool operator>(const Int n, const FastInterval& i) {
        return static_cast<double>(n) > i.max_;
    }

    bool operator<(const FastInterval& i) const {
        return max_ < i.min_;
    }

    template<IntegerType Int>
    bool operator<(const Int n) const {
        return max_ < static_cast<double>(n);
    }

    template<IntegerType Int>
    friend bool operator<(const Int n, const FastInterval& i) {
        return static_cast<double>(n) < i.min_;
    }

    FastInterval operator+() const {
        return FastInterval(+min_, +max_);
    }

    FastInterval operator-() const {
        return FastInterval(-max_, -min_);
    }

    FastInterval operator+(const FastInterval& i) const {
        return FastInterval(min_ + i.min_, max_ + i.max_);
    }

    template<IntegerType Int>
    FastInterval operator+(const Int n) const {
        return FastInterval(min_ + static_cast<double>(n), max_ + static_cast<double>(n));
    }

    template<IntegerType Int>
    friend FastInterval operator+(const Int n, const FastInterval& i) {
        return FastInterval(static_cast<double>(n) + i.min_, static_cast<double>(n) + i.max_);
    }

    FastInterval operator-(const FastInterval& i) const {
        return FastInterval(min_ - i.max_, max_ - i.min_);
    }

    template<IntegerType Int>
    FastInterval operator-(const Int n) const {
        return FastInterval(min_ - static_cast<double>(n), max_ - static_cast<double>(n));
    }

    template<IntegerType Int>
    friend FastInterval operator-(const Int n, const FastInterval& i) {
        return FastInterval(static_cast<double>(n) - i.max_, static_cast<double>(n) - i.min_);
    }

    FastInterval operator*(const FastInterval& i) const {
        const double min_min = min_ * i.min_;
        const double min_max = min_ * i.max_;
        const double max_min = max_ * i.min_;
        const double max_max = max_ * i.max_;
        return FastInterval(std::min({min_min, min_max, max_min, max_max}),
                            std::max({min_min, min_max, max_min, max_max}));
    }

    template<IntegerType Int>
    FastInterval operator*(const Int n) const {
        const double min_n = min_ * static_cast<double>(n);
        const double max_n = max_ * static_cast<double>(n);
        return FastInterval(std::min(min_n, max_n), std::max(min_n, max_n));
    }

    template<IntegerType Int>
    friend FastInterval operator*(const Int n, const FastInterval& i) {
        const double n_min = static_cast<double>(n) * i.min_;
        const double n_max = static_cast<double>(n) * i.max_;
        return FastInterval(std::min(n_min, n_max), std::max(n_min, n_max));
    }

    FastInterval operator/(const FastInterval& i) const {
        if(i.min_ <= 0 && 0 <= i.max_) {
            return FastInterval(-INF, INF);
        }
        const double min_min = min_ / i.min_;
        const double min_max = min_ / i.max_;
        const double max_min = max_ / i.min_;
        const double max_max = max_ / i.max_;
        return FastInterval(std::min({min_min, min_max, max_min, max_max}),
                            std::max({min_min, min_max, max_min, max_max}));
    }

    template<IntegerType Int>
    FastInterval operator/(const Int n) const {
        if(n == 0) {
            return FastInterval(-INF, INF);
        }
        const double min_n = min_ / static_cast<double>(n);
        const double max_n = max_ / static_cast<double>(n);
        return FastInterval(std::min(min_n, max_n), std::max(min_n, max_n));
    }

    template<IntegerType Int>
    friend FastInterval operator/(const Int n, const FastInterval& i) {
        if(i.min_ <= 0 && 0 <= i.max_) {
            return FastInterval(-INF, INF);
        }
        const double n_min = static_cast<double>(n) / i.min_;
        const double n_max = static_cast<double>(n) / i.max_;
        return FastInterval(std::min(n_min, n_max), std::max(n_min, n_max));
    }

    FastInterval inv() const {
        if(min_ <= 0 && 0 <= max_) {
            return FastInterval(-INF, INF);
        }
        return FastInterval(1.0 / max_, 1.0 / min_);
    }

    FastInterval sqr() const {
        if(0 <= min_) {
            return FastInterval(min_ * min_, max_ * max_);
        }
        if(max_ <= 0) {
            return FastInterval(max_ * max_, min_ * min_);
        }
        return FastInterval(0, std::max(min_ * min_, max_ * max_));
    }

    FastInterval sqrt() const {
        if(min_ < 0) {
            return FastInterval(NAN, NAN);
        }
        return FastInterval(std::sqrt(min_), std::sqrt(max_));
    }

    FastInterval cos() const {
        const double cos_min = std::cos(min_);
        const double cos_max = std::cos(max_);
        double min = std::min(cos_min, cos_max);
        double max = std::max(cos_min, cos_max);
        if(std::floor(min_ / TWO_PI) != std::floor(max_ / TWO_PI)) {
            max = 1.0;
        }
        if(std::floor((min_ - PI) / TWO_PI) != std::floor((max_ - PI) / TWO_PI)) {
            min = -1.0;
        }
        return FastInterval(min, max);
    }

    FastInterval sin() const {
        const double sin_min = std::sin(min_);
        const double sin_max = std::sin(max_);
        double min = std::min(sin_min, sin_max);
        double max = std::max(sin_min, sin_max);
        if(std::floor((min_ - HALF_PI) / TWO_PI) < std::floor((max_ - HALF_PI) / TWO_PI)) {
            max = 1.0;
        }
        if(std::floor((min_ + HALF_PI) / TWO_PI) < std::floor((max_ + HALF_PI) / TWO_PI)) {
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

    friend std::ostream& operator<<(std::ostream& os, const FastInterval& interval) {
        switch(print_mode) {
            case IntervalPrintMode::MIN_AND_MAX: {
                const double min = interval.min().min_;
                const double max = interval.max().min_;
                return os << "[" << min << " | " << max << "]";
            }
            case IntervalPrintMode::MID_AND_RAD: {
                const double mid = interval.mid().min_;
                const double rad = interval.rad().min_;
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

    BoostIntervalType interval_;

    static inline IntervalPrintMode print_mode = IntervalPrintMode::MIN_AND_MAX;

    explicit BoostInterval(const BoostIntervalType& interval) : interval_(interval) {}

public:
    explicit BoostInterval() : interval_(BoostIntervalType::empty()) {}

    template<IntegerType Int>
    explicit BoostInterval(const Int val) : interval_(val) {}

    template<IntegerType Int>
    explicit BoostInterval(const Int min, const Int max) : interval_(min, max) {}

    ~BoostInterval() = default;

    BoostInterval(const BoostInterval& i) = default;

    BoostInterval& operator=(const BoostInterval& i) = default;

    BoostInterval(BoostInterval&& i) = default;

    BoostInterval& operator=(BoostInterval&& i) = default;

    BoostInterval min() const {
        return BoostInterval(BoostIntervalType(boost::numeric::lower(interval_)));
    }

    BoostInterval max() const {
        return BoostInterval(BoostIntervalType(boost::numeric::upper(interval_)));
    }

    BoostInterval mid() const {
        return BoostInterval(BoostIntervalType(boost::numeric::lower(interval_)) + BoostIntervalType(boost::numeric::upper(interval_)) / 2.0);
    }

    BoostInterval len() const {
        return BoostInterval(BoostIntervalType(boost::numeric::width(interval_)));
    }

    BoostInterval rad() const {
        return BoostInterval(BoostIntervalType(boost::numeric::width(interval_)) / 2.0);
    }

    bool pos() const {
        return boost::numeric::interval_lib::cerlt(0.0, interval_);
    }

    bool neg() const {
        return boost::numeric::interval_lib::cerlt(interval_, 0.0);
    }

    bool operator>(const BoostInterval& i) const {
        return boost::numeric::interval_lib::cergt(interval_, i.interval_);
    }

    template<IntegerType Int>
    bool operator>(const Int n) const {
        return boost::numeric::interval_lib::cergt(interval_, static_cast<double>(n));
    }

    template<IntegerType Int>
    friend bool operator>(const Int n, const BoostInterval& i) {
        return boost::numeric::interval_lib::cergt(static_cast<double>(n), i.interval_);
    }

    bool operator<(const BoostInterval& i) const {
        return boost::numeric::interval_lib::cerlt(interval_, i.interval_);
    }

    template<IntegerType Int>
    bool operator<(const Int n) const {
        return boost::numeric::interval_lib::cerlt(interval_, static_cast<double>(n));
    }

    template<IntegerType Int>
    friend bool operator<(const Int n, const BoostInterval& i) {
        return boost::numeric::interval_lib::cerlt(static_cast<double>(n), i.interval_);
    }

    BoostInterval operator+() const {
        return BoostInterval(+interval_);
    }

    BoostInterval operator-() const {
        return BoostInterval(-interval_);
    }

    BoostInterval operator+(const BoostInterval& i) const {
        return BoostInterval(interval_ + i.interval_);
    }

    template<IntegerType Int>
    BoostInterval operator+(const Int n) const {
        return BoostInterval(interval_ + static_cast<double>(n));
    }

    template<IntegerType Int>
    friend BoostInterval operator+(const Int n, const BoostInterval& i) {
        return BoostInterval(static_cast<double>(n) + i.interval_);
    }

    BoostInterval operator-(const BoostInterval& i) const {
        return BoostInterval(interval_ - i.interval_);
    }

    template<IntegerType Int>
    BoostInterval operator-(const Int n) const {
        return BoostInterval(interval_ - static_cast<double>(n));
    }

    template<IntegerType Int>
    friend BoostInterval operator-(const Int n, const BoostInterval& i) {
        return BoostInterval(static_cast<double>(n) - i.interval_);
    }

    BoostInterval operator*(const BoostInterval& i) const {
        return BoostInterval(interval_ * i.interval_);
    }

    template<IntegerType Int>
    BoostInterval operator*(const Int n) const {
        return BoostInterval(interval_ * static_cast<double>(n));
    }

    template<IntegerType Int>
    friend BoostInterval operator*(const Int n, const BoostInterval& i) {
        return BoostInterval(static_cast<double>(n) * i.interval_);
    }

    BoostInterval operator/(const BoostInterval& i) const {
        return BoostInterval(interval_ / i.interval_);
    }

    template<IntegerType Int>
    BoostInterval operator/(const Int n) const {
        return BoostInterval(interval_ / static_cast<double>(n));
    }

    template<IntegerType Int>
    friend BoostInterval operator/(const Int n, const BoostInterval& i) {
        return BoostInterval(static_cast<double>(n) / i.interval_);
    }

    BoostInterval inv() const {
        return BoostInterval(boost::numeric::interval_lib::multiplicative_inverse(interval_));
    }

    BoostInterval sqr() const {
        return BoostInterval(boost::numeric::square(interval_));
    }

    BoostInterval sqrt() const {
        return BoostInterval(boost::numeric::sqrt(interval_));
    }

    BoostInterval cos() const {
        return BoostInterval(boost::numeric::cos(interval_));
    }

    BoostInterval sin() const {
        return BoostInterval(boost::numeric::sin(interval_));
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

    friend std::ostream& operator<<(std::ostream& os, const BoostInterval& interval) {
        switch(print_mode) {
            case IntervalPrintMode::MIN_AND_MAX: {
                const double min = interval.min().interval_.lower();
                const double max = interval.max().interval_.upper();
                return os << "[" << min << " | " << max << "]";
            }
            case IntervalPrintMode::MID_AND_RAD: {
                const double mid = interval.mid().interval_.lower();
                const double rad = interval.rad().interval_.lower();
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

    mpfi_t interval;

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

    PreciseInterval(const PreciseInterval& i) {
        mpfi_init_set(interval, i.interval);
    }

    PreciseInterval& operator=(const PreciseInterval& i) {
        if(this != &i) {
            mpfi_set(interval, i.interval);
        }
        return *this;
    }

    PreciseInterval(PreciseInterval&& i) noexcept {
        mpfi_init(interval);
        mpfi_swap(interval, i.interval);
    }

    PreciseInterval& operator=(PreciseInterval&& i) noexcept {
        if(this != &i) {
            mpfi_clear(interval);
            mpfi_init(interval);
            mpfi_swap(interval, i.interval);
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

    bool operator>(const PreciseInterval& i) const {
        return mpfi_cmp(interval, i.interval) > 0;
    }

    template<IntegerType Int>
    bool operator>(const Int n) const {
        return mpfi_cmp_si(interval, n) > 0;
    }

    template<IntegerType Int>
    friend bool operator>(const Int n, const PreciseInterval& i) {
        return mpfi_cmp_si(i.interval, n) < 0;
    }

    bool operator<(const PreciseInterval& i) const {
        return mpfi_cmp(interval, i.interval) < 0;
    }

    template<IntegerType Int>
    bool operator<(const Int n) const {
        return mpfi_cmp_si(interval, n) < 0;
    }

    template<IntegerType Int>
    friend bool operator<(const Int n, const PreciseInterval& i) {
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

    PreciseInterval operator+(const PreciseInterval& i) const {
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
    friend PreciseInterval operator+(const Int n, const PreciseInterval& i) {
        PreciseInterval add;
        mpfi_add_si(add.interval, i.interval, n);
        return add;
    }

    PreciseInterval operator-(const PreciseInterval& i) const {
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
    friend PreciseInterval operator-(const Int n, const PreciseInterval& i) {
        PreciseInterval sub;
        mpfi_si_sub(sub.interval, n, i.interval);
        return sub;
    }

    PreciseInterval operator*(const PreciseInterval& i) const {
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
    friend PreciseInterval operator*(const Int n, const PreciseInterval& i) {
        PreciseInterval mul;
        mpfi_mul_si(mul.interval, i.interval, n);
        return mul;
    }

    PreciseInterval operator/(const PreciseInterval& i) const {
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
    friend PreciseInterval operator/(const Int n, const PreciseInterval& i) {
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

    friend std::ostream& operator<<(std::ostream& os, const PreciseInterval& interval) {
        const std::streamsize precision = os.precision();
        switch(print_mode) {
            case IntervalPrintMode::MIN_AND_MAX: {
                mpfr_t min, max;
                mpfr_init(min);
                mpfr_init(max);
                mpfi_get_left(min, interval.interval);
                mpfi_get_right(max, interval.interval);
                char min_str[100];
                char max_str[100];
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
                char mid_str[100];
                char rad_str[100];
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
