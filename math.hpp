#pragma once

#include <random>
#include <Eigen/Dense>
#include <opencv2/opencv.hpp>

inline Eigen::Vector3f angles_to_direction(Eigen::Vector3f &v) {
    float theta = v.x();
    float phi = v.y();
    float cos_theta = std::cos(theta);
    float sin_theta = std::sin(theta);
    float cos_phi = std::cos(phi);
    float sin_phi = std::sin(phi);
    Eigen::Vector3f direction(sin_phi * cos_theta, sin_phi * sin_theta, cos_phi);
    return direction;
}

inline Eigen::Vector2f direction_to_angles(const Eigen::Vector3f &v) {
    float theta = std::atan2(v.y(), v.x());
    float phi = std::acos(v.z() / v.norm());
    Eigen::Vector2f angles(theta, phi);
    return angles;
}

inline Eigen::Matrix<float, 2, 3> angles_to_axes(const Eigen::Vector2f &v) {
    float theta = v.x();
    float phi = v.y();
    float cos_theta = std::cos(theta);
    float sin_theta = std::sin(theta);
    float cos_phi = std::cos(phi);
    float sin_phi = std::sin(phi);
    Eigen::Matrix<float, 2, 3> axes;
    axes << -sin_theta, cos_theta, 0,
            cos_phi * cos_theta, cos_phi * sin_theta, -sin_phi;
    return axes;
}

inline std::vector<Eigen::Vector2f> convex_hull(const std::vector<Eigen::Vector2f> &eigen_points) {
    std::vector<cv::Point2f> cv_points;
    for(auto &eigen_point: eigen_points) {
        cv_points.emplace_back(eigen_point.x(), eigen_point.y());
    }
    std::vector<cv::Point2f> hull_cv_points;
    cv::convexHull(cv_points, hull_cv_points);
    std::vector<Eigen::Vector2f> hull_eigen_points;
    for(auto &hull_cv_point: hull_cv_points) {
        hull_eigen_points.emplace_back(hull_cv_point.x, hull_cv_point.y);
    }
    return hull_eigen_points;
}

struct SineWave {
    float amplitude;
    float phase;

    SineWave(float amplitude, float phase) : amplitude(amplitude), phase(phase) {}

    static SineWave combine(float sine_amplitude, float cosine_amplitude) {
        float amplitude = std::sqrt(sine_amplitude * sine_amplitude + cosine_amplitude * cosine_amplitude);
        float phase = std::atan2(cosine_amplitude, sine_amplitude);
        return {amplitude, phase};
    }

    float operator()(float angle) const {
        return amplitude * std::sin(angle + phase);
    }
};

struct Interval {
    float min;
    float max;
};

inline std::vector<Eigen::Vector2f> boundary(Eigen::Vector3f vertex, Interval theta_interval, Interval phi_interval, float theta_step, float phi_step) {
    float x = vertex.x();
    float y = vertex.y();
    float z = vertex.z();
    float theta_min = theta_interval.min;
    float theta_max = theta_interval.max;
    float phi_min = phi_interval.min;
    float phi_max = phi_interval.max;
    std::vector<Eigen::Vector2f> points;
    // theta = theta_min
    float x_theta_min = -x * std::sin(theta_min) + y * std::cos(theta_min);
    SineWave y_theta_min = SineWave::combine(-z, x * std::cos(theta_min) + y * std::sin(theta_min));
    for(float phi = phi_min; phi < phi_max; phi += phi_step) {
        points.emplace_back(x_theta_min, y_theta_min(phi));
    }
    // phi = phi_max
    SineWave x_phi_max = SineWave::combine(-x, y);
    SineWave y_phi_max = SineWave::combine(y * std::cos(phi_max), x * std::cos(phi_max));
    float y_phi_max_constant = -z * std::sin(phi_max);
    for(float theta = theta_min; theta < theta_max; theta += theta_step) {
        points.emplace_back(x_phi_max(theta), y_phi_max(theta) + y_phi_max_constant);
    }
    // theta = theta_max
    float x_theta_max = -x * std::sin(theta_max) + y * std::cos(theta_max);
    SineWave y_theta_max = SineWave::combine(-z, x * std::cos(theta_max) + y * std::sin(theta_max));
    for(float phi = phi_max; phi > phi_min; phi -= phi_step) {
        points.emplace_back(x_theta_max, y_theta_max(phi));
    }
    // phi = phi_min
    SineWave x_phi_min = SineWave::combine(-x, y);
    SineWave y_phi_min = SineWave::combine(y * std::cos(phi_min), x * std::cos(phi_min));
    float y_phi_min_constant = -z * std::sin(phi_min);
    for(float theta = theta_max; theta > theta_min; theta -= theta_step) {
        points.emplace_back(x_phi_min(theta), y_phi_min(theta) + y_phi_min_constant);
    }
    return points;
}
