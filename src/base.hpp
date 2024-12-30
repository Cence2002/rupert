#pragma once

#include <opencv2/opencv.hpp>

struct Color {
    inline static const cv::Scalar BLACK = cv::Scalar(0, 0, 0);
    inline static const cv::Scalar WHITE = cv::Scalar(255, 255, 255);
    inline static const cv::Scalar RED = cv::Scalar(0, 0, 255);
    inline static const cv::Scalar GREEN = cv::Scalar(0, 255, 0);
    inline static const cv::Scalar BLUE = cv::Scalar(255, 0, 0);
    inline static const cv::Scalar YELLOW = cv::Scalar(0, 255, 255);
    inline static const cv::Scalar CYAN = cv::Scalar(255, 255, 0);
    inline static const cv::Scalar MAGENTA = cv::Scalar(255, 0, 255);
    inline static const cv::Scalar GRAY = cv::Scalar(127, 127, 127);

    inline static const cv::Scalar top_right = GREEN;
    inline static const cv::Scalar top_left = RED;
    inline static const cv::Scalar bottom_right = YELLOW;
    inline static const cv::Scalar bottom_left = BLUE;

    static cv::Scalar rgb(int r, int g, int b) {
        return {static_cast<double>(b), static_cast<double>(g), static_cast<double>(r)};
    }

    static cv::Scalar gradient(double t, const cv::Scalar &color_0, const cv::Scalar &color_1) {
        return color_0 * (1 - t) + color_1 * t;
    }
};

struct Vector2 {
    double x;
    double y;

    friend std::ostream &operator<<(std::ostream &os, const Vector2 &v) {
        os << "(" << v.x << ", " << v.y << ")";
        return os;
    }

    Vector2 operator+(const double s) const {
        return {x + s, y + s};
    }

    Vector2 operator+(const Vector2 &v) const {
        return {x + v.x, y + v.y};
    }

    Vector2 operator-(const double s) const {
        return {x - s, y - s};
    }

    Vector2 operator-(const Vector2 &v) const {
        return {x - v.x, y - v.y};
    }

    Vector2 operator*(const double s) const {
        return {x * s, y * s};
    }

    Vector2 operator*(const Vector2 &v) const {
        return {x * v.x, y * v.y};
    }

    Vector2 operator/(const double s) const {
        return {x / s, y / s};
    }

    Vector2 operator/(const Vector2 &v) const {
        return {x / v.x, y / v.y};
    }

    double norm() const {
        return std::sqrt(x * x + y * y);
    }

    Vector2 normalized() const {
        return *this / norm();
    }

    void normalize() {
        const double s = norm();
        x /= s;
        y /= s;
    }

    double dot(const Vector2 &v) const {
        return x * v.x + y * v.y;
    }

    double cross(const Vector2 &v) const {
        return x * v.y - y * v.x;
    }

    static std::vector<Vector2> hull(const std::vector<Vector2> &points) {
        std::vector<cv::Point2f> cv_points;
        for(const Vector2 &point: points) {
            cv_points.emplace_back(point.x, point.y);
        }
        std::vector<cv::Point2f> cv_hull_points;
        cv::convexHull(cv_points, cv_hull_points);
        std::vector<Vector2> hull_points;
        for(const cv::Point2f &cv_hull_point: cv_hull_points) {
            hull_points.emplace_back(cv_hull_point.x, cv_hull_point.y);
        }
        return hull_points;
    }

    double get_angle() const {
        if(x == 0 && y == 0) [[unlikely]] {
            return 0;
        }
        const double angle = std::atan2(y, x);
        return angle >= 0 ? angle : angle + 2 * M_PIf;
    }

    static std::vector<Vector2> sort(const std::vector<Vector2> &vectors) {
        std::vector<Vector2> sorted_vectors = vectors;
        std::ranges::sort(sorted_vectors, [](const Vector2 &v0, const Vector2 &v1) {
            return v0.get_angle() < v1.get_angle();
        });
        return sorted_vectors;
    }

    Vector2 rotate(const double angle) const {
        const double cos_angle = std::cos(angle);
        const double sin_angle = std::sin(angle);
        return {
                    x * cos_angle - y * sin_angle,
                    x * sin_angle + y * cos_angle
                };
    }

    int get_index(const std::vector<double> &angles) const {
        const double angle = get_angle();
        if(angle < angles[0] || angle >= angles.back()) [[unlikely]] {
            return static_cast<int>(angles.size()) - 1;
        }
        return static_cast<int>(std::ranges::upper_bound(angles, angle) - angles.begin()) - 1;
    }

    bool is_inside(const Vector2 &a, const Vector2 &b) const {
        const Vector2 d = b - a;
        return d.cross(a) < d.cross(*this);
    }
};

struct Vector3 {
    double x;
    double y;
    double z;

    friend std::ostream &operator<<(std::ostream &os, const Vector3 &v) {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }

    Vector3 operator+(const double s) const {
        return {x + s, y + s, z + s};
    }

    Vector3 operator+(const Vector3 &v) const {
        return {x + v.x, y + v.y, z + v.z};
    }

    Vector3 operator-(const double s) const {
        return {x - s, y - s, z - s};
    }

    Vector3 operator-(const Vector3 &v) const {
        return {x - v.x, y - v.y, z - v.z};
    }

    Vector3 operator*(const double s) const {
        return {x * s, y * s, z * s};
    }

    Vector3 operator*(const Vector3 &v) const {
        return {x * v.x, y * v.y, z * v.z};
    }

    Vector3 operator/(const double s) const {
        return {x / s, y / s, z / s};
    }

    Vector3 operator/(const Vector3 &v) const {
        return {x / v.x, y / v.y, z / v.z};
    }

    double norm() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vector3 normalized() const {
        return *this / norm();
    }

    void normalize() {
        const double s = norm();
        x /= s;
        y /= s;
        z /= s;
    }

    double dot(const Vector3 &v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    Vector3 cross(const Vector3 &v) const {
        return {
                    y * v.z - z * v.y,
                    z * v.x - x * v.z,
                    x * v.y - y * v.x
                };
    }

    Vector2 project(const double theta, const double phi) const {
        const double sin_theta = std::sin(theta);
        const double cos_theta = std::cos(theta);
        return {
                    -x * sin_theta + y * cos_theta,
                    (x * cos_theta + y * sin_theta) * std::cos(phi) - z * std::sin(phi)
                };
    }
};

class SineWave {
    const double amplitude;
    const double phase;

public:
    SineWave(double amplitude, double phase) : amplitude(amplitude), phase(phase) {}

    static SineWave combine(double sine_amplitude, double cosine_amplitude) {
        double amplitude = std::sqrt(sine_amplitude * sine_amplitude + cosine_amplitude * cosine_amplitude);
        double phase = Vector2(sine_amplitude, cosine_amplitude).get_angle();
        return {amplitude, phase};
    }

    double operator()(double angle) const {
        return amplitude * std::sin(angle + phase);
    }
};

struct Interval {
    const double min;
    const double max;
};

struct Box {
    static double constexpr theta_range = 2 * M_PIf;
    static double constexpr phi_range = M_PI_2f;
    const Interval theta_interval;
    const Interval phi_interval;

    Vector2 center() const {
        return {
                    (theta_interval.min + theta_interval.max) / 2 / M_PIf - 1,
                    (phi_interval.min + phi_interval.max) / 2 / M_PI_2f - 1
                };
    }

    Box normalized() const {
        return {
                    Interval(theta_interval.min / M_PIf - 1, theta_interval.max / M_PIf - 1),
                    Interval(phi_interval.min / M_PI_2f - 1, phi_interval.max / M_PI_2f - 1)
                };
    }

    cv::Scalar color() const {
        const double x = (theta_interval.min + theta_interval.max) / 2 / (2 * M_PIf);
        const double y = (phi_interval.min + phi_interval.max) / 2 / M_PIf;
        return Color::gradient(y,
                               Color::gradient(x, Color::bottom_left, Color::bottom_right),
                               Color::gradient(x, Color::top_left, Color::top_right));
    }
};
