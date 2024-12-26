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
};

class Plot {
public:
    cv::Mat image;

private:
    Eigen::Vector2f origin;
    float scale;

public:
    Plot(int size, float range) : Plot(size, size, range) {}

    Plot(int width, int height, float range) {
        this->image = cv::Mat::zeros(height, width, CV_8UC3);
        this->origin = Eigen::Vector2f(width / 2, height / 2);
        this->scale = std::min(this->origin.x(), this->origin.y()) / range;
    }

    void point(const Eigen::Vector2f &v, const cv::Scalar &color = Color::WHITE, int size = 3) {
        Eigen::Vector2f eigen_point = origin + scale * v;
        cv::Point cv_point(static_cast<int>(eigen_point.x()), static_cast<int>(eigen_point.y()));
        cv::circle(image, cv_point, size, color, 0, cv::FILLED);
    }

    void line(const Eigen::Vector2f &v1, const Eigen::Vector2f &v2, const cv::Scalar &color = Color::WHITE, int size = 1) {
        Eigen::Vector2f eigen_point1 = origin + scale * v1;
        Eigen::Vector2f eigen_point2 = origin + scale * v2;
        cv::Point cv_point1(static_cast<int>(eigen_point1.x()), static_cast<int>(eigen_point1.y()));
        cv::Point cv_point2(static_cast<int>(eigen_point2.x()), static_cast<int>(eigen_point2.y()));
        cv::line(image, cv_point1, cv_point2, color, size, cv::LINE_8);
    }

    void circle(const Eigen::Vector2f &v, float radius, const cv::Scalar &color = Color::WHITE, int size = 1) {
        Eigen::Vector2f eigen_point = origin + scale * v;
        cv::Point cv_point(static_cast<int>(eigen_point.x()), static_cast<int>(eigen_point.y()));
        cv::circle(image, cv_point, static_cast<int>(scale * radius), color, size, cv::LINE_8);
    }

    void show() const {
        cv::imshow("Plot", image);
        cv::waitKey(0);
    }

    void save(const std::string &filename) const {
        cv::imwrite(filename, image);
    }
};
