#pragma once

#include "geometry/vector.hpp"

enum class Orientation {
    positive,
    negative,
    ambiguous
};

inline std::ostream& operator<<(std::ostream& os, const Orientation& orientation) {
    switch(orientation) {
        case Orientation::positive: return os << "positive";
        case Orientation::negative: return os << "negative";
        case Orientation::ambiguous: return os << "ambiguous";
        default: throw std::runtime_error("Unknown orientation");
    }
}

constexpr bool same_orientation(const Orientation orientation_0, const Orientation orientation_1) {
    return orientation_0 != Orientation::ambiguous &&
           orientation_1 != Orientation::ambiguous &&
           orientation_0 == orientation_1;
}

constexpr bool opposite_orientation(const Orientation orientation_0, const Orientation orientation_1) {
    return orientation_0 != Orientation::ambiguous &&
           orientation_1 != Orientation::ambiguous &&
           orientation_0 != orientation_1;
}

template<IntervalType Interval>
struct Edge {
private:
    Vector<Interval> from_;
    Vector<Interval> to_;

public:
    explicit Edge(const Vector<Interval>& from, const Vector<Interval>& to) : from_(from), to_(to) {}

    const Vector<Interval>& from() const {
        return from_;
    }

    const Vector<Interval>& to() const {
        return to_;
    }

    Vector<Interval> dir() const {
        return (to_ - from_).unit();
    }

    Interval len() const {
        return dir().len();
    }

    Vector<Interval> mid() const {
        return (from_ + to_) / Interval(2);
    }

    Orientation orientation(const Vector<Interval>& vector) const {
        const Interval cross = dir().cross(vector - from_);
        if(cross.is_positive()) {
            return Orientation::positive;
        }
        if(cross.is_negative()) {
            return Orientation::negative;
        }
        return Orientation::ambiguous;
    }

    bool intersects(const Edge& edge) const {
        return opposite_orientation(orientation(edge.from_), orientation(edge.to_)) &&
               opposite_orientation(edge.orientation(from_), edge.orientation(to_));
    }

    bool avoids(const Vector<Interval>& vector) const {
        if(orientation(vector) != Orientation::ambiguous) {
            return true;
        }
        return vector.dist(mid()) > len() / 2;
    }

    bool avoids(const Edge& edge) const {
        if(same_orientation(orientation(edge.from_), orientation(edge.to_)) ||
           same_orientation(edge.orientation(from_), edge.orientation(to_))) {
            return true;
        }
        const Interval from_dot = dir().dot(edge.from() - from_);
        const Interval to_dot = dir().dot(edge.to() - from_);
        if((from_dot.is_negative() && to_dot.is_negative()) ||
           (from_dot > len() && to_dot > len())) {
            return true;
        }
        const Interval edge_from_dot = edge.dir().dot(from_ - edge.from());
        const Interval edge_to_dot = edge.dir().dot(to_ - edge.from());
        if((edge_from_dot.is_negative() && edge_to_dot.is_negative()) ||
           (edge_from_dot > edge.len() && edge_to_dot > edge.len())) {
            return true;
        }
        return edge.mid().dist(mid()) > (len() + edge.len()) / 2;
    }

    friend std::ostream& operator<<(std::ostream& os, const Edge& edge) {
        return os << edge.from_ << " -> " << edge.to_;
    }
};
