#pragma once

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/adapted/boost_tuple.hpp>

BOOST_GEOMETRY_REGISTER_BOOST_TUPLE_CS(cs::cartesian)

template<typename T>
struct Vector2 {
    T x, y;

    friend std::ostream &operator<<(std::ostream &os, const Vector2 &v) {
        os << "(" << v.x << ", " << v.y << ")";
        return os;
    }

    Vector2 operator+(const T s) const {
        return {x + s, y + s};
    }

    Vector2 operator+(const Vector2 &v) const {
        return {x + v.x, y + v.y};
    }

    Vector2 operator-(const T s) const {
        return {x - s, y - s};
    }

    Vector2 operator-(const Vector2 &v) const {
        return {x - v.x, y - v.y};
    }

    Vector2 operator*(const T s) const {
        return {x * s, y * s};
    }

    Vector2 operator*(const Vector2 &v) const {
        return {x * v.x, y * v.y};
    }

    Vector2 operator/(const T s) const {
        return {x / s, y / s};
    }

    Vector2 operator/(const Vector2 &v) const {
        return {x / v.x, y / v.y};
    }

    T r() const {
        return sqrt(x * x + y * y);
    }

    Vector2 unit() const {
        return *this / r();
    }

    T dot(const Vector2 &v) const {
        return x * v.x + y * v.y;
    }

    T cross(const Vector2 &v) const {
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
        const double angle = atan2(y, x);
        return angle < 0 ? angle + 2 * M_PI : angle;
    }

    Vector2 rotate(const T angle) const {
        const T cos_angle = cos(angle);
        const T sin_angle = sin(angle);
        return {
                    x * cos_angle - y * sin_angle,
                    x * sin_angle + y * cos_angle
                };
    }

    template<typename = std::enable_if<std::is_floating_point_v<T>>>
    bool is_inside_polygon(const std::vector<Vector2<double>> &vertices, const std::vector<double> &angles) const {
        const double angle = get_angle();
        const int upper_bound = static_cast<int>(std::ranges::upper_bound(angles, angle) - angles.begin());
        const int index_0 = upper_bound == 0 ? static_cast<int>(angles.size()) - 1 : upper_bound - 1;
        const int index_1 = upper_bound == static_cast<int>(angles.size()) ? 0 : upper_bound;
        const Vector2 d = vertices[index_1] - vertices[index_0];
        return d.cross(vertices[index_0]) < d.cross(*this);
    }
};

using Vector2i = Vector2<int>;
using Vector2f = Vector2<float>;
using Vector2d = Vector2<double>;

template<typename T>
struct Vector3 {
    T x, y, z;

    friend std::ostream &operator<<(std::ostream &os, const Vector3 &v) {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }

    Vector3 operator+(const T s) const {
        return {x + s, y + s, z + s};
    }

    Vector3 operator+(const Vector3 &v) const {
        return {x + v.x, y + v.y, z + v.z};
    }

    Vector3 operator-(const T s) const {
        return {x - s, y - s, z - s};
    }

    Vector3 operator-(const Vector3 &v) const {
        return {x - v.x, y - v.y, z - v.z};
    }

    Vector3 operator*(const T s) const {
        return {x * s, y * s, z * s};
    }

    Vector3 operator*(const Vector3 &v) const {
        return {x * v.x, y * v.y, z * v.z};
    }

    Vector3 operator/(const T s) const {
        return {x / s, y / s, z / s};
    }

    Vector3 operator/(const Vector3 &v) const {
        return {x / v.x, y / v.y, z / v.z};
    }

    T r() const {
        return sqrt(x * x + y * y + z * z);
    }

    Vector3 unit() const {
        return *this / r();
    }

    T dot(const Vector3 &v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    Vector3 cross(const Vector3 &v) const {
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

using Vector3i = Vector3<int>;
using Vector3f = Vector3<float>;
using Vector3d = Vector3<double>;
