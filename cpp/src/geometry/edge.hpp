#pragma once

#include "geometry/vector.hpp"

enum class Side {
    left,
    right,
    ambiguous
};

inline std::ostream& operator<<(std::ostream& os, const Side& side) {
    switch(side) {
        case Side::left: return os << "left";
        case Side::right: return os << "right";
        case Side::ambiguous: return os << "ambiguous";
        default: throw std::runtime_error("Unknown side");
    }
}

constexpr bool same_side(const Side side_0, const Side side_1) {
    return side_0 != Side::ambiguous &&
           side_1 != Side::ambiguous &&
           side_0 == side_1;
}

constexpr bool diff_side(const Side side_0, const Side side_1) {
    return side_0 != Side::ambiguous &&
           side_1 != Side::ambiguous &&
           side_0 != side_1;
}

template<IntervalType Interval>
struct Edge {
private:
    Vector<Interval> from_;
    Vector<Interval> to_;

public:
    explicit Edge(const Vector<Interval>& from, const Vector<Interval>& to) : from_(from), to_(to) {
        if(!from_.dist(to_).is_positive()) {
            throw std::runtime_error("Zero length edge found");
        }
    }

    ~Edge() = default;

    Edge(const Edge& edge) = default;

    Edge(Edge&& edge) = default;

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
        return to_.dist(from_);
    }

    Vector<Interval> mid() const {
        return (from_ + to_) / Interval(2);
    }

    Side side(const Vector<Interval>& vector) const {
        const Interval cross = dir().cross(vector - from_);
        if(cross.is_positive()) {
            return Side::left;
        }
        if(cross.is_negative()) {
            return Side::right;
        }
        return Side::ambiguous;
    }

    bool intersects(const Edge& edge) const {
        return diff_side(side(edge.from_), side(edge.to_)) && diff_side(edge.side(from_), edge.side(to_));
    }

    bool avoids(const Vector<Interval>& vector) const {
        return side(vector) != Side::ambiguous || vector.dist(mid()) > len() / Interval(2);
    }

    bool avoids(const Edge& edge) const {
        return edge.mid().dist(mid()) > (len() + edge.len()) / Interval(2) ||
               same_side(side(edge.from_), side(edge.to_)) ||
               same_side(edge.side(from_), edge.side(to_));
    }

    friend std::ostream& operator<<(std::ostream& os, const Edge& edge) {
        return os << edge.from_ << " -> " << edge.to_;
    }
};
