#pragma once

#include "base.hpp"

class Plot {
    cv::Mat image;
    Vector2d origin{};
    double scale;

public:
    Plot(const int size, const double range) : Plot(size, size, range) {}

    Plot(const int width, const int height, const double range) {
        image = cv::Mat::zeros(height, width, CV_8UC3);
        origin = Vector2(static_cast<double>(width) / 2, static_cast<double>(height) / 2);
        scale = std::min(origin.x, origin.y) / range;
    }

    void clear() {
        image = cv::Mat::zeros(image.size(), image.type());
    }

    void show() const {
        cv::imshow("Plot", image);
        cv::waitKey(0);
    }

    void save(const std::string &filename) const {
        cv::imwrite(filename, image);
    }

    cv::Point cv_point(const Vector2d &v) const {
        return {
                    static_cast<int>(origin.x + scale * v.x),
                    static_cast<int>(origin.y - scale * v.y)
                };
    }

    void point(const Vector2d &v, const cv::Scalar &color = Color::WHITE, const int radius = 0) {
        cv::circle(image, cv_point(v), radius, color, -1, cv::LINE_4);
    }

    void points(const std::vector<Vector2d> &vs, const cv::Scalar &color = Color::WHITE, const int radius = 0) {
        for(const Vector2d &v: vs) {
            point(v, color, radius);
        }
    }

    void circle(const Vector2d &v, const double radius, const cv::Scalar &color = Color::WHITE, const int thickness = 1) {
        cv::circle(image, cv_point(v), static_cast<int>(scale * radius), color, thickness, cv::LINE_4);
    }

    void line(const Vector2d &v0, const Vector2d &v1, const cv::Scalar &color = Color::WHITE, const int thickness = 1) {
        cv::line(image, cv_point(v0), cv_point(v1), color, thickness, cv::LINE_4);
    }

    void polyline(const std::vector<Vector2d> &vs, const cv::Scalar &color = Color::WHITE, const int thickness = 1) {
        for(size_t from = 0, to = 1; to < vs.size(); from++, to++) {
            line(vs[from], vs[to], color, thickness);
        }
    }

    void polygon(const std::vector<Vector2d> &vs, const cv::Scalar &color = Color::WHITE, const int thickness = 1) {
        polyline(vs, color, thickness);
        line(vs.back(), vs.front(), color, thickness);
    }

    void filled_polygon(const std::vector<Vector2d> &vs, const cv::Scalar &color = Color::WHITE) {
        std::vector<cv::Point> cv_points;
        for(const Vector2d &v: vs) {
            cv_points.push_back(cv_point(v));
        }
        cv::fillConvexPoly(image, cv_points.data(), static_cast<int>(cv_points.size()), color, cv::LINE_4);
    }

    void box(const Interval2 &interval2, const cv::Scalar &color = Color::WHITE, const int thickness = 1) {
        line({interval2.theta.lower(), interval2.phi.lower()}, {interval2.theta.lower(), interval2.phi.upper()}, color, thickness);
        line({interval2.theta.lower(), interval2.phi.upper()}, {interval2.theta.upper(), interval2.phi.upper()}, color, thickness);
        line({interval2.theta.upper(), interval2.phi.upper()}, {interval2.theta.upper(), interval2.phi.lower()}, color, thickness);
        line({interval2.theta.upper(), interval2.phi.lower()}, {interval2.theta.lower(), interval2.phi.lower()}, color, thickness);
    }

    void filled_box(const Interval2 &box, const cv::Scalar &color = Color::WHITE) {
        const std::vector<Vector2d> vertices = {
                    {box.theta.lower(), box.phi.lower()},
                    {box.theta.lower(), box.phi.upper()},
                    {box.theta.upper(), box.phi.upper()},
                    {box.theta.upper(), box.phi.lower()}
                };
        filled_polygon(vertices, color);
    }
};
