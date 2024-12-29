#pragma once

#include "helper.hpp"
#include <random>


inline Eigen::Vector3f angles_to_direction(const Eigen::Vector3f &v) {
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

inline Eigen::Vector2f project_point(const Eigen::Vector3f &point, float theta, float phi) {
    float x = point.x();
    float y = point.y();
    float z = point.z();
    float cos_theta = std::cos(theta);
    float sin_theta = std::sin(theta);
    float cos_phi = std::cos(phi);
    float sin_phi = std::sin(phi);
    float x_projected = -x * sin_theta + y * cos_theta;
    float y_projected = x * cos_phi * cos_theta + y * cos_phi * sin_theta - z * sin_phi;
    Eigen::Vector2f projected_point(x_projected, y_projected);
    return projected_point;
}

inline Eigen::Vector2f rotate_point(const Eigen::Vector2f &point, float angle) {
    float x = point.x();
    float y = point.y();
    float cos_angle = std::cos(angle);
    float sin_angle = std::sin(angle);
    float x_rotated = x * cos_angle - y * sin_angle;
    float y_rotated = x * sin_angle + y * cos_angle;
    Eigen::Vector2f rotated_point(x_rotated, y_rotated);
    return rotated_point;
}

inline std::vector<Eigen::Vector2f> convex_hull(const std::vector<Eigen::Vector2f> &eigen_points) {
    std::vector<cv::Point2f> cv_points;
    for(const Eigen::Vector2f &eigen_point: eigen_points) {
        cv_points.emplace_back(eigen_point.x(), eigen_point.y());
    }
    std::vector<cv::Point2f> hull_cv_points;
    cv::convexHull(cv_points, hull_cv_points);
    std::vector<Eigen::Vector2f> hull_eigen_points;
    for(const cv::Point2f &hull_cv_point: hull_cv_points) {
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

inline std::vector<Eigen::Vector2f> boundary(Eigen::Vector3f vertex, Box box, float theta_step, float phi_step) {
    float x = vertex.x();
    float y = vertex.y();
    float z = vertex.z();
    float theta_min = box.theta_interval.min;
    float theta_max = box.theta_interval.max;
    float phi_min = box.phi_interval.min;
    float phi_max = box.phi_interval.max;
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

inline float get_angle(const Eigen::Vector2f &v) {
    const float angle = std::atan2(v.y(), v.x());
    return angle < 0 ? angle + 2 * M_PIf : angle;
}

std::vector<Eigen::Vector2f> sort_by_angle(const std::vector<Eigen::Vector2f> &vectors) {
    std::vector<Eigen::Vector2f> sorted_vectors = vectors;
    std::sort(sorted_vectors.begin(), sorted_vectors.end(), [](const Eigen::Vector2f &v0, const Eigen::Vector2f &v1) {
        return get_angle(v0) < get_angle(v1);
    });
    return sorted_vectors;
}


inline int get_index(const std::vector<float> &angles, float angle) {
    if(angle < angles[0] || angle > angles.back()) {
        return angles.size() - 1;
    }
    int index = 0;
    while(angle > angles[index + 1]) {
        index++;
    }
    return index;
}

inline bool is_inside(const Eigen::Vector2f &v, const Eigen::Vector2f &a, const Eigen::Vector2f &b) {
    Eigen::Vector2f d = b - a;
    return d.x() * a.y() - d.y() * a.x() <= d.x() * v.y() - d.y() * v.x();
}
