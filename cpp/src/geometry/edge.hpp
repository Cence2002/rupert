#pragma once

#include "vector.hpp"

enum class Orientation {
    COUNTERCLOCKWISE,
    CLOCKWISE,
    COLLINEAR
};

bool same_orientation(const Orientation orientation_0, const Orientation orientation_1) {
    return orientation_0 != Orientation::COLLINEAR &&
           orientation_1 != Orientation::COLLINEAR &&
           orientation_0 == orientation_1;
}

bool opposite_orientation(const Orientation orientation_0, const Orientation orientation_1) {
    return orientation_0 != Orientation::COLLINEAR &&
           orientation_1 != Orientation::COLLINEAR &&
           orientation_0 != orientation_1;
}

template<IntervalType Interval>
struct Edge {
private:
    Vector2Interval<Interval> from_;
    Vector2Interval<Interval> to_;

public:
    explicit Edge(const Vector2Interval<Interval>& from, const Vector2Interval<Interval>& to) noexcept: from_(from), to_(to) {}

    Vector2Interval<Interval> from() const {
        return from_;
    }

    Vector2Interval<Interval> to() const {
        return to_;
    }

    Vector2Interval<Interval> direction() const {
        return to_ - from_;
    }

    Orientation orientation(const Vector2Interval<Interval>& vector2) const {
        const Interval cross = Vector2Interval<Interval>::cross(direction(), vector2 - from_);
        if(cross.is_pos()) {
            return Orientation::COUNTERCLOCKWISE;
        }
        if(cross.is_neg()) {
            return Orientation::CLOCKWISE;
        }
        return Orientation::COLLINEAR;
    }

    bool intersects(const Edge& edge) const {
        return opposite_orientation(orientation(edge.from_), orientation(edge.to_)) &&
               opposite_orientation(edge.orientation(from_), edge.orientation(to_));
    }

    bool avoids(const Edge& edge) const {
        return same_orientation(orientation(edge.from_), orientation(edge.to_)) ||
               same_orientation(edge.orientation(from_), edge.orientation(to_));
    }

    bool avoids(const Vector2Interval<Interval>& vector2) const {
        if(orientation(vector2) != Orientation::COLLINEAR) {
            return true;
        }
        const Interval dot = Vector2Interval<Interval>::dot(direction(), vector2 - Vector2Interval<Interval>(from_));
        return dot.is_neg() || dot > Vector2Interval<Interval>::dot(direction(), direction());
    }

    friend std::ostream& operator<<(std::ostream& os, const Edge& edge) {
        return os << edge.from_ << " -> " << edge.to_;
    }
};
