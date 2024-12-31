#pragma once

#include "base.hpp"
#include <random>

class RandomNumberGenerator {
    static constexpr uint32_t default_seed = 42;
    std::mt19937_64 engine;

public:
    RandomNumberGenerator() : engine(default_seed) {}

    void seed(const std::mt19937_64::result_type seed) {
        engine.seed(seed);
    }

    double uniform(const double min, const double max) {
        std::uniform_real_distribution<double> distribution(min, max);
        return distribution(engine);
    }
};

inline bool is_close(const double value, const double target_value, const double absolute_tolerance = 1e-4, const double relative_tolerance = 1e-4) {
    return std::abs(value - target_value) < absolute_tolerance + relative_tolerance * std::abs(target_value);
}

inline void test_combined_amplitude_phase() {
    RandomNumberGenerator rng;
    for(int t = 0; t < 10000; t++) {
        const double sine_amplitude = rng.uniform(-10, 10);
        const double cosine_amplitude = rng.uniform(-10, 10);
        const double angle = rng.uniform(0, 2 * M_PI);
        const double target_value = sine_amplitude * std::sin(angle) + cosine_amplitude * std::cos(angle);

        SineWave sine_wave = SineWave::combine(sine_amplitude, cosine_amplitude);
        const double value = sine_wave(angle);

        assert(is_close(value, target_value));
    }
}

inline void test_angle() {
    RandomNumberGenerator rng;
    for(int t = 0; t < 10000; t++) {
        Vector2 v(rng.uniform(-10, 10), rng.uniform(-10, 10));
        v.normalize();

        const double angle = v.get_angle();
        const double x = std::cos(angle);
        const double y = std::sin(angle);

        assert(angle >= 0);
        assert(angle < 2 * M_PI);
        assert(is_close(v.x, x));
        assert(is_close(v.y, y));
    }
}

inline void test_intersections() {
    assert(0);
}
