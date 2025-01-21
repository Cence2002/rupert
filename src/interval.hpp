#pragma once

#include <boost/numeric/interval.hpp>

template<typename T>
concept IntervalType =
        requires(const T a) {
            { a.min() } -> std::same_as<double>;
            { a.max() } -> std::same_as<double>;
            { a.pos() } -> std::same_as<bool>;
            { a.neg() } -> std::same_as<bool>;

            { +a } -> std::same_as<T>;
            { -a } -> std::same_as<T>;
            { a.sqrt() } -> std::same_as<T>;
            { a.cos() } -> std::same_as<T>;
            { a.sin() } -> std::same_as<T>;
        } &&
        requires(const T a, const T b) {
            { a + b } -> std::same_as<T>;
            { a - b } -> std::same_as<T>;
            { a * b } -> std::same_as<T>;
            { a / b } -> std::same_as<T>;
        } &&
        requires(const T a, const double b) {
            { a + b } -> std::same_as<T>;
            { a - b } -> std::same_as<T>;
            { a * b } -> std::same_as<T>;
            { a / b } -> std::same_as<T>;
        } &&
        requires(const double a, const T b) {
            { a + b } -> std::same_as<T>;
            { a - b } -> std::same_as<T>;
            { a * b } -> std::same_as<T>;
            { a / b } -> std::same_as<T>;
        } &&
        std::is_constructible_v<T, double> &&
        std::is_constructible_v<T, double, double>;

class PointInterval {
    double val;

public:
    explicit PointInterval(const double val) : val(val) {}

    explicit PointInterval(const double min, const double max) : val((min + max) / 2) {}

    double min() const {
        return val;
    }

    double max() const {
        return val;
    }

    bool pos() const {
        return val > 0;
    }

    bool neg() const {
        return val < 0;
    }

    PointInterval operator+() const {
        return PointInterval(+val);
    }

    PointInterval operator-() const {
        return PointInterval(-val);
    }

    PointInterval sqrt() const {
        return PointInterval(std::sqrt(val));
    }

    PointInterval cos() const {
        return PointInterval(std::cos(val));
    }

    PointInterval sin() const {
        return PointInterval(std::sin(val));
    }

    PointInterval operator+(const PointInterval &other) const {
        return PointInterval(val + other.val);
    }

    PointInterval operator-(const PointInterval &other) const {
        return PointInterval(val - other.val);
    }

    PointInterval operator*(const PointInterval &other) const {
        return PointInterval(val * other.val);
    }

    PointInterval operator/(const PointInterval &other) const {
        return PointInterval(val / other.val);
    }

    PointInterval operator+(const double other) const {
        return PointInterval(val + other);
    }

    PointInterval operator-(const double other) const {
        return PointInterval(val - other);
    }

    PointInterval operator*(const double other) const {
        return PointInterval(val * other);
    }

    PointInterval operator/(const double other) const {
        return PointInterval(val / other);
    }
};

inline PointInterval operator+(const double a, const PointInterval &b) {
    return PointInterval(a) + b;
}

inline PointInterval operator-(const double a, const PointInterval &b) {
    return PointInterval(a) - b;
}

inline PointInterval operator*(const double a, const PointInterval &b) {
    return PointInterval(a) * b;
}

inline PointInterval operator/(const double a, const PointInterval &b) {
    return PointInterval(a) / b;
}

class BoostInterval {
    using BoostIntervalType = boost::numeric::interval<
        double,
        boost::numeric::interval_lib::policies<
            boost::numeric::interval_lib::save_state<
                boost::numeric::interval_lib::rounded_transc_std<double>>,
            boost::numeric::interval_lib::checking_base<double>>>;

    BoostIntervalType interval;

public:
    explicit BoostInterval(const double val) : interval(val) {}

    explicit BoostInterval(const double min, const double max) : interval(min, max) {}

    explicit BoostInterval(const BoostIntervalType &interval) : interval(interval) {}

    double min() const {
        return interval.lower();
    }

    double max() const {
        return interval.upper();
    }

    bool pos() const {
        return interval.lower() > 0;
    }

    bool neg() const {
        return interval.upper() < 0;
    }

    BoostInterval operator+() const {
        return BoostInterval(+interval);
    }

    BoostInterval operator-() const {
        return BoostInterval(-interval);
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

    BoostInterval operator+(const BoostInterval &other) const {
        return BoostInterval(interval + other.interval);
    }

    BoostInterval operator-(const BoostInterval &other) const {
        return BoostInterval(interval - other.interval);
    }

    BoostInterval operator*(const BoostInterval &other) const {
        return BoostInterval(interval * other.interval);
    }

    BoostInterval operator/(const BoostInterval &other) const {
        return BoostInterval(interval / other.interval);
    }

    BoostInterval operator+(const double other) const {
        return BoostInterval(interval + other);
    }

    BoostInterval operator-(const double other) const {
        return BoostInterval(interval - other);
    }

    BoostInterval operator*(const double other) const {
        return BoostInterval(interval * other);
    }

    BoostInterval operator/(const double other) const {
        return BoostInterval(interval / other);
    }
};

inline BoostInterval operator+(const double a, const BoostInterval &b) {
    return BoostInterval(a) + b;
}

inline BoostInterval operator-(const double a, const BoostInterval &b) {
    return BoostInterval(a) - b;
}

inline BoostInterval operator*(const double a, const BoostInterval &b) {
    return BoostInterval(a) * b;
}

inline BoostInterval operator/(const double a, const BoostInterval &b) {
    return BoostInterval(a) / b;
}
