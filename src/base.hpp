#pragma once

#include <opencv2/opencv.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/adapted/boost_tuple.hpp>
BOOST_GEOMETRY_REGISTER_BOOST_TUPLE_CS(cs::cartesian)

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

    static cv::Scalar rgb(const int r, const int g, const int b) {
        return {static_cast<double>(b), static_cast<double>(g), static_cast<double>(r)};
    }

    static cv::Scalar gradient(const double t, const cv::Scalar &color_0, const cv::Scalar &color_1) {
        return color_0 * (1 - t) + color_1 * t;
    }
};

constexpr double mod(const double angle) {
    return std::fmod(std::fmod(angle, 2 * M_PI) + 2 * M_PI, 2 * M_PI);
}

struct Vector2 {
    double x;
    double y;

    friend std::ostream &operator<<(std::ostream &os, const Vector2 &v) {
        os << "(" << v.x << ", " << v.y << ")";
        return os;
    }

    Vector2 operator+(const double s) const {
        return {x + s, y + s};
    }

    Vector2 operator+(const Vector2 &v) const {
        return {x + v.x, y + v.y};
    }

    Vector2 operator-(const double s) const {
        return {x - s, y - s};
    }

    Vector2 operator-(const Vector2 &v) const {
        return {x - v.x, y - v.y};
    }

    Vector2 operator*(const double s) const {
        return {x * s, y * s};
    }

    Vector2 operator*(const Vector2 &v) const {
        return {x * v.x, y * v.y};
    }

    Vector2 operator/(const double s) const {
        return {x / s, y / s};
    }

    Vector2 operator/(const Vector2 &v) const {
        return {x / v.x, y / v.y};
    }

    double norm() const {
        return std::sqrt(x * x + y * y);
    }

    Vector2 normalized() const {
        return *this / norm();
    }

    void normalize() {
        const double s = norm();
        x /= s;
        y /= s;
    }

    double dot(const Vector2 &v) const {
        return x * v.x + y * v.y;
    }

    double cross(const Vector2 &v) const {
        return x * v.y - y * v.x;
    }

    static std::vector<Vector2> convex_hull(const std::vector<Vector2> &points) {
        namespace bg = boost::geometry;
        using BoostPoint = boost::tuple<double, double>;
        using BoostPolygon = bg::model::polygon<BoostPoint>;
        BoostPolygon input_polygon;
        for(const auto &[x, y]: points) {
            bg::append(input_polygon.outer(), BoostPoint(x, y));
        }
        BoostPolygon hull_polygon;
        bg::convex_hull(input_polygon, hull_polygon);
        std::vector<Vector2> hull_points;
        for(const auto &hull_point: hull_polygon.outer()) {
            hull_points.emplace_back(Vector2{bg::get<0>(hull_point), bg::get<1>(hull_point)});
        }
        return hull_points;
    }

    double get_angle() const {
        if(x == 0 && y == 0) [[unlikely]] {
            return 0;
        }
        const double angle = std::atan2(y, x);
        // return angle >= 0 ? angle : angle + 2 * M_PI;
        return mod(angle);
    }

    static std::vector<Vector2> sort(const std::vector<Vector2> &vectors) {
        std::vector<Vector2> sorted_vectors = vectors;
        std::ranges::sort(sorted_vectors, [](const Vector2 &v0, const Vector2 &v1) {
            return v0.get_angle() < v1.get_angle();
        });
        return sorted_vectors;
    }

    Vector2 rotate(const double angle) const {
        const double cos_angle = std::cos(angle);
        const double sin_angle = std::sin(angle);
        return {
                    x * cos_angle - y * sin_angle,
                    x * sin_angle + y * cos_angle
                };
    }

    int get_index(const std::vector<double> &angles) const {
        const double angle = get_angle();
        if(angle < angles[0] || angle >= angles.back()) [[unlikely]] {
            return static_cast<int>(angles.size()) - 1;
        }
        return static_cast<int>(std::ranges::upper_bound(angles, angle) - angles.begin()) - 1;
    }

    bool is_inside(const Vector2 &a, const Vector2 &b) const {
        const Vector2 d = b - a;
        return d.cross(a) < d.cross(*this);
    }

    bool is_inside_polygon(const std::vector<Vector2> &vertices) const {
        std::vector<double> angles;
        for(const Vector2 &vertex: vertices) {
            angles.push_back(vertex.get_angle());
        }
        const int index = get_index(angles);
        return is_inside(vertices[index], vertices[(index + 1) % vertices.size()]);
    }
};

struct Vector3 {
    double x;
    double y;
    double z;

    friend std::ostream &operator<<(std::ostream &os, const Vector3 &v) {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }

    Vector3 operator+(const double s) const {
        return {x + s, y + s, z + s};
    }

    Vector3 operator+(const Vector3 &v) const {
        return {x + v.x, y + v.y, z + v.z};
    }

    Vector3 operator-(const double s) const {
        return {x - s, y - s, z - s};
    }

    Vector3 operator-(const Vector3 &v) const {
        return {x - v.x, y - v.y, z - v.z};
    }

    Vector3 operator*(const double s) const {
        return {x * s, y * s, z * s};
    }

    Vector3 operator*(const Vector3 &v) const {
        return {x * v.x, y * v.y, z * v.z};
    }

    Vector3 operator/(const double s) const {
        return {x / s, y / s, z / s};
    }

    Vector3 operator/(const Vector3 &v) const {
        return {x / v.x, y / v.y, z / v.z};
    }

    double norm() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vector3 normalized() const {
        return *this / norm();
    }

    void normalize() {
        const double s = norm();
        x /= s;
        y /= s;
        z /= s;
    }

    double dot(const Vector3 &v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    Vector3 cross(const Vector3 &v) const {
        return {
                    y * v.z - z * v.y,
                    z * v.x - x * v.z,
                    x * v.y - y * v.x
                };
    }

    Vector2 project(const double theta, const double phi) const {
        const double sin_theta = std::sin(theta);
        const double cos_theta = std::cos(theta);
        return {
                    -x * sin_theta + y * cos_theta,
                    (x * cos_theta + y * sin_theta) * std::cos(phi) - z * std::sin(phi)
                };
    }
};

class SineWave {
    const double amplitude;
    const double phase;

public:
    SineWave(const double amplitude, const double phase) : amplitude(amplitude), phase(phase) {}

    static SineWave combine(const double sine_amplitude, const double cosine_amplitude) {
        double amplitude = std::sqrt(sine_amplitude * sine_amplitude + cosine_amplitude * cosine_amplitude);
        double phase = Vector2(sine_amplitude, cosine_amplitude).get_angle();
        return {amplitude, phase};
    }

    double operator()(const double angle) const {
        return amplitude * std::sin(angle + phase);
    }
};

struct Interval {
    const double min;
    const double max;

    bool contains(const double value) const {
        return min <= value && value <= max;
    }
};

struct Box {
    const Interval theta_interval;
    const Interval phi_interval;

    Vector2 center() const {
        return {
                    (theta_interval.min + theta_interval.max) / 2 / M_PI - 1,
                    (phi_interval.min + phi_interval.max) / 2 / M_PI_2 - 1
                };
    }

    Box normalized() const {
        return {
                    Interval(theta_interval.min / M_PI - 1, theta_interval.max / M_PI - 1),
                    Interval(phi_interval.min / M_PI_2 - 1, phi_interval.max / M_PI_2 - 1)
                };
    }

    cv::Scalar color() const {
        const double x = (theta_interval.min + theta_interval.max) / 2 / (2 * M_PI);
        const double y = (phi_interval.min + phi_interval.max) / 2 / M_PI;
        return Color::gradient(y,
                               Color::gradient(x, Color::bottom_left, Color::bottom_right),
                               Color::gradient(x, Color::top_left, Color::top_right));
    }

    std::vector<Vector2> boundary(const Vector3 &vertex, double theta_step, double phi_step) const {
        const double x = vertex.x;
        const double y = vertex.y;
        const double z = vertex.z;
        const double theta_min = theta_interval.min;
        const double theta_max = theta_interval.max;
        const double phi_min = phi_interval.min;
        const double phi_max = phi_interval.max;
        theta_step *= theta_max - theta_min;
        phi_step *= phi_max - phi_min;
        std::vector<Vector2> points;
        // theta = theta_min
        double x_theta_min = -x * std::sin(theta_min) + y * std::cos(theta_min);
        SineWave y_theta_min = SineWave::combine(-z, x * std::cos(theta_min) + y * std::sin(theta_min));
        for(double phi = phi_min; phi <= phi_max; phi += phi_step) {
            points.emplace_back(x_theta_min, y_theta_min(phi));
        }
        // phi = phi_max
        SineWave x_phi_max = SineWave::combine(-x, y);
        SineWave y_phi_max = SineWave::combine(y * std::cos(phi_max), x * std::cos(phi_max));
        double y_phi_max_constant = -z * std::sin(phi_max);
        for(double theta = theta_min; theta <= theta_max; theta += theta_step) {
            points.emplace_back(x_phi_max(theta), y_phi_max(theta) + y_phi_max_constant);
        }
        // theta = theta_max
        double x_theta_max = -x * std::sin(theta_max) + y * std::cos(theta_max);
        SineWave y_theta_max = SineWave::combine(-z, x * std::cos(theta_max) + y * std::sin(theta_max));
        for(double phi = phi_max; phi >= phi_min; phi -= phi_step) {
            points.emplace_back(x_theta_max, y_theta_max(phi));
        }
        // phi = phi_min
        SineWave x_phi_min = SineWave::combine(-x, y);
        SineWave y_phi_min = SineWave::combine(y * std::cos(phi_min), x * std::cos(phi_min));
        double y_phi_min_constant = -z * std::sin(phi_min);
        for(double theta = theta_max; theta >= theta_min; theta -= theta_step) {
            points.emplace_back(x_phi_min(theta), y_phi_min(theta) + y_phi_min_constant);
        }
        return points;
    }

    static bool intersects_line_fixed_phi(const Vector3 &v, const Vector2 &vertex_0, const Vector2 &vertex_1, const Interval &theta_interval, const double phi) {
        const double y_shift = v.z * std::sin(phi);
        const double y_scale = std::cos(phi);
        const Vector2 a(vertex_0.x, (vertex_0.y + y_shift) / y_scale);
        const Vector2 b(vertex_1.x, (vertex_1.y + y_shift) / y_scale);

        const Vector2 d = b - a;
        const double A_double = 2 * d.dot(d);
        const double B = 2 * d.dot(a);
        const double C = a.dot(a) - (v.x * v.x + v.y * v.y);
        const double discriminant = B * B - 2 * A_double * C;
        if(discriminant < 0) [[likely]] {
            return false;
        }

        const double sqrt_discriminant = std::sqrt(discriminant);
        const auto solutions = {
                    (-B + sqrt_discriminant) / A_double,
                    (-B - sqrt_discriminant) / A_double
                };
        return std::ranges::any_of(solutions, [&](const double solution) {
            return solution >= 0 && solution <= 1 &&
                   theta_interval.contains(mod((a + d * solution).get_angle() - Vector2(v.y, v.x).get_angle()));
        });
    }

    static bool intersects_line_fixed_theta(const Vector3 &v, const Vector2 &vertex_0, const Vector2 &vertex_1, const double theta, const Interval &phi_interval) {
        const double sin_theta = std::sin(theta);
        const double cos_theta = std::cos(theta);
        const double x = -v.x * sin_theta + v.y * cos_theta;
        if(x < std::min(vertex_0.x, vertex_1.x) || x > std::max(vertex_0.x, vertex_1.x)) [[likely]] {
            return false;
        }
        const Vector2 coefficient(v.x * cos_theta + v.y * sin_theta, -v.z);
        const double r = coefficient.norm();
        const double shift = coefficient.get_angle();
        const double y_0 = r * std::cos(phi_interval.min - shift);
        const double y_1 = r * std::cos(phi_interval.max - shift);
        double min_y = std::min(y_0, y_1);
        if(phi_interval.contains(shift + M_PI)) [[unlikely]] {
            min_y = -1;
        }
        double max_y = std::max(y_0, y_1);
        if(phi_interval.contains(shift)) [[unlikely]] {
            max_y = 1;
        }
        const auto [dx, dy] = vertex_1 - vertex_0;
        const double y = vertex_0.y + dy * (x - vertex_0.x) / dx;
        return min_y <= y && y <= max_y;
    }

    bool intersects_line(const Vector3 &v, const Vector2 &vertex_0, const Vector2 &vertex_1) const {
        return intersects_line_fixed_phi(v, vertex_0, vertex_1, theta_interval, phi_interval.min) ||
               intersects_line_fixed_phi(v, vertex_0, vertex_1, theta_interval, phi_interval.max) ||
               intersects_line_fixed_theta(v, vertex_0, vertex_1, theta_interval.min, phi_interval) ||
               intersects_line_fixed_theta(v, vertex_0, vertex_1, theta_interval.max, phi_interval);
    }

    bool intersects_polygon(const Vector3 &v, const std::vector<Vector2> &vertices) const {
        const double sample_theta = (theta_interval.min + theta_interval.max) / 2;
        const double sample_phi = (phi_interval.min + phi_interval.max) / 2;
        if(v.project(sample_theta, sample_phi).is_inside_polygon(vertices)) {
            return true;
        }
        for(size_t from = 0, to = 1; to < vertices.size(); from++, to++) {
            if(intersects_line(v, vertices[from], vertices[to])) {
                return true;
            }
        }
        return intersects_line(v, vertices.back(), vertices.front());
    }
};
