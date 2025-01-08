#pragma once

#include "vector.hpp"
#include <random>

class RandomNumberGenerator {
    static constexpr uint32_t default_seed = 42;
    std::mt19937_64 engine;

public:
    RandomNumberGenerator() : engine(default_seed) {}

    explicit RandomNumberGenerator(const std::mt19937_64::result_type seed) : engine(seed) {}

    void seed(const std::mt19937_64::result_type seed) {
        engine.seed(seed);
    }

    double uniform(const double min, const double max) {
        std::uniform_real_distribution<double> distribution(min, max);
        return distribution(engine);
    }
};

inline bool is_close(const double value, const double target_value, const double absolute_tolerance = 1e-6, const double relative_tolerance = 1e-6) {
    return std::abs(value - target_value) < absolute_tolerance + relative_tolerance * std::abs(target_value);
}

inline void test_angle() {
    RandomNumberGenerator rng;
    for(int t = 0; t < 10000; t++) {
        Vector2 v = Vector2d(rng.uniform(-10, 10), rng.uniform(-10, 10)).unit();

        const double angle = v.angle();
        const double x = std::cos(angle);
        const double y = std::sin(angle);

        assert(angle >= 0);
        assert(angle < 2 * M_PI);
        assert(is_close(v.x, x));
        assert(is_close(v.y, y));
    }
}

inline void test_intervals() {
    namespace il = boost::numeric::interval_lib;
    using I = boost::numeric::interval<
        double,
        boost::numeric::interval_lib::policies<
            boost::numeric::interval_lib::save_state<
                boost::numeric::interval_lib::rounded_transc_std<double>>,
            boost::numeric::interval_lib::checking_base<double>>>;
    auto pi = boost::numeric::interval_lib::pi<I>();
    std::cout << std::setprecision(20) << lower(pi) << std::endl;
    std::cout << std::setprecision(20) << upper(pi) << std::endl;
    I sin_pi = boost::numeric::sin(pi);
    std::cout << std::setprecision(20) << lower(sin_pi) << std::endl;
    std::cout << std::setprecision(20) << upper(sin_pi) << std::endl;
    I sin_pi_reciprocal = 1.0 / sin_pi;
    std::cout << std::setprecision(20) << lower(sin_pi_reciprocal) << std::endl;
    std::cout << std::setprecision(20) << upper(sin_pi_reciprocal) << std::endl;
}
