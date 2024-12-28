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
    cv::Mat image;
    Eigen::Vector2f origin;
    float scale;

public:
    Plot(int size, float range) : Plot(size, size, range) {}

    Plot(int width, int height, float range) {
        this->image = cv::Mat::zeros(height, width, CV_8UC3);
        this->origin = Eigen::Vector2f(width / 2, height / 2);
        this->scale = std::min(this->origin.x(), this->origin.y()) / range;
    }

    cv::Point eigen_to_cv(const Eigen::Vector2f &v) {
        return {
                    static_cast<int>(origin.x() + scale * v.x()),
                    static_cast<int>(origin.y() - scale * v.y())
                };
    }

    void point(const Eigen::Vector2f &eigen_point, const cv::Scalar &color = Color::WHITE, int radius = 0) {
        cv::Point cv_point = eigen_to_cv(eigen_point);
        cv::circle(image, cv_point, radius, color, -1, cv::LINE_4);
    }

    void points(const std::vector<Eigen::Vector2f> &eigen_points, const cv::Scalar &color = Color::WHITE, int radius = 0) {
        for(const Eigen::Vector2f &eigen_point: eigen_points) {
            this->point(eigen_point, color, radius);
        }
    }

    void line(const Eigen::Vector2f &eigen_point_0, const Eigen::Vector2f &eigen_point_1, const cv::Scalar &color = Color::WHITE, int thickness = 1) {
        cv::Point cv_point_0 = eigen_to_cv(eigen_point_0);
        cv::Point cv_point_1 = eigen_to_cv(eigen_point_1);
        cv::line(image, cv_point_0, cv_point_1, color, thickness, cv::LINE_4);
    }

    void line2(const Eigen::Vector2f &eigen_point_0, const Eigen::Vector2f &eigen_point_1, const cv::Scalar &color = Color::WHITE, int thickness = 1) {
        cv::Point cv_point_0 = eigen_to_cv(eigen_point_0);
        cv::Point cv_point_1 = eigen_to_cv(eigen_point_1);
        cv::line(image, cv_point_0, cv_point_1, color, thickness, cv::LINE_8);
    }

    void circle(const Eigen::Vector2f &eigen_point, float radius, const cv::Scalar &color = Color::WHITE, int thickness = 1) {
        cv::Point cv_point = eigen_to_cv(eigen_point);
        cv::circle(image, cv_point, static_cast<int>(scale * radius), color, thickness, cv::LINE_4);
    }

    void polygon(const std::vector<Eigen::Vector2f> &eigen_points, const cv::Scalar &color = Color::WHITE, int thickness = 1) {
        for(size_t from = 0; from < eigen_points.size(); from++) {
            size_t to = (from + 1) % eigen_points.size();
            this->line(eigen_points[from], eigen_points[to], color, thickness);
        }
    }

    void filled_polygon(const std::vector<Eigen::Vector2f> &eigen_points, const cv::Scalar &color = Color::WHITE) {
        std::vector<cv::Point> cv_points;
        for(const Eigen::Vector2f &eigen_point: eigen_points) {
            cv_points.push_back(eigen_to_cv(eigen_point));
        }
        cv::fillConvexPoly(image, cv_points.data(), static_cast<int>(cv_points.size()), color, cv::LINE_4);
    }

    void show() const {
        cv::imshow("Plot", image);
        cv::waitKey(0);
    }

    void save(const std::string &filename) const {
        cv::imwrite(filename, image);
    }
};
