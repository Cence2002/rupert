#pragma once

#include <Eigen/Dense>


inline Eigen::Vector3f angles_to_direction(const Eigen::Vector3f &v) {
    const float theta = v.x();
    const float phi = v.y();
    const float cos_theta = std::cos(theta);
    const float sin_theta = std::sin(theta);
    const float cos_phi = std::cos(phi);
    const float sin_phi = std::sin(phi);
    Eigen::Vector3f direction(sin_phi * cos_theta, sin_phi * sin_theta, cos_phi);
    return direction;
}

inline Eigen::Vector2f direction_to_angles(const Eigen::Vector3f &v) {
    const float theta = std::atan2(v.y(), v.x());
    const float phi = std::acos(v.z() / v.norm());
    Eigen::Vector2f angles(theta, phi);
    return angles;
}

inline Eigen::Matrix<float, 2, 3> angles_to_axes(const Eigen::Vector2f &v) {
    const float theta = v.x();
    const float phi = v.y();
    const float cos_theta = std::cos(theta);
    const float sin_theta = std::sin(theta);
    const float cos_phi = std::cos(phi);
    const float sin_phi = std::sin(phi);
    Eigen::Matrix<float, 2, 3> axes;
    axes << -sin_theta, cos_theta, 0,
            cos_phi * cos_theta, cos_phi * sin_theta, -sin_phi;
    return axes;
}
