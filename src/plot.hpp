#pragma once

#include "base.hpp"

class Plot {
    cv::Mat image;
    Vector2 origin{};
    double scale;

public:
    Plot(int size, double range) : Plot(size, size, range) {}

    Plot(int width, int height, double range) {
        image = cv::Mat::zeros(height, width, CV_8UC3);
        origin = Vector2(static_cast<double>(width) / 2, static_cast<double>(height) / 2);
        scale = std::min(origin.x, origin.y) / range;
    }

    cv::Point cv_point(const Vector2 &v) const {
        return {
                    static_cast<int>(origin.x + scale * v.x),
                    static_cast<int>(origin.y - scale * v.y)
                };
    }

    void point(const Vector2 &v, const cv::Scalar &color = Color::WHITE, const int radius = 0) {
        cv::circle(image, cv_point(v), radius, color, -1, cv::LINE_AA);
    }

    void points(const std::vector<Vector2> &vs, const cv::Scalar &color = Color::WHITE, const int radius = 0) {
        for(const Vector2 &v: vs) {
            point(v, color, radius);
        }
    }

    void circle(const Vector2 &v, const double radius, const cv::Scalar &color = Color::WHITE, const int thickness = 1) {
        cv::circle(image, cv_point(v), static_cast<int>(scale * radius), color, thickness, cv::LINE_AA);
    }

    void line(const Vector2 &v0, const Vector2 &v1, const cv::Scalar &color = Color::WHITE, const int thickness = 1) {
        cv::line(image, cv_point(v0), cv_point(v1), color, thickness, cv::LINE_AA);
    }

    void polyline(const std::vector<Vector2> &vs, const cv::Scalar &color = Color::WHITE, const int thickness = 1) {
        for(size_t from = 0, to = 1; to < vs.size(); from++, to++) {
            line(vs[from], vs[to], color, thickness);
        }
    }

    void polygon(const std::vector<Vector2> &vs, const cv::Scalar &color = Color::WHITE, int thickness = 1) {
        polyline(vs, color, thickness);
        line(vs.back(), vs.front(), color, thickness);
    }

    void filled_polygon(const std::vector<Vector2> &vs, const cv::Scalar &color = Color::WHITE) {
        std::vector<cv::Point> cv_points;
        for(const Vector2 &v: vs) {
            cv_points.push_back(cv_point(v));
        }
        cv::fillConvexPoly(image, cv_points.data(), static_cast<int>(cv_points.size()), color, cv::LINE_AA);
    }

    void box(const Box &box, const cv::Scalar &color = Color::WHITE, int thickness = 1) {
        Vector2 bottom_left = {box.theta_interval.min, box.phi_interval.min};
        Vector2 top_left = {box.theta_interval.min, box.phi_interval.max};
        Vector2 top_right = {box.theta_interval.max, box.phi_interval.max};
        Vector2 bottom_right = {box.theta_interval.max, box.phi_interval.min};
        line(bottom_left, top_left, color, thickness);
        line(top_left, top_right, color, thickness);
        line(top_right, bottom_right, color, thickness);
        line(bottom_right, bottom_left, color, thickness);
    }

    void filled_box(const Box &box, const cv::Scalar &color = Color::WHITE) {
        std::vector<Vector2> vertices = {
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
