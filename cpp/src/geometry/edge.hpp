#pragma once

#include "geometry/vector.hpp"

enum class Orientation {
    counterclockwise,
    clockwise,
    collinear
};

inline std::ostream& operator<<(std::ostream& os, const Orientation& orientation) {
    switch(orientation) {
        case Orientation::counterclockwise: return os << "counterclockwise";
        case Orientation::clockwise: return os << "clockwise";
        case Orientation::collinear: return os << "collinear";
        default: throw std::runtime_error("Unknown orientation");
    }
}

constexpr bool same_orientation(const Orientation orientation_0, const Orientation orientation_1) {
    return orientation_0 != Orientation::collinear &&
           orientation_1 != Orientation::collinear &&
           orientation_0 == orientation_1;
}

constexpr bool opposite_orientation(const Orientation orientation_0, const Orientation orientation_1) {
    return orientation_0 != Orientation::collinear &&
           orientation_1 != Orientation::collinear &&
           orientation_0 != orientation_1;
}

template<IntervalType Interval>
struct Edge {
private:
    Vector<Interval> from_;
    Vector<Interval> to_;

public:
    explicit Edge(const Vector<Interval>& from, const Vector<Interval>& to) noexcept : from_(from), to_(to) {}

    Vector<Interval> from() const {
        return from_;
    }

    Vector<Interval> to() const {
        return to_;
    }

    Vector<Interval> direction() const {
        return to_ - from_;
    }

    Vector<Interval> midpoint() const {
        return (from_ + to_) / Interval(2);
    }

    Orientation orientation(const Vector<Interval>& vector) const {
        const Interval cross = direction().cross(vector - from_);
        if(cross.is_positive()) {
            return Orientation::counterclockwise;
        }
        if(cross.is_negative()) {
            return Orientation::clockwise;
        }
        return Orientation::collinear;
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
        const Interval from_dot = direction().dot(edge.from() - Vector<Interval>(from_));
        const Interval to_dot = direction().dot(edge.to() - Vector<Interval>(from_));
        if(from_dot.is_negative() && to_dot.is_negative()) {
            return true;
        }
        if(from_dot > direction().len_sqr() && to_dot > direction().len_sqr()) {
            return true;
        }
        const Interval edge_from_dot = edge.direction().dot(from_ - edge.from());
        const Interval edge_to_dot = edge.direction().dot(to_ - edge.from());
        if(edge_from_dot.is_negative() && edge_to_dot.is_negative()) {
            return true;
        }
        if(edge_from_dot > edge.direction().len_sqr() && edge_to_dot > edge.direction().len_sqr()) {
            return true;
        }
        return (edge.midpoint() - midpoint()).len() > (direction().len() + edge.direction().len()) / 2;
    }

    bool avoids(const Vector<Interval>& vector) const {
        if(orientation(vector) != Orientation::collinear) {
            return true;
        }
        const Interval dot = direction().dot(vector - from_);
        if(dot.is_negative() || dot > direction().len_sqr()) {
            return true;
        }
        return (vector - midpoint()).len() > direction().len() / 2;
    }

    friend std::ostream& operator<<(std::ostream& os, const Edge& edge) {
        return os << edge.from_ << " -> " << edge.to_;
    }
};
