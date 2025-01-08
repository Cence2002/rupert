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

inline std::ostream &operator<<(std::ostream &os, const Interval &interval) {
    os << "[" << lower(interval) << ", " << upper(interval) << "]";
    return os;
}

inline const Interval UNIT{0, 1};

using Vector2I = Vector2<Interval>;
using Vector3I = Vector3<Interval>;

inline std::vector<Interval> divide(const Interval &interval, const int parts) {
    std::vector<double> cutoffs;
    cutoffs.push_back(lower(interval));
    const double step = width(interval) / parts;
    for(int i = 1; i < parts; i++) {
        cutoffs.push_back(lower(interval) + i * step);
    }
    cutoffs.push_back(upper(interval));
    std::vector<Interval> intervals;
    for(size_t i = 0; i < cutoffs.size() - 1; i++) {
        intervals.emplace_back(cutoffs[i], cutoffs[i + 1]);
    }
    return intervals;
}

struct Box2 {
    Interval theta, phi;

    inline static const Interval theta_interval{0, boost::numeric::interval_lib::constants::pi_twice_upper<double>()};
    inline static const Interval phi_interval{0, boost::numeric::interval_lib::constants::pi_upper<double>()};

    Vector2d center() const {
        return {median(theta), median(phi)};
    }

    Box2 normalized() const {
        return {
                    theta / upper(theta_interval),
                    phi / upper(phi_interval)
                };
    }

    std::vector<Vector2d> vertices() const {
        return {
                    {lower(theta), lower(phi)},
                    {lower(theta), upper(phi)},
                    {upper(theta), upper(phi)},
                    {upper(theta), lower(phi)}
                };
    }

    std::vector<Box2> sub_boxes() const {
        const auto [median_theta, median_phi] = center();
        return {
                    {{lower(theta), median_theta}, {lower(phi), median_phi}},
                    {{lower(theta), median_theta}, {median_phi, upper(phi)}},
                    {{median_theta, upper(theta)}, {lower(phi), median_phi}},
                    {{median_theta, upper(theta)}, {median_phi, upper(phi)}}
                };
    }
};

inline std::ostream &operator<<(std::ostream &os, const Box2 &box) {
    os << "[" << box.theta << ", " << box.phi << "]";
    return os;
}

inline bool box_intersects_polygon(const Vector3d &point, const Box2 &box, const std::vector<Vector2d> &vertices) {
    const Vector2I projection = Vector3I(point.x, point.y, point.z).project(box.theta, box.phi);
    const Box2 projection_box{projection.x, projection.y};
    const std::vector<Vector2d> box_vertices = projection_box.vertices();
    return point_inside_polygon(projection_box.center(), vertices) ||
           line_intersects_polygon(box_vertices[0], box_vertices[1], vertices) ||
           line_intersects_polygon(box_vertices[2], box_vertices[3], vertices) ||
           line_intersects_polygon(box_vertices[1], box_vertices[2], vertices) ||
           line_intersects_polygon(box_vertices[3], box_vertices[0], vertices) ||
           in(0.0, projection_box.theta) && in(0.0, projection_box.phi);
}

struct Box3 {
    Interval theta, phi, alpha;

    inline static const Interval theta_interval{0, boost::numeric::interval_lib::constants::pi_twice_upper<double>()};
    inline static const Interval phi_interval{0, boost::numeric::interval_lib::constants::pi_upper<double>()};
    inline static const Interval alpha_interval{0, boost::numeric::interval_lib::constants::pi_twice_upper<double>()};

    Vector3d center() const {
        return {median(theta), median(phi), median(alpha)};
    }

    Box3 normalized() const {
        return {
                    theta / upper(theta_interval),
                    phi / upper(phi_interval),
                    alpha / upper(alpha_interval)
                };
    }

    std::vector<Box3> sub_boxes() const {
        const auto [median_theta, median_phi, median_alpha] = center();
        return {
                    {{lower(theta), median_theta}, {lower(phi), median_phi}, {lower(alpha), median_alpha}},
                    {{lower(theta), median_theta}, {lower(phi), median_phi}, {median_alpha, upper(alpha)}},
                    {{lower(theta), median_theta}, {median_phi, upper(phi)}, {lower(alpha), median_alpha}},
                    {{lower(theta), median_theta}, {median_phi, upper(phi)}, {median_alpha, upper(alpha)}},
                    {{median_theta, upper(theta)}, {lower(phi), median_phi}, {lower(alpha), median_alpha}},
                    {{median_theta, upper(theta)}, {lower(phi), median_phi}, {median_alpha, upper(alpha)}},
                    {{median_theta, upper(theta)}, {median_phi, upper(phi)}, {lower(alpha), median_alpha}},
                    {{median_theta, upper(theta)}, {median_phi, upper(phi)}, {median_alpha, upper(alpha)}}
                };
    }
};

inline std::ostream &operator<<(std::ostream &os, const Box3 &box) {
    os << "[" << box.theta << ", " << box.phi << ", " << box.alpha << "]";
    return os;
}
