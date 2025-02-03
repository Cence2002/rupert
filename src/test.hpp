#pragma once

#include "util.hpp"
#include "geometry/geometry.hpp"
#include "cover.hpp"
#include <random>

struct RandomNumberGenerator {
private:
    static constexpr uint32_t default_seed = 42;
    std::mt19937_64 engine{};

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

inline void tests() {
    const auto start = current_time();

    // TODO: Add MUCH MUCH more tests, potentially visual ones

    print("All tests passed in", elapsed_milliseconds(start), "ms");
}
