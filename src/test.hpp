#pragma once

#include "util.hpp"
#include "interval.hpp"
#include "vector.hpp"
#include "cover.hpp"
#include "polyhedron.hpp"
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

inline void test_intersection() {
    RandomNumberGenerator rng;
    for(int t = 0; t < 100000; t++) {
        std::vector<std::pair<double, double>> hull;
        while(hull.size() != 4) {
            std::vector<std::pair<double, double>> vertices;
            for(int i = 0; i < 4; i++) {
                vertices.emplace_back(rng.uniform(-1, 1), rng.uniform(-1, 1));
            }
            hull = convex_hull(vertices);
            hull.pop_back();
        }
        auto &[ax, ay] = hull[0];
        auto &[bx, by] = hull[1];
        auto &[cx, cy] = hull[2];
        auto &[dx, dy] = hull[3];

        auto a = Vector2<PointInterval>(ax, ay);
        auto b = Vector2<PointInterval>(bx, by);
        auto c = Vector2<PointInterval>(cx, cy);
        auto d = Vector2<PointInterval>(dx, dy);

        assert(Line2(a, c).intersects(Line2(b, d)));
        assert(!Line2(a, b).intersects(Line2(c, d)));
        assert(!Line2(a, d).intersects(Line2(b, c)));

        assert(!Line2(a, c).avoids(Line2(b, d)));
        assert(Line2(a, b).avoids(Line2(c, d)));
        assert(Line2(a, d).avoids(Line2(b, c)));
    }
}

inline void tests() {
    auto start = current_time();
    static_assert(IntervalType<PointInterval>);
    static_assert(IntervalType<BoostInterval>);
    static_assert(Vector2Type<Vector2<BoostInterval>, BoostInterval>);
    static_assert(Vector3Type<Vector3<BoostInterval>, BoostInterval>);
    test_intersection();

    print("All tests passed in", elapsed_milliseconds(start), "ms");
}
