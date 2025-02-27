#pragma once

#include "util.hpp"
#include "geometry/geometry.hpp"
#include "cover.hpp"
#include <random>
#include <thread>

struct RandomNumberGenerator {
private:
    static constexpr uint32_t default_seed = 42;
    std::mt19937_64 engine;

public:
    RandomNumberGenerator() : engine(default_seed) {}

    explicit RandomNumberGenerator(const std::mt19937_64::result_type seed) : engine(seed) {}

    void seed(const std::mt19937_64::result_type seed) {
        engine.seed(seed);
    }

    double uniform_float(const double min, const double max) {
        std::uniform_real_distribution<double> distribution(min, max);
        return distribution(engine);
    }

    double uniform_float(const double max) {
        return uniform_float(0, max);
    }

    double uniform_float() {
        return uniform_float(0, 1);
    }

    int uniform_int(const int min, const int max) {
        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(engine);
    }

    int uniform_int(const int max) {
        return uniform_int(0, max);
    }
};

inline void assert_log(const bool condition, const std::string& message) {
    if(!condition) {
        throw std::runtime_error(message);
    }
}

inline bool compare_bool(const bool bool_0, const bool bool_1) {
    return bool_0 == bool_1;
}

inline bool compare_float(const double float_0, const double float_1, const double tolerance = 1e-6) {
    if(std::isnan(float_0) && std::isnan(float_1)) {
        return true;
    }
    if(std::isinf(float_0) && std::isinf(float_1) && std::signbit(float_0) == std::signbit(float_1)) {
        return true;
    }
    return std::abs(float_0 - float_1) < tolerance;
}

template<IntervalType Interval_0, IntervalType Interval_1>
struct IntervalValidator {
private:
    RandomNumberGenerator rng_;
    double tolerance_;

public:
    using Number_0 = typename Interval_0::Number;
    using Number_1 = typename Interval_1::Number;

    explicit IntervalValidator(const double tolerance = 1e-6) : rng_(), tolerance_(tolerance) {}

    std::pair<int, int> random_integers(const int resolution) {
        const int integer = rng_.uniform_int(-resolution, resolution);
        return {integer, integer};
    }

    std::pair<typename Interval_0::Number, typename Interval_1::Number> random_numbers(const int resolution) {
        if(rng_.uniform_int(0, 1000) == 0) {
            return {Number_0::nan(), Number_1::nan()};
        }
        const auto [integer_0, integer_1] = random_integers(resolution);
        return {
            typename Interval_0::Number(integer_0),
            typename Interval_1::Number(integer_1)
        };
    }

    std::pair<Interval_0, Interval_1> random_intervals(const int resolution) {
        const auto [min_0, min_1] = random_numbers(resolution);
        const auto [max_0, max_1] = random_numbers(resolution);
        if(min_0.is_nan() || max_0.is_nan() || min_1.is_nan() || max_1.is_nan()) {
            return {
                Interval_0::nan(),
                Interval_1::nan()
            };
        }
        const bool swap_0 = min_0 > max_0;
        const bool swap_1 = min_1 > max_1;
        return {
            Interval_0(swap_0 ? max_0 : min_0, swap_0 ? min_0 : max_0),
            Interval_1(swap_1 ? max_1 : min_1, swap_1 ? min_1 : max_1)
        };
    }

    bool compare_number(const typename Interval_0::Number& number0, const typename Interval_1::Number& number1) {
        return compare_float(number0.float_value(), number1.float_value(), tolerance_);
    }

    bool compare_interval(const Interval_0& interval_0, const Interval_1& interval_1) {
        return compare_number(interval_0.min(), interval_1.min()) && compare_number(interval_0.max(), interval_1.max());
    }

    void validate(const int iterations, const int resolution) {
        for(int iteration = 0; iteration < iterations; iteration++) {
            auto [interval_0, interval_1] = random_intervals(resolution);
            interval_0 = interval_0 / (resolution / 10);
            interval_1 = interval_1 / (resolution / 10);
            auto [other_interval_0, other_interval_1] = random_intervals(resolution);
            other_interval_0 = other_interval_0 / (resolution / 10);
            other_interval_1 = other_interval_1 / (resolution / 10);
            auto [number_0, number_1] = random_numbers(10);
            auto [integer_0, integer_1] = random_integers(10);

            if(!compare_interval(interval_0, interval_1)) {
                print("(", Interval_0::name, ")", interval_0, "    <====>    ", "(", Interval_1::name, ")", interval_1);
            }

            if(!compare_number(interval_0.min(), interval_1.min())) {
                print("(", Interval_0::name, ")", interval_0, ".min()", "  ==>  ", interval_0.min(), "    <====>    ", interval_1.min(), "  <==  ", "(", Interval_1::name, ")", interval_1, ".min()");
            }
            if(!compare_number(interval_0.max(), interval_1.max())) {
                print("(", Interval_0::name, ")", interval_0, ".max()", "  ==>  ", interval_0.max(), "    <====>    ", interval_1.max(), "  <==  ", "(", Interval_1::name, ")", interval_1, ".max()");
            }
            if(!compare_number(interval_0.mid(), interval_1.mid())) {
                print("(", Interval_0::name, ")", interval_0, ".mid()", "  ==>  ", interval_0.mid(), "    <====>    ", interval_1.mid(), "  <==  ", "(", Interval_1::name, ")", interval_1, ".mid()");
            }
            if(!compare_number(interval_0.len(), interval_1.len())) {
                print("(", Interval_0::name, ")", interval_0, ".len()", "  ==>  ", interval_0.len(), "    <====>    ", interval_1.len(), "  <==  ", "(", Interval_1::name, ")", interval_1, ".len()");
            }
            if(!compare_number(interval_0.rad(), interval_1.rad())) {
                print("(", Interval_0::name, ")", interval_0, ".rad()", "  ==>  ", interval_0.rad(), "    <====>    ", interval_1.rad(), "  <==  ", "(", Interval_1::name, ")", interval_1, ".rad()");
            }

            if(!compare_bool(interval_0.is_nonzero(), interval_1.is_nonzero())) {
                print("(", Interval_0::name, ")", interval_0, ".is_nonzero()", "  ==>  ", interval_0.is_nonzero(), "    <====>    ", interval_1.is_nonzero(), "  <==  ", "(", Interval_1::name, ")", interval_1, ".is_nonzero()");
            }
            if(!compare_bool(interval_0.is_pos(), interval_1.is_pos())) {
                print("(", Interval_0::name, ")", interval_0, ".is_pos()", "  ==>  ", interval_0.is_pos(), "    <====>    ", interval_1.is_pos(), "  <==  ", "(", Interval_1::name, ")", interval_1, ".is_pos()");
            }
            if(!compare_bool(interval_0.is_neg(), interval_1.is_neg())) {
                print("(", Interval_0::name, ")", interval_0, ".is_neg()", "  ==>  ", interval_0.is_neg(), "    <====>    ", interval_1.is_neg(), "  <==  ", "(", Interval_1::name, ")", interval_1, ".is_neg()");
            }
            if(!compare_bool(interval_0.is_nan(), interval_1.is_nan())) {
                print("(", Interval_0::name, ")", interval_0, ".is_nan()", "  ==>  ", interval_0.is_nan(), "    <====>    ", interval_1.is_nan(), "  <==  ", "(", Interval_1::name, ")", interval_1, ".is_nan()");
            }

            if(!compare_bool(interval_0 > other_interval_0, interval_1 > other_interval_1)) {
                print("(", Interval_0::name, ")", interval_0, " > ", "(", Interval_0::name, ")", other_interval_0, "  ==>  ", interval_0 > other_interval_0, "    <====>    ", interval_1 > other_interval_1, "  <==  ", "(", Interval_1::name, ")", interval_1, " > ", "(", Interval_1::name, ")", other_interval_1);
            }
            if(!compare_bool(interval_0 > number_0, interval_1 > number_1)) {
                print("(", Interval_0::name, ")", interval_0, " > ", "(", Number_0::name, ")", number_0, "  ==>  ", interval_0 > number_0, "    <====>    ", interval_1 > number_1, "  <==  ", "(", Interval_1::name, ")", interval_1, " > ", "(", Number_1::name, ")", number_1);
            }
            if(!compare_bool(number_0 > interval_0, number_1 > interval_1)) {
                print("(", Number_0::name, ")", number_0, " > ", "(", Interval_0::name, ")", interval_0, "  ==>  ", number_0 > interval_0, "    <====>    ", number_1 > interval_1, "  <==  ", "(", Number_1::name, ")", number_1, " > ", "(", Interval_1::name, ")", interval_1);
            }
            if(!compare_bool(interval_0 > integer_0, interval_1 > integer_1)) {
                print("(", Interval_0::name, ")", interval_0, " > ", "(int)", integer_0, "  ==>  ", interval_0 > integer_0, "    <====>    ", interval_1 > integer_1, "  <==  ", "(", Interval_1::name, ")", interval_1, " > ", "(int)", integer_1);
            }
            if(!compare_bool(integer_0 > interval_0, integer_1 > interval_1)) {
                print("(int)", integer_0, " > ", "(", Interval_0::name, ")", interval_0, "  ==>  ", integer_0 > interval_0, "    <====>    ", integer_1 > interval_1, "  <==  ", "(int)", integer_1, " > ", "(", Interval_1::name, ")", interval_1);
            }

            if(!compare_bool(interval_0 < other_interval_0, interval_1 < other_interval_1)) {
                print("(", Interval_0::name, ")", interval_0, " < ", "(", Interval_0::name, ")", other_interval_0, "  ==>  ", interval_0 < other_interval_0, "    <====>    ", interval_1 < other_interval_1, "  <==  ", "(", Interval_1::name, ")", interval_1, " < ", "(", Interval_1::name, ")", other_interval_1);
            }
            if(!compare_bool(interval_0 < number_0, interval_1 < number_1)) {
                print("(", Interval_0::name, ")", interval_0, " < ", "(", Number_0::name, ")", number_0, "  ==>  ", interval_0 < number_0, "    <====>    ", interval_1 < number_1, "  <==  ", "(", Interval_1::name, ")", interval_1, " < ", "(", Number_1::name, ")", number_1);
            }
            if(!compare_bool(number_0 < interval_0, number_1 < interval_1)) {
                print("(", Number_0::name, ")", number_0, " < ", "(", Interval_0::name, ")", interval_0, "  ==>  ", number_0 < interval_0, "    <====>    ", number_1 < interval_1, "  <==  ", "(", Number_1::name, ")", number_1, " < ", "(", Interval_1::name, ")", interval_1);
            }
            if(!compare_bool(interval_0 < integer_0, interval_1 < integer_1)) {
                print("(", Interval_0::name, ")", interval_0, " < ", "(int)", integer_0, "  ==>  ", interval_0 < integer_0, "    <====>    ", interval_1 < integer_1, "  <==  ", "(", Interval_1::name, ")", interval_1, " < ", "(int)", integer_1);
            }
            if(!compare_bool(integer_0 < interval_0, integer_1 < interval_1)) {
                print("(int)", integer_0, " < ", "(", Interval_0::name, ")", interval_0, "  ==>  ", integer_0 < interval_0, "    <====>    ", integer_1 < interval_1, "  <==  ", "(int)", integer_1, " < ", "(", Interval_1::name, ")", interval_1);
            }

            if(!compare_interval(+interval_0, +interval_1)) {
                print("+", "(", Interval_0::name, ")", interval_0, "  ==>  ", +interval_0, "    <====>    ", +interval_1, "  <==  ", "+", "(", Interval_1::name, ")", interval_1);
            }
            if(!compare_interval(-interval_0, -interval_1)) {
                print("-", "(", Interval_0::name, ")", interval_0, "  ==>  ", -interval_0, "    <====>    ", -interval_1, "  <==  ", "-", "(", Interval_1::name, ")", interval_1);
            }

            if(!compare_interval(interval_0 + other_interval_0, interval_1 + other_interval_1)) {
                print("(", Interval_0::name, ")", interval_0, " + ", "(", Interval_0::name, ")", other_interval_0, "  ==>  ", interval_0 + other_interval_0, "    <====>    ", interval_1 + other_interval_1, "  <==  ", "(", Interval_1::name, ")", interval_1, " + ", "(", Interval_1::name, ")", other_interval_1);
            }
            if(!compare_interval(interval_0 + number_0, interval_1 + number_1)) {
                print("(", Interval_0::name, ")", interval_0, " + ", "(", Number_0::name, ")", number_0, "  ==>  ", interval_0 + number_0, "    <====>    ", interval_1 + number_1, "  <==  ", "(", Interval_1::name, ")", interval_1, " + ", "(", Number_1::name, ")", number_1);
            }
            if(!compare_interval(number_0 + interval_0, number_1 + interval_1)) {
                print("(", Number_0::name, ")", number_0, " + ", "(", Interval_0::name, ")", interval_0, "  ==>  ", number_0 + interval_0, "    <====>    ", number_1 + interval_1, "  <==  ", "(", Number_1::name, ")", number_1, " + ", "(", Interval_1::name, ")", interval_1);
            }
            if(!compare_interval(interval_0 + integer_0, interval_1 + integer_1)) {
                print("(", Interval_0::name, ")", interval_0, " + ", "(int)", integer_0, "  ==>  ", interval_0 + integer_0, "    <====>    ", interval_1 + integer_1, "  <==  ", "(", Interval_1::name, ")", interval_1, " + ", "(int)", integer_1);
            }
            if(!compare_interval(integer_0 + interval_0, integer_1 + interval_1)) {
                print("(int)", integer_0, " + ", "(", Interval_0::name, ")", interval_0, "  ==>  ", integer_0 + interval_0, "    <====>    ", integer_1 + interval_1, "  <==  ", "(int)", integer_1, " + ", "(", Interval_1::name, ")", interval_1);
            }

            if(!compare_interval(interval_0 - other_interval_0, interval_1 - other_interval_1)) {
                print("(", Interval_0::name, ")", interval_0, " - ", "(", Interval_0::name, ")", other_interval_0, "  ==>  ", interval_0 - other_interval_0, "    <====>    ", interval_1 - other_interval_1, "  <==  ", "(", Interval_1::name, ")", interval_1, " - ", "(", Interval_1::name, ")", other_interval_1);
            }
            if(!compare_interval(interval_0 - number_0, interval_1 - number_1)) {
                print("(", Interval_0::name, ")", interval_0, " - ", "(", Number_0::name, ")", number_0, "  ==>  ", interval_0 - number_0, "    <====>    ", interval_1 - number_1, "  <==  ", "(", Interval_1::name, ")", interval_1, " - ", "(", Number_1::name, ")", number_1);
            }
            if(!compare_interval(number_0 - interval_0, number_1 - interval_1)) {
                print("(", Number_0::name, ")", number_0, " - ", "(", Interval_0::name, ")", interval_0, "  ==>  ", number_0 - interval_0, "    <====>    ", number_1 - interval_1, "  <==  ", "(", Number_1::name, ")", number_1, " - ", "(", Interval_1::name, ")", interval_1);
            }
            if(!compare_interval(interval_0 - integer_0, interval_1 - integer_1)) {
                print("(", Interval_0::name, ")", interval_0, " - ", "(int)", integer_0, "  ==>  ", interval_0 - integer_0, "    <====>    ", interval_1 - integer_1, "  <==  ", "(", Interval_1::name, ")", interval_1, " - ", "(int)", integer_1);
            }
            if(!compare_interval(integer_0 - interval_0, integer_1 - interval_1)) {
                print("(int)", integer_0, " - ", "(", Interval_0::name, ")", interval_0, "  ==>  ", integer_0 - interval_0, "    <====>    ", integer_1 - interval_1, "  <==  ", "(int)", integer_1, " - ", "(", Interval_1::name, ")", interval_1);
            }

            if(!compare_interval(interval_0 * other_interval_0, interval_1 * other_interval_1)) {
                print("(", Interval_0::name, ")", interval_0, " * ", "(", Interval_0::name, ")", other_interval_0, "  ==>  ", interval_0 * other_interval_0, "    <====>    ", interval_1 * other_interval_1, "  <==  ", "(", Interval_1::name, ")", interval_1, " * ", "(", Interval_1::name, ")", other_interval_1);
            }
            if(!compare_interval(interval_0 * number_0, interval_1 * number_1)) {
                print("(", Interval_0::name, ")", interval_0, " * ", "(", Number_0::name, ")", number_0, "  ==>  ", interval_0 * number_0, "    <====>    ", interval_1 * number_1, "  <==  ", "(", Interval_1::name, ")", interval_1, " * ", "(", Number_1::name, ")", number_1);
            }
            if(!compare_interval(number_0 * interval_0, number_1 * interval_1)) {
                print("(", Number_0::name, ")", number_0, " * ", "(", Interval_0::name, ")", interval_0, "  ==>  ", number_0 * interval_0, "    <====>    ", number_1 * interval_1, "  <==  ", "(", Number_1::name, ")", number_1, " * ", "(", Interval_1::name, ")", interval_1);
            }
            if(!compare_interval(interval_0 * integer_0, interval_1 * integer_1)) {
                print("(", Interval_0::name, ")", interval_0, " * ", "(int)", integer_0, "  ==>  ", interval_0 * integer_0, "    <====>    ", interval_1 * integer_1, "  <==  ", "(", Interval_1::name, ")", interval_1, " * ", "(int)", integer_1);
            }
            if(!compare_interval(integer_0 * interval_0, integer_1 * interval_1)) {
                print("(int)", integer_0, " * ", "(", Interval_0::name, ")", interval_0, "  ==>  ", integer_0 * interval_0, "    <====>    ", integer_1 * interval_1, "  <==  ", "(int)", integer_1, " * ", "(", Interval_1::name, ")", interval_1);
            }

            if(!compare_interval(interval_0 / other_interval_0, interval_1 / other_interval_1)) {
                print("(", Interval_0::name, ")", interval_0, " / ", "(", Interval_0::name, ")", other_interval_0, "  ==>  ", interval_0 / other_interval_0, "    <====>    ", interval_1 / other_interval_1, "  <==  ", "(", Interval_1::name, ")", interval_1, " / ", "(", Interval_1::name, ")", other_interval_1);
            }
            if(!compare_interval(interval_0 / number_0, interval_1 / number_1)) {
                print("(", Interval_0::name, ")", interval_0, " / ", "(", Number_0::name, ")", number_0, "  ==>  ", interval_0 / number_0, "    <====>    ", interval_1 / number_1, "  <==  ", "(", Interval_1::name, ")", interval_1, " / ", "(", Number_1::name, ")", number_1);
            }
            if(!compare_interval(number_0 / interval_0, number_1 / interval_1)) {
                print("(", Number_0::name, ")", number_0, " / ", "(", Interval_0::name, ")", interval_0, "  ==>  ", number_0 / interval_0, "    <====>    ", number_1 / interval_1, "  <==  ", "(", Number_1::name, ")", number_1, " / ", "(", Interval_1::name, ")", interval_1);
            }
            if(!compare_interval(interval_0 / integer_0, interval_1 / integer_1)) {
                print("(", Interval_0::name, ")", interval_0, " / ", "(int)", integer_0, "  ==>  ", interval_0 / integer_0, "    <====>    ", interval_1 / integer_1, "  <==  ", "(", Interval_1::name, ")", interval_1, " / ", "(int)", integer_1);
            }
            if(!compare_interval(integer_0 / interval_0, integer_1 / interval_1)) {
                print("(int)", integer_0, " / ", "(", Interval_0::name, ")", interval_0, "  ==>  ", integer_0 / interval_0, "    <====>    ", integer_1 / interval_1, "  <==  ", "(int)", integer_1, " / ", "(", Interval_1::name, ")", interval_1);
            }

            if(!compare_interval(interval_0.inv(), interval_1.inv())) {
                print("(", Interval_0::name, ")", interval_0, ".inv()", "  ==>  ", interval_0.inv(), "    <====>    ", interval_1.inv(), "  <==  ", "(", Interval_1::name, ")", interval_1, ".inv()");
            }
            if(!compare_interval(interval_0.sqr(), interval_1.sqr())) {
                print("(", Interval_0::name, ")", interval_0, ".sqr()", "  ==>  ", interval_0.sqr(), "    <====>    ", interval_1.sqr(), "  <==  ", "(", Interval_1::name, ")", interval_1, ".sqr()");
            }
            if(!compare_interval(interval_0.sqrt(), interval_1.sqrt())) {
                print("(", Interval_0::name, ")", interval_0, ".sqrt()", "  ==>  ", interval_0.sqrt(), "    <====>    ", interval_1.sqrt(), "  <==  ", "(", Interval_1::name, ")", interval_1, ".sqrt()");
            }

            if(!compare_interval(interval_0.cos(), interval_1.cos())) {
                print("(", Interval_0::name, ")", interval_0, ".cos()", "  ==>  ", interval_0.cos(), "    <====>    ", interval_1.cos(), "  <==  ", "(", Interval_1::name, ")", interval_1, ".cos()");
            }
            if(!compare_interval(interval_0.sin(), interval_1.sin())) {
                print("(", Interval_0::name, ")", interval_0, ".sin()", "  ==>  ", interval_0.sin(), "    <====>    ", interval_1.sin(), "  <==  ", "(", Interval_1::name, ")", interval_1, ".sin()");
            }
            if(!compare_interval(interval_0.atan(), interval_1.atan())) {
                print("(", Interval_0::name, ")", interval_0, ".atan()", "  ==>  ", interval_0.atan(), "    <====>    ", interval_1.atan(), "  <==  ", "(", Interval_1::name, ")", interval_1, ".atan()");
            }
        }
        if(!compare_interval(Interval_0::unit(), Interval_1::unit())) {
            print("(", Interval_0::name, ")", ".unit()", "  ==>  ", Interval_0::unit(), "    <====>    ", Interval_1::unit(), "  <==  ", "(", Interval_1::name, ")", ".unit()");
        }
        if(!compare_interval(Interval_0::pi(), Interval_1::pi())) {
            print("(", Interval_0::name, ")", ".pi()", "  ==>  ", Interval_0::pi(), "    <====>    ", Interval_1::pi(), "  <==  ", "(", Interval_1::name, ")", ".pi()");
        }
    }
};

inline void tests(const int iterations = 10000, const int resolution = 100, const double tolerance = 1e-12) {
    const auto start = current_time();

    std::thread thread([&] {
        IntervalValidator<FloatInterval, BoostInterval> float_interval_vs_boost_interval(tolerance);
        float_interval_vs_boost_interval.validate(iterations, resolution);

        IntervalValidator<FloatInterval, MpfiInterval> float_interval_vs_mpfi_interval(tolerance);
        float_interval_vs_mpfi_interval.validate(iterations, resolution);

        IntervalValidator<BoostInterval, MpfiInterval> boost_interval_vs_mpfi_interval(tolerance);
        boost_interval_vs_mpfi_interval.validate(iterations, resolution);

        mpfr_free_cache2(MPFR_FREE_LOCAL_CACHE);
    });
    thread.join();

    print("All tests passed in ", elapsed_milliseconds(start), " ms");
}
