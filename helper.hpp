#pragma once

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>

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
    inline static const cv::Scalar top_right = Color::GREEN;
    inline static const cv::Scalar top_left = Color::RED;
    inline static const cv::Scalar bottom_right = Color::YELLOW;
    inline static const cv::Scalar bottom_left = Color::BLUE;

    static cv::Scalar rgb(int r, int g, int b) {
        return cv::Scalar(b, g, r);
    }

    static cv::Scalar gradient(float t, const cv::Scalar &color_0, const cv::Scalar &color_1) {
        return color_0 * (1 - t) + color_1 * t;
    }
};

struct Vector2f {
    float x;
    float y;

    Vector2f operator+(const Vector2f &v) const {
        return {x + v.x, y + v.y};
    }

    Vector2f operator-(const Vector2f &v) const {
        return {x - v.x, y - v.y};
    }

    Vector2f operator*(const float s) const {
        return {x * s, y * s};
    }

    Vector2f operator/(const float s) const {
        return {x / s, y / s};
    }

    Vector2f &operator+=(const Vector2f &v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    Vector2f &operator-=(const Vector2f &v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    Vector2f &operator*=(const float s) {
        x *= s;
        y *= s;
        return *this;
    }

    Vector2f &operator/=(const float s) {
        x /= s;
        y /= s;
        return *this;
    }

    Vector2f normalized() const {
        return *this / std::sqrt(x * x + y * y);
    }

    void normalize() {
        *this /= std::sqrt(x * x + y * y);
    }

    float dot(const Vector2f &v) const {
        return x * v.x + y * v.y;
    }

    float cross(const Vector2f &v) const {
        return x * v.y - y * v.x;
    }

    static std::vector<Vector2f> hull(const std::vector<Vector2f> &points) {
        std::vector<cv::Point2f> cv_points;
        for(const Vector2f &point: points) {
            cv_points.emplace_back(point.x, point.y);
        }
        std::vector<cv::Point2f> cv_hull_points;
        cv::convexHull(cv_points, cv_hull_points);
        std::vector<Vector2f> hull_points;
        for(const cv::Point2f &cv_hull_point: cv_hull_points) {
            hull_points.emplace_back(cv_hull_point.x, cv_hull_point.y);
        }
        return hull_points;
    }

    float get_angle() const {
        if(x == 0 && y == 0) [[unlikely]] {
            return 0;
        }
        const float angle = std::atan2(y, x);
        return angle >= 0 ? angle : angle + 2 * M_PIf;
    }

    static std::vector<Vector2f> sort(const std::vector<Vector2f> &vectors) {
        std::vector<Vector2f> sorted_vectors = vectors;
        std::sort(sorted_vectors.begin(), sorted_vectors.end(), [](const Vector2f &v0, const Vector2f &v1) {
            return v0.get_angle() < v1.get_angle();
        });
        return sorted_vectors;
    }

    Vector2f rotate(const float angle) const {
        const float cos_angle = std::cos(angle);
        const float sin_angle = std::sin(angle);
        return {
                    x * cos_angle - y * sin_angle,
                    x * sin_angle + y * cos_angle
                };
    }

    int get_index(const std::vector<float> &angles) const {
        const float angle = get_angle();
        if(angle < angles[0] || angle >= angles.back()) [[unlikely]] {
            return static_cast<int>(angles.size()) - 1;
        }
        return static_cast<int>(std::upper_bound(angles.begin(), angles.end(), angle) - angles.begin()) - 1;
    }

    bool is_inside(const Vector2f &a, const Vector2f &b) const {
        const Vector2f d = b - a;
        return d.cross(a) < d.cross(*this);
    }
};

struct Vector3f {
    float x;
    float y;
    float z;

    Vector3f operator+(const Vector3f &v) const {
        return {x + v.x, y + v.y, z + v.z};
    }

    Vector3f operator-(const Vector3f &v) const {
        return {x - v.x, y - v.y, z - v.z};
    }

    Vector3f operator*(const float s) const {
        return {x * s, y * s, z * s};
    }

    Vector3f operator/(const float s) const {
        return {x / s, y / s, z / s};
    }

    Vector3f &operator+=(const Vector3f &v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vector3f &operator-=(const Vector3f &v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    Vector3f &operator*=(const float s) {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    Vector3f &operator/=(const float s) {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }

    Vector3f normalized() const {
        return *this / std::sqrt(x * x + y * y + z * z);
    }

    void normalize() {
        *this /= std::sqrt(x * x + y * y + z * z);
    }

    float dot(const Vector3f &v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    Vector3f cross(const Vector3f &v) const {
        return {
                    y * v.z - z * v.y,
                    z * v.x - x * v.z,
                    x * v.y - y * v.x
                };
    }

    Vector2f project(const float theta, const float phi) const {
        const float sin_theta = std::sin(theta);
        const float cos_theta = std::cos(theta);
        return {
                    -x * sin_theta + y * cos_theta,
                    (x * cos_theta + y * sin_theta) * std::cos(phi) - z * std::sin(phi)
                };
    }
};

class SineWave {
    const float amplitude;
    const float phase;

public:
    SineWave(float amplitude, float phase) : amplitude(amplitude), phase(phase) {}

    static SineWave combine(float sine_amplitude, float cosine_amplitude) {
        float amplitude = std::sqrt(sine_amplitude * sine_amplitude + cosine_amplitude * cosine_amplitude);
        float phase = Vector2f(sine_amplitude, cosine_amplitude).get_angle();
        return {amplitude, phase};
    }

    float operator()(float angle) const {
        return amplitude * std::sin(angle + phase);
    }
};

struct Interval {
    const float min;
    const float max;
};

struct Box {
    const Interval theta_interval;
    const Interval phi_interval;
};
