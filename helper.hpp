#pragma once

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>

struct Interval {
    float min;
    float max;
};

struct Box {
    Interval theta_interval;
    Interval phi_interval;
};

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

    inline static cv::Scalar rgb(int r, int g, int b) {
        return cv::Scalar(b, g, r);
    }

    inline static cv::Scalar gradient(float t, const cv::Scalar &color_0, const cv::Scalar &color_1) {
        return color_0 * (1 - t) + color_1 * t;
    }
};
