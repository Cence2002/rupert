#pragma once

#include "vector.hpp"
#include <opencv2/opencv.hpp>

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

    inline static const cv::Scalar top_right = GREEN;
    inline static const cv::Scalar top_left = RED;
    inline static const cv::Scalar bottom_right = YELLOW;
    inline static const cv::Scalar bottom_left = BLUE;

    static cv::Scalar rgb(const int red, const int green, const int blue) {
        return {static_cast<double>(blue), static_cast<double>(green), static_cast<double>(red)};
    }

    static cv::Scalar gradient(const double t, const cv::Scalar &color_0, const cv::Scalar &color_1) {
        return color_0 * (1 - t) + color_1 * t;
    }

    static cv::Scalar gradient(const double t) {
        return gradient(t, GREEN, RED);
    }

    static cv::Scalar gradient2(const double t0, const double t1, const cv::Scalar &color_00, const cv::Scalar &color_01, const cv::Scalar &color_10, const cv::Scalar &color_11) {
        return gradient(t1, gradient(t0, color_00, color_01), gradient(t0, color_10, color_11));
    }

    static cv::Scalar gradient2(const double t0, const double t1) {
        return gradient2(t0, t1, RED, GREEN, BLUE, YELLOW);
    }

    static cv::Scalar gradient2(const Vector2d &v) {
        return gradient2(v.x, v.y);
    }
};

class Plot {
    const std::string name;
    cv::Mat pixels;
    double origin, scale;
    Vector2d mouse{0, 0};

public:
    Plot(const std::string &name, const int size, const double range) : name(name) {
        cv::namedWindow(name, cv::WINDOW_AUTOSIZE | cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_NORMAL);
        pixels = cv::Mat::zeros(size, size, CV_8UC3);
        origin = static_cast<double>(size) / 2;
        scale = origin / range;
        cv::setMouseCallback(name, [](int event, int x, int y, int, void *data) {
            auto *plot = static_cast<Plot*>(data);
            plot->mouse = {
                        (x - plot->origin) / plot->scale,
                        (plot->origin - y) / plot->scale
                    };
        }, this);
    }

    Plot(const std::string &name, const int size) : Plot(name, size, 1) {}

    void show() const {
        cv::imshow(name, pixels);
    }

    void save(const std::string &filename) const {
        cv::imwrite(filename, pixels);
    }

    static bool step(int delay = 50) {
        return cv::waitKey(delay) != 27;
    }

    cv::Point cv_point(const Vector2d &v) const {
        return {
                    static_cast<int>(origin + scale * v.x),
                    static_cast<int>(origin - scale * v.y)
                };
    }

    Vector2d get_mouse() const {
        return mouse;
    }

    void clear() {
        pixels.setTo(Color::BLACK);
    }

    void point(const Vector2d &v, const cv::Scalar &color = Color::WHITE, const int radius = 0) {
        cv::circle(pixels, cv_point(v), radius, color, -1, cv::LINE_4);
    }

    void points(const std::vector<Vector2d> &vs, const cv::Scalar &color = Color::WHITE, const int radius = 0) {
        for(const Vector2d &v: vs) {
            point(v, color, radius);
        }
    }

    void circle(const Vector2d &v, const double radius, const cv::Scalar &color = Color::WHITE, const int thickness = 1) {
        cv::circle(pixels, cv_point(v), static_cast<int>(scale * radius), color, thickness, cv::LINE_4);
    }

    void line(const Vector2d &v0, const Vector2d &v1, const cv::Scalar &color = Color::WHITE, const int thickness = 1) {
        cv::line(pixels, cv_point(v0), cv_point(v1), color, thickness, cv::LINE_4);
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
        cv::fillConvexPoly(pixels, cv_points.data(), static_cast<int>(cv_points.size()), color, cv::LINE_4);
    }

    void interval2(const Interval2 &interval2, const cv::Scalar &color = Color::WHITE, const int thickness = 1) {
        line({lower(interval2.interval_0), lower(interval2.interval_1)}, {lower(interval2.interval_0), upper(interval2.interval_1)}, color, thickness);
        line({lower(interval2.interval_0), upper(interval2.interval_1)}, {upper(interval2.interval_0), upper(interval2.interval_1)}, color, thickness);
        line({upper(interval2.interval_0), upper(interval2.interval_1)}, {upper(interval2.interval_0), lower(interval2.interval_1)}, color, thickness);
        line({upper(interval2.interval_0), lower(interval2.interval_1)}, {lower(interval2.interval_0), lower(interval2.interval_1)}, color, thickness);
    }

    void filled_interval2(const Interval2 &interval2, const cv::Scalar &color = Color::WHITE) {
        const std::vector<Vector2d> vertices = {
                    {lower(interval2.interval_0), lower(interval2.interval_1)},
                    {lower(interval2.interval_0), upper(interval2.interval_1)},
                    {upper(interval2.interval_0), upper(interval2.interval_1)},
                    {upper(interval2.interval_0), lower(interval2.interval_1)}
                };
        filled_polygon(vertices, color);
    }
};
