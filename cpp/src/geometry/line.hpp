#pragma once

#include "geometry/util.hpp"

template<IntervalType Interval>
struct Line {
private:
    Vector2<Interval> point_;
    Vector2<Interval> direction_;

public:
    explicit Line(const Vector2<Interval>& point, const Vector2<Interval>& direction) : point_(point), direction_(direction) {}

    explicit Line(const Vector2<Interval>& direction) : point_(Vector2<Interval>(0, 0)), direction_(direction) {}

    static Line from_two_points(const Vector2<Interval>& point, const Vector2<Interval>& other_point) {
        return DirectedLine(point, other_point - point);
    }

    Orientation orientation(const Vector2<Interval>& vector2) const {
        const Interval cross = direction_.cross(vector2 - point_);
        if(cross.is_pos()) {
            return Orientation::COUNTERCLOCKWISE;
        }
        if(cross.is_neg()) {
            return Orientation::CLOCKWISE;
        }
        return Orientation::COLLINEAR;
    }

    friend std::ostream& operator<<(std::ostream& os, const Line& line) {
        return os << line.point_ << " @ " << line.direction_;
    }
};
