#pragma once


class RandomNumberGenerator {
    static constexpr uint32_t default_seed = 42;
    std::mt19937 engine;

public:
    RandomNumberGenerator() : engine(default_seed) {}

    void seed(uint32_t seed) {
        engine.seed(seed);
    }

    float uniform(float min, float max) {
        std::uniform_real_distribution<float> distribution(min, max);
        return distribution(engine);
    }
};

inline bool is_close(float value, float target_value, float absolute_tolerance = 1e-4, float relative_tolerance = 1e-4) {
    return std::abs(value - target_value) < absolute_tolerance + relative_tolerance * std::abs(target_value);
}

inline void test_combined_amplitude_phase() {
    RandomNumberGenerator rng;
    for(int t = 0; t < 10000; t++) {
        float sine_amplitude = rng.uniform(-10, 10);
        float cosine_amplitude = rng.uniform(-10, 10);
        float angle = rng.uniform(0, 2 * M_PI);
        float target_value = sine_amplitude * std::sin(angle) + cosine_amplitude * std::cos(angle);

        SineWave sine_wave = SineWave::combine(sine_amplitude, cosine_amplitude);
        float value = sine_wave(angle);

        assert(is_close(value, target_value));
    }
}
