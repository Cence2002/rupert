#pragma once

#include "interval/interval.hpp"
#include "vector/interval_vector2.hpp"

enum class Orientation {
    COUNTERCLOCKWISE,
    CLOCKWISE,
    COLLINEAR
};

template<IntervalType Interval>
struct Line {
private:
    IntervalVector2<Interval> point_;
    IntervalVector2<Interval> direction_;

public:
    explicit Line(const IntervalVector2<Interval>& point, const IntervalVector2<Interval>& direction) : point_(point), direction_(direction) {}

    explicit Line(const IntervalVector2<Interval>& direction) : point_(IntervalVector2<Interval>(0, 0)), direction_(direction) {}

    IntervalVector2<Interval> point() const {
        return point_;
    }

    IntervalVector2<Interval> direction() const {
        return direction_;
    }

    static Line from_two_points(const IntervalVector2<Interval>& point, const IntervalVector2<Interval>& other_point) {
        return DirectedLine(point, other_point - point);
    }

    Orientation orientation(const IntervalVector2<Interval>& interval_vector2) const {
        const Interval cross = direction_.cross(interval_vector2 - point_);
        if(cross.is_pos()) {
            return Orientation::COUNTERCLOCKWISE;
        }
        if(cross.is_neg()) {
            return Orientation::CLOCKWISE;
        }
        return Orientation::COLLINEAR;
    }

    bool intersects(const Line& line) const {
        const Orientation orientation_0_0 = orientation(line.point_);
        const Orientation orientation_0_1 = orientation(line.point_ + line.direction_);
        const Orientation orientation_1_0 = line.orientation(point_);
        const Orientation orientation_1_1 = line.orientation(point_ + direction_);
        if(orientation_0_0 == Orientation::COLLINEAR ||
           orientation_0_1 == Orientation::COLLINEAR ||
           orientation_1_0 == Orientation::COLLINEAR ||
           orientation_1_1 == Orientation::COLLINEAR) {
            return false;
        }
        return (orientation_0_0 != orientation_0_1) && (orientation_1_0 != orientation_1_1);
    }

    bool avoids(const Line& line) const {
        const Orientation orientation_0_0 = orientation(line.point_);
        const Orientation orientation_0_1 = orientation(line.point_ + line.direction_);
        const Orientation orientation_1_0 = line.orientation(point_);
        const Orientation orientation_1_1 = line.orientation(point_ + direction_);
        if(orientation_0_0 == Orientation::COLLINEAR ||
           orientation_0_1 == Orientation::COLLINEAR ||
           orientation_1_0 == Orientation::COLLINEAR ||
           orientation_1_1 == Orientation::COLLINEAR) {
            return false;
        }
        return (orientation_0_0 == orientation_0_1) && (orientation_1_0 == orientation_1_1);
    }

    bool avoids(const IntervalVector2<Interval>& interval_vector2) const {
        if(orientation(interval_vector2) != Orientation::COLLINEAR) {
            return true;
        }
        const Interval dot = direction_.dot(interval_vector2 - point_);
        return dot.is_neg() || dot > direction_.dot(direction_);
    }

    friend std::ostream& operator<<(std::ostream& os, const Line& line) {
        return os << line.point_ << " @ " << line.direction_;
    }
};
