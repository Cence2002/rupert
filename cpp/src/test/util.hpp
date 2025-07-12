#pragma once

#include <iostream>
#include <chrono>
#include <random>

template<typename... Args>
void print(Args&&... args) {
    ((std::cout << args), ...);
    std::cout << std::endl;
}

inline std::chrono::time_point<std::chrono::high_resolution_clock> current_time() {
    return std::chrono::high_resolution_clock::now();
}

inline std::chrono::duration<double>::rep elapsed_time(const std::chrono::time_point<std::chrono::high_resolution_clock>& start) {
    return std::chrono::duration<double>(current_time() - start).count();
}

inline std::chrono::duration<long>::rep elapsed_microseconds(const std::chrono::time_point<std::chrono::high_resolution_clock>& start) {
    return std::chrono::duration_cast<std::chrono::microseconds>(current_time() - start).count();
}

inline std::chrono::duration<long>::rep elapsed_milliseconds(const std::chrono::time_point<std::chrono::high_resolution_clock>& start) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(current_time() - start).count();
}

inline std::chrono::duration<long>::rep elapsed_seconds(const std::chrono::time_point<std::chrono::high_resolution_clock>& start) {
    return std::chrono::duration_cast<std::chrono::seconds>(current_time() - start).count();
}

class RandomNumberGenerator {
    static constexpr uint32_t default_seed = 42;
    std::mt19937_64 engine_;

public:
    explicit RandomNumberGenerator() : engine_(default_seed) {}

    explicit RandomNumberGenerator(const std::mt19937_64::result_type seed) : engine_(seed) {}

    void seed(const std::mt19937_64::result_type seed) {
        engine_.seed(seed);
    }

    double uniform_float(const double min, const double max) {
        std::uniform_real_distribution<double> distribution(min, max);
        return distribution(engine_);
    }

    double uniform_float(const double max) {
        return uniform_float(0, max);
    }

    double uniform_float() {
        return uniform_float(0, 1);
    }

    int uniform_int(const int min, const int max) {
        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(engine_);
    }

    int uniform_int(const int max) {
        return uniform_int(0, max);
    }
};
