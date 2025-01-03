#pragma once

#include "base.hpp"
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

inline bool is_close(const double value, const double target_value, const double absolute_tolerance = 1e-4, const double relative_tolerance = 1e-4) {
    return std::abs(value - target_value) < absolute_tolerance + relative_tolerance * std::abs(target_value);
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
    RandomNumberGenerator rng;
    for(int T = 0; T < 10; T++) {
        std::vector<Vector2d> points = {
                    {0.1, 0.1},
                    {0.1, -0.1},
                    {-0.1, -0.1},
                    {-0.1, -0.1}
                };
        for(int t = 0; t < 10; t++) {
            const double theta = rng.uniform(0, 2 * M_PI);
            const double r = rng.uniform(0, 1);
            points.push_back({r * std::cos(theta), r * std::sin(theta)});
        }
        std::vector<Vector2d> polygon = Vector2d::convex_hull(points);
        std::ranges::sort(polygon, [](const Vector2d &v0, const Vector2d &v1) {
            return v0.get_angle() < v1.get_angle();
        });
        std::vector<double> angles;
        for(const Vector2d &v: polygon) {
            angles.push_back(v.get_angle());
        }
        for(int t = 0; t < 10000; t++) {
            Vector3 vertex(rng.uniform(-1, 1), rng.uniform(-1, 1), rng.uniform(-1, 1));
            vertex.normalize();
            const double theta_range = rng.uniform(0.01, 0.1);
            const double theta_min = rng.uniform(0, 2 * M_PI - theta_range);
            const double phi_range = rng.uniform(0.01, 0.1);
            const double phi_min = rng.uniform(0, M_PI - phi_range);
            Interval2 interval2(
                Interval(theta_min, theta_min + theta_range),
                Interval(phi_min, phi_min + phi_range)
            );
            const bool intersects = interval2.intersects_polygon(vertex, polygon, angles);
            bool actual_intersects = false;
            const std::vector<Vector2d> boundary = interval2.boundary(vertex, 0.1, 0.1);
            for(const Vector2d &v: boundary) {
                if(v.is_inside_polygon(polygon, angles)) {
                    actual_intersects = true;
                    break;
                }
            }
            if(!intersects && actual_intersects) {
                assert(0);
            }
        }
    }
}

void test_constant_phi() {
    constexpr Vector2 a(0.7, -0.7), b(0.4, 0.3);
    RandomNumberGenerator rng;
    for(int t = 0; t < 3000; t++) {
        Vector3 v(rng.uniform(-1, 1), rng.uniform(-1, 1), rng.uniform(-1, 1));
        v.normalize();
        const double theta_range = rng.uniform(0.1, 0.4);
        const double theta_min = rng.uniform(0, 2 * M_PI - theta_range);
        const double phi = rng.uniform(0, M_PI);
        Interval theta_interval(theta_min, theta_min + theta_range);

        if(Interval2::intersects_line_fixed_phi(v, a, b, theta_interval, phi)) {
            std::vector<Vector2d> points;
            for(double theta = theta_min; theta <= theta_min + theta_range; theta += 0.001) {
                points.push_back(v.project(theta, phi));
            }
            // plot_main.polyline(points, Color::GREEN, 2);
            // plot_boxes.filled_box(Box(theta_interval, Interval(phi, phi)).normalized(), Color::GREEN);
        } else {
            std::vector<Vector2d> points;
            for(double theta = theta_min; theta <= theta_min + theta_range; theta += 0.001) {
                points.push_back(v.project(theta, phi));
            }
            // plot_main.polyline(points, Color::RED / 2);
            // plot_boxes.filled_box(Box(theta_interval, Interval(phi, phi)).normalized(), Color::RED / 2);
        }
        // plot_main.line(a, b, Color::RED);
    }
}

void test_constant_theta() {
    constexpr Vector2 a(0.7, -0.7), b(-0.4, -0.3);
    RandomNumberGenerator rng;
    for(int t = 0; t < 3000; t++) {
        Vector3 v(rng.uniform(-1, 1), rng.uniform(-1, 1), rng.uniform(-1, 1));
        v.normalize();
        const double phi_range = rng.uniform(0.1, 0.5);
        const double phi_min = rng.uniform(0, M_PI - phi_range);
        const double theta = rng.uniform(0, 2 * M_PI);
        Interval phi_interval(phi_min, phi_min + phi_range);

        if(Interval2::intersects_line_fixed_theta(v, a, b, theta, phi_interval)) {
            std::vector<Vector2d> points;
            for(double phi = phi_min; phi <= phi_min + phi_range; phi += 0.001) {
                points.push_back(v.project(theta, phi));
            }
            // plot_main.polyline(points, Color::GREEN, 2);
            // plot_boxes.filled_box(Box(Interval(theta, theta), phi_interval).normalized(), Color::GREEN);
        } else {
            std::vector<Vector2d> points;
            for(double phi = phi_min; phi <= phi_min + phi_range; phi += 0.001) {
                points.push_back(v.project(theta, phi));
            }
            // plot_main.polyline(points, Color::RED / 2);
            // plot_boxes.filled_box(Box(Interval(theta, theta), phi_interval).normalized(), Color::RED / 2);
        }
        // plot_main.line(a, b, Color::RED);
    }
}

inline bool line_intersects_line(const Vector2d &a_0, const Vector2d &b_0, const Vector2d &a_1, const Vector2d &b_1) {
    const Vector2d d_0 = b_0 - a_0;
    const Vector2d d_1 = b_1 - a_1;
    const Vector2d c = a_1 - a_0;
    const double denominator = d_0.cross(d_1);
    if(denominator == 0) {
        return false;
    }
    const double t_0 = c.cross(d_1) / denominator;
    const double t_1 = c.cross(d_0) / denominator;
    return 0 <= t_0 && t_0 <= 1 && 0 <= t_1 && t_1 <= 1;
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
