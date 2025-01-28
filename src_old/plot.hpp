/*
#pragma once

#include "vector.hpp"
#include "interval.hpp"
#include <opencv2/opencv.hpp>

using Color = cv::Scalar;

namespace Colors {
    inline static const Color BLACK{0, 0, 0};
    inline static const Color WHITE{255, 255, 255};
    inline static const Color RED{0, 0, 255};
    inline static const Color GREEN{0, 255, 0};
    inline static const Color BLUE{255, 0, 0};
    inline static const Color YELLOW{0, 255, 255};
    inline static const Color CYAN{255, 255, 0};
    inline static const Color MAGENTA{255, 0, 255};
    inline static const Color GRAY{127, 127, 127};
}

inline Color gradient(const double t, const Color &color_0, const Color &color_1) {
    return color_0 * (1 - t) + color_1 * t;
}

inline Color gradient(const double t) {
    return gradient(t, Colors::GREEN, Colors::RED);
}

inline Color gradient(const double t0, const double t1, const Color &color_00, const Color &color_01, const Color &color_10, const Color &color_11) {
    return gradient(t1, gradient(t0, color_00, color_01), gradient(t0, color_10, color_11));
}

inline Color gradient(const double t0, const double t1) {
    return gradient(t0, t1, Colors::BLUE, Colors::YELLOW, Colors::RED, Colors::GREEN);
}

inline Color gradient(const Vector2d &v) {
    return gradient(v.x, v.y);
}

inline Color gradient(const Box2 &box) {
    return gradient(box.normalized().center());
}

struct Plot {
    const std::string name;
    cv::Mat pixels;

    Vector2d scale{};
    Vector2d translate{};

    Vector2i mouse_pixels{};
    Vector2d mouse{};

public:
    explicit Plot(const std::string &name, const int width, const int height, const BoostInterval &x_interval, const BoostInterval &y_interval) : name(name) {
        pixels = cv::Mat::zeros(height, width, CV_8UC3);

        scale = {
                    static_cast<double>(width) / boost::numeric::width(x_interval),
                    -static_cast<double>(height) / boost::numeric::width(y_interval)
                };
        translate = {
                    -lower(x_interval) * scale.x,
                    height - lower(y_interval) * scale.y
                };

        mouse_pixels = {0, 0};
        mouse = {0, 0};

        cv::namedWindow(name, cv::WINDOW_AUTOSIZE | cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_NORMAL);
        cv::moveWindow(name, 100, 100);

        cv::setMouseCallback(name, [](const int event, const int x, const int y, const int, void *data) {
            auto *plot = static_cast<Plot*>(data);
            plot->mouse_pixels = {x, y};
            plot->mouse = (Vector2d(x, y) - plot->translate) / plot->scale;
        }, this);
    }

    void move(const Vector2i position) const {
        cv::moveWindow(name, position.x, position.y);
    }

    void show() const {
        cv::imshow(name, pixels);
    }

    void save(const std::string &filename) const {
        cv::imwrite(filename, pixels);
    }

    static bool step(const int delay = 50) {
        const int key = cv::waitKey(delay);
        return key != 27 && key != 32 && key != 120;
    }

    cv::Point cv_point(const Vector2d &point) const {
        const auto [x, y] = point * scale + translate;
        return {
                    static_cast<int>(x),
                    static_cast<int>(y)
                };
    }

    void clear() {
        pixels.setTo(Colors::BLACK);
    }

    void point(const Vector2d &point, const Color &color = Colors::WHITE, const int radius = 0) {
        cv::circle(pixels, cv_point(point), radius, color, -1, cv::LINE_4);
    }

    void points(const std::vector<Vector2d> &points, const Color &color = Colors::WHITE, const int radius = 0) {
        for(const Vector2d &point: points) {
            this->point(point, color, radius);
        }
    }

    void circle(const Vector2d &center, const double radius, const Color &color = Colors::WHITE, const int thickness = 1) {
        const cv::Size axes{static_cast<int>(radius * abs(scale.x)), static_cast<int>(radius * abs(scale.y))};
        cv::ellipse(pixels, cv_point(center), axes, 0, 0, 360, color, thickness, cv::LINE_4);
    }

    void line(const Vector2d &point_0, const Vector2d &point_1, const Color &color = Colors::WHITE, const int thickness = 1) {
        cv::line(pixels, cv_point(point_0), cv_point(point_1), color, thickness, cv::LINE_4);
    }

    void polyline(const std::vector<Vector2d> &points, const Color &color = Colors::WHITE, const int thickness = 1) {
        std::vector<cv::Point> cv_points;
        for(const Vector2d &point: points) {
            cv_points.push_back(cv_point(point));
        }
        cv::polylines(pixels, cv_points, false, color, thickness, cv::LINE_4);
    }

    void polygon(const std::vector<Vector2d> &points, const Color &color = Colors::WHITE, const int thickness = 1) {
        std::vector<cv::Point> cv_points;
        for(const Vector2d &point: points) {
            cv_points.push_back(cv_point(point));
        }
        std::vector<std::vector<cv::Point>> cv_contours{cv_points};
        cv::polylines(pixels, cv_contours, true, color, thickness, cv::LINE_4);
    }

    void filled_polygon(const std::vector<Vector2d> &vs, const Color &color = Colors::WHITE) {
        std::vector<cv::Point> cv_points;
        for(const Vector2d &v: vs) {
            cv_points.push_back(cv_point(v));
        }
        cv::fillConvexPoly(pixels, cv_points.data(), static_cast<int>(cv_points.size()), color, cv::LINE_4);
    }

    void box(const Box2 &box, const Color &color = Colors::WHITE, const int thickness = 1) {
        polygon(box.vertices(), color, thickness);
    }

    void filled_box(const Box2 &box, const Color &color = Colors::WHITE) {
        filled_polygon(box.vertices(), color);
    }
};

*/