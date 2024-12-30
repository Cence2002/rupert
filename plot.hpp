#pragma once

#include "helper.hpp"


class Plot {
    cv::Mat image;
    Vector2f origin;
    float scale;

public:
    Plot(int size, float range) : Plot(size, size, range) {}

    Plot(int width, int height, float range) {
        image = cv::Mat::zeros(height, width, CV_8UC3);
        origin = Vector2f(static_cast<float>(width) / 2, static_cast<float>(height) / 2);
        scale = std::min(origin.x, origin.y) / range;
    }

    cv::Point eigen_to_cv(const Vector2f &v) const {
        return {
                    static_cast<int>(origin.x + scale * v.x),
                    static_cast<int>(origin.y - scale * v.y)
                };
    }

    void point(const Vector2f &eigen_point, const cv::Scalar &color = Color::WHITE, int radius = 0) {
        cv::Point cv_point = eigen_to_cv(eigen_point);
        cv::circle(image, cv_point, radius, color, -1, cv::LINE_4);
    }

    void points(const std::vector<Vector2f> &eigen_points, const cv::Scalar &color = Color::WHITE, int radius = 0) {
        for(const Vector2f &eigen_point: eigen_points) {
            point(eigen_point, color, radius);
        }
    }

    void line(const Vector2f &eigen_point_0, const Vector2f &eigen_point_1, const cv::Scalar &color = Color::WHITE, int thickness = 1) {
        cv::Point cv_point_0 = eigen_to_cv(eigen_point_0);
        cv::Point cv_point_1 = eigen_to_cv(eigen_point_1);
        cv::line(image, cv_point_0, cv_point_1, color, thickness, cv::LINE_4);
    }

    void line2(const Vector2f &eigen_point_0, const Vector2f &eigen_point_1, const cv::Scalar &color = Color::WHITE, int thickness = 1) {
        cv::Point cv_point_0 = eigen_to_cv(eigen_point_0);
        cv::Point cv_point_1 = eigen_to_cv(eigen_point_1);
        cv::line(image, cv_point_0, cv_point_1, color, thickness, cv::LINE_8);
    }

    void circle(const Vector2f &eigen_point, float radius, const cv::Scalar &color = Color::WHITE, int thickness = 1) {
        cv::Point cv_point = eigen_to_cv(eigen_point);
        cv::circle(image, cv_point, static_cast<int>(scale * radius), color, thickness, cv::LINE_4);
    }

    void polygon(const std::vector<Vector2f> &eigen_points, const cv::Scalar &color = Color::WHITE, int thickness = 1) {
        for(size_t from = 0; from < eigen_points.size(); from++) {
            size_t to = (from + 1) % eigen_points.size();
            line(eigen_points[from], eigen_points[to], color, thickness);
        }
    }

    void filled_polygon(const std::vector<Vector2f> &eigen_points, const cv::Scalar &color = Color::WHITE) {
        std::vector<cv::Point> cv_points;
        for(const Vector2f &eigen_point: eigen_points) {
            cv_points.push_back(eigen_to_cv(eigen_point));
        }
        cv::fillConvexPoly(image, cv_points.data(), static_cast<int>(cv_points.size()), color, cv::LINE_4);
    }

    void box(const Box &box, const cv::Scalar &color = Color::WHITE, int thickness = 1) {
        Vector2f bottom_left = {box.theta_interval.min, box.phi_interval.min};
        Vector2f top_left = {box.theta_interval.min, box.phi_interval.max};
        Vector2f top_right = {box.theta_interval.max, box.phi_interval.max};
        Vector2f bottom_right = {box.theta_interval.max, box.phi_interval.min};
        line(bottom_left, top_left, color, thickness);
        line(top_left, top_right, color, thickness);
        line(top_right, bottom_right, color, thickness);
        line(bottom_right, bottom_left, color, thickness);
    }

    void filled_box(const Box &box, const cv::Scalar &color = Color::WHITE) {
        std::vector<Vector2f> vertices = {
                    {box.theta_interval.min, box.phi_interval.min},
                    {box.theta_interval.min, box.phi_interval.max},
                    {box.theta_interval.max, box.phi_interval.max},
                    {box.theta_interval.max, box.phi_interval.min}
                };
        filled_polygon(vertices, color);
    }

    void show() const {
        cv::imshow("Plot", image);
        cv::waitKey(0);
    }

    void save(const std::string &filename) const {
        cv::imwrite(filename, image);
    }
};
