#pragma once

#include <opencv2/opencv.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/adapted/boost_tuple.hpp>
#include <boost/numeric/interval.hpp>
#include <boost/numeric/interval/transc.hpp>

BOOST_GEOMETRY_REGISTER_BOOST_TUPLE_CS(cs::cartesian)

using BoostInterval = boost::numeric::interval<
    double,
    boost::numeric::interval_lib::policies<
        boost::numeric::interval_lib::save_state<
            boost::numeric::interval_lib::rounded_transc_std<double>
        >,
        boost::numeric::interval_lib::checking_base<double>
    >
>;

struct Interval : BoostInterval {
    Interval() : BoostInterval() {}

    Interval(const double value) : BoostInterval(value) {}

    Interval(const double lower, const double upper) : BoostInterval(lower, upper) {}

    Interval(const BoostInterval &interval) : BoostInterval(interval) {}

    friend std::ostream &operator<<(std::ostream &os, const Interval &interval) {
        os << "[" << interval.lower() << ", " << interval.upper() << "]";
        return os;
    }

    double length() const {
        return upper() - lower();
    }

    double center() const {
        return (lower() + upper()) / 2;
    }

    bool contains(const double value) const {
        return lower() <= value && value <= upper();
    }

    std::vector<Interval> divide(const int parts) const {
        std::vector<double> points;
        points.push_back(lower());
        for(int i = 1; i < parts; i++) {
            points.push_back(lower() + i * length() / parts);
        }
        points.push_back(upper());
        std::vector<Interval> intervals;
        for(size_t i = 0; i < points.size() - 1; i++) {
            intervals.emplace_back(points[i], points[i + 1]);
        }
        return intervals;
    }
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
};

template<typename T>
constexpr T mod(const T &angle) {
    return fmod(fmod(angle, 2 * M_PI) + 2 * M_PI, 2 * M_PI);
}

template<typename T>
struct Vector2 {
    T x;
    T y;

    friend std::ostream &operator<<(std::ostream &os, const Vector2<T> &v) {
        os << "(" << v.x << ", " << v.y << ")";
        return os;
    }

    Vector2<T> operator+(const T s) const {
        return {x + s, y + s};
    }

    Vector2<T> operator+(const Vector2<T> &v) const {
        return {x + v.x, y + v.y};
    }

    Vector2<T> operator-(const T s) const {
        return {x - s, y - s};
    }

    Vector2<T> operator-(const Vector2<T> &v) const {
        return {x - v.x, y - v.y};
    }

    Vector2<T> operator*(const T s) const {
        return {x * s, y * s};
    }

    Vector2<T> operator*(const Vector2<T> &v) const {
        return {x * v.x, y * v.y};
    }

    Vector2<T> operator/(const T s) const {
        return {x / s, y / s};
    }

    Vector2<T> operator/(const Vector2<T> &v) const {
        return {x / v.x, y / v.y};
    }

    T norm() const {
        return sqrt(x * x + y * y);
    }

    Vector2<T> normalized() const {
        return *this / norm();
    }

    void normalize() {
        const T s = norm();
        x /= s;
        y /= s;
    }

    T dot(const Vector2<T> &v) const {
        return x * v.x + y * v.y;
    }

    T cross(const Vector2<T> &v) const {
        return x * v.y - y * v.x;
    }

    static std::vector<Vector2<double>> convex_hull(const std::vector<Vector2<double>> &points) {
        namespace bg = boost::geometry;
        using BoostPoint = boost::tuple<double, double>;
        using BoostPolygon = bg::model::polygon<BoostPoint>;
        BoostPolygon input_polygon;
        for(const auto &[x, y]: points) {
            bg::append(input_polygon.outer(), BoostPoint(x, y));
        }
        BoostPolygon hull_polygon;
        bg::convex_hull(input_polygon, hull_polygon);
        std::vector<Vector2<double>> hull_points;
        for(const auto &hull_point: hull_polygon.outer()) {
            hull_points.emplace_back(Vector2<double>{bg::get<0>(hull_point), bg::get<1>(hull_point)});
        }
        return hull_points;
    }

    T get_angle() const {
        if(x == 0 && y == 0) {
            return 0;
        }
        return mod(atan2(y, x));
    }

    Vector2<T> rotate(const T angle) const {
        const T cos_angle = cos(angle);
        const T sin_angle = sin(angle);
        return {
                    x * cos_angle - y * sin_angle,
                    x * sin_angle + y * cos_angle
                };
    }

    bool is_inside_polygon(const std::vector<Vector2<double>> &vertices, const std::vector<double> &angles) const {
        const double angle = get_angle();
        const int upper_bound = static_cast<int>(std::ranges::upper_bound(angles, angle) - angles.begin());
        const int index_0 = upper_bound == 0 ? static_cast<int>(angles.size()) - 1 : upper_bound - 1;
        const int index_1 = upper_bound == static_cast<int>(angles.size()) ? 0 : upper_bound;
        const Vector2<T> d = vertices[index_1] - vertices[index_0];
        return d.cross(vertices[index_0]) < d.cross(*this);
    }
};

using Vector2f = Vector2<float>;
using Vector2d = Vector2<double>;
using Vector2I = Vector2<Interval>;

template<typename T>
struct Vector3 {
    T x;
    T y;
    T z;

    friend std::ostream &operator<<(std::ostream &os, const Vector3<T> &v) {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }

    Vector3<T> operator+(const T s) const {
        return {x + s, y + s, z + s};
    }

    Vector3<T> operator+(const Vector3<T> &v) const {
        return {x + v.x, y + v.y, z + v.z};
    }

    Vector3<T> operator-(const T s) const {
        return {x - s, y - s, z - s};
    }

    Vector3<T> operator-(const Vector3<T> &v) const {
        return {x - v.x, y - v.y, z - v.z};
    }

    Vector3<T> operator*(const T s) const {
        return {x * s, y * s, z * s};
    }

    Vector3<T> operator*(const Vector3<T> &v) const {
        return {x * v.x, y * v.y, z * v.z};
    }

    Vector3<T> operator/(const T s) const {
        return {x / s, y / s, z / s};
    }

    Vector3<T> operator/(const Vector3<T> &v) const {
        return {x / v.x, y / v.y, z / v.z};
    }

    T norm() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vector3<T> normalized() const {
        return *this / norm();
    }

    void normalize() {
        const T s = norm();
        x /= s;
        y /= s;
        z /= s;
    }

    T dot(const Vector3<T> &v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    Vector3<T> cross(const Vector3<T> &v) const {
        return {
                    y * v.z - z * v.y,
                    z * v.x - x * v.z,
                    x * v.y - y * v.x
                };
    }

    Vector2<T> project(const T theta, const T phi) const {
        const T sin_theta = sin(theta);
        const T cos_theta = cos(theta);
        return {
                    -x * sin_theta + y * cos_theta,
                    (x * cos_theta + y * sin_theta) * cos(phi) - z * sin(phi)
                };
    }
};

using Vector3f = Vector3<float>;
using Vector3d = Vector3<double>;
using Vector3I = Vector3<Interval>;

struct Interval2 {
    const Interval theta;
    const Interval phi;

    Vector2d center() const {
        return {
                    (theta.lower() + theta.upper()) / 2 / M_PI - 1,
                    (phi.lower() + phi.upper()) / 2 / M_PI_2 - 1
                };
    }

    Interval2 normalized() const {
        return {
                    Interval(theta.lower() / M_PI - 1, theta.upper() / M_PI - 1),
                    Interval(phi.lower() / M_PI_2 - 1, phi.upper() / M_PI_2 - 1)
                };
    }

    cv::Scalar color() const {
        const double x = (theta.lower() + theta.upper()) / 2 / (2 * M_PI);
        const double y = (phi.lower() + phi.upper()) / 2 / M_PI;
        return Color::gradient(y,
                               Color::gradient(x, Color::bottom_left, Color::bottom_right),
                               Color::gradient(x, Color::top_left, Color::top_right));
    }

    std::vector<Vector2d> boundary(const Vector3d &v, const int theta_parts, int phi_parts) const {
        std::vector<Vector2d> points;
        const double theta_step = theta.length() / theta_parts;
        const double phi_step = phi.length() / phi_parts;
        for(int phi_i = 0; phi_i < phi_parts; phi_i++) {
            points.push_back(v.project(theta.lower(), phi.lower() + phi_i * phi_step));
        }
        for(int theta_i = 0; theta_i < theta_parts; theta_i++) {
            points.push_back(v.project(theta.lower() + theta_i * theta_step, phi.upper()));
        }
        for(int phi_i = phi_parts; phi_i > 0; phi_i--) {
            points.push_back(v.project(theta.upper(), phi.lower() + phi_i * phi_step));
        }
        for(int theta_i = theta_parts; theta_i > 0; theta_i--) {
            points.push_back(v.project(theta.lower() + theta_i * theta_step, phi.lower()));
        }
        return points;
    }

    static bool intersects_line_fixed_phi(const Vector3d &v, const Vector2d &vertex_0, const Vector2d &vertex_1, const Interval &theta_interval, const double phi) {
        const double y_shift = v.z * std::sin(phi);
        const double y_scale = std::cos(phi);
        const Vector2 a(vertex_0.x, (vertex_0.y + y_shift) / y_scale);
        const Vector2 b(vertex_1.x, (vertex_1.y + y_shift) / y_scale);

        const Vector2 d = b - a;
        const double A_double = 2 * d.dot(d);
        const double B = 2 * d.dot(a);
        const double C = a.dot(a) - (v.x * v.x + v.y * v.y);
        const double discriminant = B * B - 2 * A_double * C;
        if(discriminant < 0) {
            return false;
        }

        const double sqrt_discriminant = std::sqrt(discriminant);
        const auto solutions = {
                    (-B + sqrt_discriminant) / A_double,
                    (-B - sqrt_discriminant) / A_double
                };
        return std::ranges::any_of(solutions, [&](const double solution) {
            return 0 <= solution && solution <= 1 &&
                   theta_interval.contains(mod((a + d * solution).get_angle() - Vector2(v.y, v.x).get_angle()));
        });
    }

    static bool intersects_line_fixed_theta(const Vector3d &v, const Vector2d &vertex_0, const Vector2d &vertex_1, const double theta, const Interval &phi_interval) {
        const double sin_theta = std::sin(theta);
        const double cos_theta = std::cos(theta);
        const double x = -v.x * sin_theta + v.y * cos_theta;
        if(x < std::min(vertex_0.x, vertex_1.x) || x > std::max(vertex_0.x, vertex_1.x)) {
            return false;
        }
        const Vector2 coefficient(v.x * cos_theta + v.y * sin_theta, -v.z);
        const double r = coefficient.norm();
        const double shift = coefficient.get_angle();
        const double y_0 = r * std::cos(phi_interval.lower() - shift);
        const double y_1 = r * std::cos(phi_interval.upper() - shift);
        const double min_y = phi_interval.contains(mod(shift + M_PI)) ? -1
                                 : std::min(y_0, y_1);
        const double max_y = phi_interval.contains(mod(shift)) ? 1
                                 : std::max(y_0, y_1);
        const auto [dx, dy] = vertex_1 - vertex_0;
        const double y = vertex_0.y + dy * (x - vertex_0.x) / dx;
        return min_y <= y && y <= max_y;
    }

    bool intersects_line(const Vector3d &v, const Vector2d &vertex_0, const Vector2d &vertex_1) const {
        return intersects_line_fixed_phi(v, vertex_0, vertex_1, theta, phi.lower()) ||
               intersects_line_fixed_phi(v, vertex_0, vertex_1, theta, phi.upper()) ||
               intersects_line_fixed_theta(v, vertex_0, vertex_1, theta.lower(), phi) ||
               intersects_line_fixed_theta(v, vertex_0, vertex_1, theta.upper(), phi);
    }

    bool intersects_polygon(const Vector3d &v, const std::vector<Vector2d> &vertices, const std::vector<double> &angles) const {
        if(v.project(theta.lower(), phi.lower()).is_inside_polygon(vertices, angles) ||
           v.project(theta.lower(), phi.upper()).is_inside_polygon(vertices, angles) ||
           v.project(theta.upper(), phi.lower()).is_inside_polygon(vertices, angles) ||
           v.project(theta.upper(), phi.upper()).is_inside_polygon(vertices, angles)) {
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
