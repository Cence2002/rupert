#pragma once

#include "vector.hpp"

enum class Orientation {
    COUNTERCLOCKWISE,
    CLOCKWISE,
    COLLINEAR
};

inline std::ostream& operator<<(std::ostream& os, const Orientation& orientation) {
    switch(orientation) {
        case Orientation::COUNTERCLOCKWISE: return os << "COUNTERCLOCKWISE";
        case Orientation::CLOCKWISE: return os << "CLOCKWISE";
        case Orientation::COLLINEAR: return os << "COLLINEAR";
        default: throw std::runtime_error("Unknown orientation");
    }
}

constexpr bool same_orientation(const Orientation orientation_0, const Orientation orientation_1) {
    return orientation_0 != Orientation::COLLINEAR &&
           orientation_1 != Orientation::COLLINEAR &&
           orientation_0 == orientation_1;
}

constexpr bool opposite_orientation(const Orientation orientation_0, const Orientation orientation_1) {
    return orientation_0 != Orientation::COLLINEAR &&
           orientation_1 != Orientation::COLLINEAR &&
           orientation_0 != orientation_1;
}

template<IntervalType Interval>
struct Edge {
private:
    Vector<Interval> from_;
    Vector<Interval> to_;

public:
    explicit Edge(const Vector<Interval>& from, const Vector<Interval>& to) noexcept: from_(from), to_(to) {}

    Vector<Interval> from() const {
        return from_;
    }

    Vector<Interval> to() const {
        return to_;
    }

    Vector<Interval> direction() const {
        return to_ - from_;
    }

    Orientation orientation(const Vector<Interval>& vector) const {
        const Interval cross = Vector<Interval>::cross(direction(), vector - from_);
        if(cross.is_positive()) {
            return Orientation::COUNTERCLOCKWISE;
        }
        if(cross.is_negative()) {
            return Orientation::CLOCKWISE;
        }
        return Orientation::COLLINEAR;
    }

    bool intersects(const Edge& edge) const {
        return opposite_orientation(orientation(edge.from_), orientation(edge.to_)) &&
               opposite_orientation(edge.orientation(from_), edge.orientation(to_));
    }

    bool avoids(const Edge& edge) const {
        if(same_orientation(orientation(edge.from_), orientation(edge.to_)) ||
           same_orientation(edge.orientation(from_), edge.orientation(to_))) {
            return true;
        }
        const Interval from_dot = Vector<Interval>::dot(direction(), edge.from() - Vector<Interval>(from_));
        const Interval to_dot = Vector<Interval>::dot(direction(), edge.to() - Vector<Interval>(from_));
        if(from_dot.is_negative() && to_dot.is_negative()) {
            return true;
        }
        if(from_dot > direction().len_sqr() && to_dot > direction().len_sqr()) {
            return true;
        }
        const Interval edge_from_dot = Vector<Interval>::dot(edge.direction(), from_ - edge.from());
        const Interval edge_to_dot = Vector<Interval>::dot(edge.direction(), to_ - edge.from());
        if(edge_from_dot.is_negative() && edge_to_dot.is_negative()) {
            return true;
        }
        if(edge_from_dot > edge.direction().len_sqr() && edge_to_dot > edge.direction().len_sqr()) {
            return true;
        }
        const Vector<Interval> midpoint = (from_ + to_) / Interval(2);
        const Vector<Interval> edge_midpoint = (edge.from() + edge.to()) / Interval(2);
        return (edge_midpoint - midpoint).len() > (direction().len() + edge.direction().len()) / 2;
    }

    bool avoids(const Vector<Interval>& vector) const {
        if(orientation(vector) != Orientation::COLLINEAR) {
            return true;
        }
        const Interval dot = Vector<Interval>::dot(direction(), vector - from_);
        if(dot.is_negative() || dot > direction().len_sqr()) {
            return true;
        }
        const Vector<Interval> midpoint = (from_ + to_) / Interval(2);
        return (vector - midpoint).len() > direction().len() / 2;
    }

    friend std::ostream& operator<<(std::ostream& os, const Edge& edge) {
        return os << edge.from_ << " -> " << edge.to_;
    }
};
