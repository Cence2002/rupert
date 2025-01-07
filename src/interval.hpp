#pragma once

#include "vector.hpp"
#include <boost/numeric/interval.hpp>
#include <boost/numeric/interval/transc.hpp>

using Interval = boost::numeric::interval<
    double,
    boost::numeric::interval_lib::policies<
        boost::numeric::interval_lib::save_state<
            boost::numeric::interval_lib::rounded_transc_std<double>
        >,
        boost::numeric::interval_lib::checking_base<double>
    >
>;

using Vector2I = Vector2<Interval>;
using Vector3I = Vector3<Interval>;

template<typename T>
constexpr T mod(const T &angle) {
    return fmod(fmod(angle, 2 * M_PI) + 2 * M_PI, 2 * M_PI);
}

template<typename T>
inline T map(const T value, const Interval &from, const Interval &to) {
    return (value - lower(from)) / width(from) * width(to) + lower(to);
}

inline std::vector<Interval> divide(const Interval &interval, const int parts) {
    std::vector<double> points;
    points.push_back(lower(interval));
    for(int i = 1; i < parts; i++) {
        points.push_back(lower(interval) + i * width(interval) / parts);
    }
    points.push_back(upper(interval));
    std::vector<Interval> intervals;
    for(size_t i = 0; i < points.size() - 1; i++) {
        intervals.emplace_back(points[i], points[i + 1]);
    }
    return intervals;
}

inline std::ostream &operator<<(std::ostream &os, const Interval &interval) {
    os << "[" << lower(interval) << ", " << upper(interval) << "]";
    return os;
}

struct Interval2 {
    Interval interval_0, interval_1;

    Vector2d center() const {
        return {
                    (lower(interval_0) + upper(interval_0)) / 2,
                    (lower(interval_1) + upper(interval_1)) / 2
                };
    }

    std::array<Vector2d, 4> vertices() const {
        return {
                    Vector2(lower(interval_0), lower(interval_1)),
                    Vector2(lower(interval_0), upper(interval_1)),
                    Vector2(upper(interval_0), upper(interval_1)),
                    Vector2(upper(interval_0), lower(interval_1))
                };
    }

    std::array<Interval2, 4> parts() const {
        const auto [center_x, center_y] = center();
        return {
                    Interval2{{lower(interval_0), center_x}, {lower(interval_1), center_y}},
                    Interval2{{lower(interval_0), center_x}, {center_y, upper(interval_1)}},
                    Interval2{{center_x, upper(interval_0)}, {center_y, upper(interval_1)}},
                    Interval2{{center_x, upper(interval_0)}, {lower(interval_1), center_y}}
                };
    }

    std::vector<Vector2d> boundary(const Vector3d &v, const int theta_parts, int phi_parts) const {
        std::vector<Vector2d> points;
        const double step_0 = width(interval_0) / theta_parts;
        const double step_1 = width(interval_1) / phi_parts;
        for(int i = 0; i < phi_parts; i++) {
            points.push_back(v.project(lower(interval_0), lower(interval_1) + i * step_1));
        }
        for(int i = 0; i < theta_parts; i++) {
            points.push_back(v.project(lower(interval_0) + i * step_0, upper(interval_1)));
        }
        for(int i = phi_parts; i > 0; i--) {
            points.push_back(v.project(upper(interval_0), lower(interval_1) + i * step_1));
        }
        for(int i = theta_parts; i > 0; i--) {
            points.push_back(v.project(lower(interval_0) + i * step_0, lower(interval_1)));
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
                   in(mod((a + d * solution).get_angle() - Vector2(v.y, v.x).get_angle()), theta_interval);
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
        const double r = coefficient.r();
        const double shift = coefficient.get_angle();
        const double y_0 = r * std::cos(lower(phi_interval) - shift);
        const double y_1 = r * std::cos(upper(phi_interval) - shift);
        const double min_y = in(mod(shift + M_PI), phi_interval) ? -1
                                 : std::min(y_0, y_1);
        const double max_y = in(mod(shift), phi_interval) ? 1
                                 : std::max(y_0, y_1);
        const auto [dx, dy] = vertex_1 - vertex_0;
        const double y = vertex_0.y + dy * (x - vertex_0.x) / dx;
        return min_y <= y && y <= max_y;
    }

    bool intersects_line(const Vector3d &v, const Vector2d &vertex_0, const Vector2d &vertex_1) const {
        return intersects_line_fixed_phi(v, vertex_0, vertex_1, interval_0, lower(interval_1)) ||
               intersects_line_fixed_phi(v, vertex_0, vertex_1, interval_0, upper(interval_1)) ||
               intersects_line_fixed_theta(v, vertex_0, vertex_1, lower(interval_0), interval_1) ||
               intersects_line_fixed_theta(v, vertex_0, vertex_1, upper(interval_0), interval_1);
    }

    bool intersects_polygon(const Vector3d &v, const std::vector<Vector2d> &vertices, const std::vector<double> &angles) const {
        if(v.project(lower(interval_0), lower(interval_1)).is_inside_polygon(vertices, angles) ||
           v.project(lower(interval_0), upper(interval_1)).is_inside_polygon(vertices, angles) ||
           v.project(upper(interval_0), lower(interval_1)).is_inside_polygon(vertices, angles) ||
           v.project(upper(interval_0), upper(interval_1)).is_inside_polygon(vertices, angles)) {
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

struct Interval3 {
    Interval interval_0, interval_1, interval_2;

    Vector3d center() const {
        return {
                    (lower(interval_0) + upper(interval_0)) / 2,
                    (lower(interval_1) + upper(interval_1)) / 2,
                    (lower(interval_2) + upper(interval_2)) / 2
                };
    }

    std::array<Vector3d, 8> vertices() const {
        return {
                    Vector3d(lower(interval_0), lower(interval_1), lower(interval_2)),
                    Vector3d(lower(interval_0), lower(interval_1), upper(interval_2)),
                    Vector3d(lower(interval_0), upper(interval_1), lower(interval_2)),
                    Vector3d(lower(interval_0), upper(interval_1), upper(interval_2)),
                    Vector3d(upper(interval_0), lower(interval_1), lower(interval_2)),
                    Vector3d(upper(interval_0), lower(interval_1), upper(interval_2)),
                    Vector3d(upper(interval_0), upper(interval_1), lower(interval_2)),
                    Vector3d(upper(interval_0), upper(interval_1), upper(interval_2))
                };
    }

    std::array<Interval3, 8> parts() const {
        const auto [center_x, center_y, center_z] = center();
        return {
                    Interval3{{lower(interval_0), center_x}, {lower(interval_1), center_y}, {lower(interval_2), center_z}},
                    Interval3{{lower(interval_0), center_x}, {lower(interval_1), center_y}, {center_z, upper(interval_2)}},
                    Interval3{{lower(interval_0), center_x}, {center_y, upper(interval_1)}, {lower(interval_2), center_z}},
                    Interval3{{lower(interval_0), center_x}, {center_y, upper(interval_1)}, {center_z, upper(interval_2)}},
                    Interval3{{center_x, upper(interval_0)}, {lower(interval_1), center_y}, {lower(interval_2), center_z}},
                    Interval3{{center_x, upper(interval_0)}, {lower(interval_1), center_y}, {center_z, upper(interval_2)}},
                    Interval3{{center_x, upper(interval_0)}, {center_y, upper(interval_1)}, {lower(interval_2), center_z}},
                    Interval3{{center_x, upper(interval_0)}, {center_y, upper(interval_1)}, {center_z, upper(interval_2)}}
                };
    }
};
