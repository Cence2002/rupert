#pragma once

#include "geometry/vector2.hpp"

enum class Side {
    left,
    right,
    ambiguous
};

inline std::ostream& operator<<(std::ostream& ostream, const Side& side) {
    switch(side) {
        case Side::left: return ostream << "left";
        case Side::right: return ostream << "right";
        case Side::ambiguous: return ostream << "ambiguous";
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
    Vector2<Interval> from_;
    Vector2<Interval> to_;

public:
    explicit Edge(const Vector2<Interval>& from, const Vector2<Interval>& to) : from_(from), to_(to) {
        if(!from_.dist(to_).is_positive()) {
            throw std::runtime_error("Zero length edge found");
        }
    }

    explicit Edge(const Vector2<Interval>& to) : from_(Vector2<Interval>(Interval(0), Interval(0))), to_(to) {
        if(!to_.len().is_positive()) {
            throw std::runtime_error("Zero length edge found");
        }
    }

    ~Edge() = default;

    Edge(const Edge& edge) = default;

    Edge(Edge&& edge) = default;

    Edge& operator=(const Edge&) = delete;

    Edge& operator=(Edge&&) = delete;

    const Vector2<Interval>& from() const {
        return from_;
    }

    const Vector2<Interval>& to() const {
        return to_;
    }

    Vector2<Interval> dir() const {
        return (to_ - from_).unit();
    }

    Interval len() const {
        return to_.dist(from_);
    }

    Vector2<Interval> mid() const {
        return (from_ + to_) / Interval(2);
    }

    Side side(const Vector2<Interval>& vector2) const {
        const Interval cross = dir().cross(vector2 - from_);
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

    bool avoids(const Vector2<Interval>& vector2) const {
        return side(vector2) != Side::ambiguous || vector2.dist(mid()) > len() / Interval(2);
    }

    bool avoids(const Edge& edge) const {
        return edge.mid().dist(mid()) > (len() + edge.len()) / Interval(2) ||
               same_side(side(edge.from_), side(edge.to_)) ||
               same_side(edge.side(from_), edge.side(to_));
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Edge& edge) {
        return ostream << edge.from_ << "->" << edge.to_;
    }
};
