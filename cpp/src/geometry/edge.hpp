#pragma once

#include "geometry/vector2.hpp"

enum struct Side {
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

constexpr bool same_side(const Side side, const Side other_side) {
    return side != Side::ambiguous &&
           other_side != Side::ambiguous &&
           side == other_side;
}

constexpr bool diff_side(const Side side, const Side other_side) {
    return side != Side::ambiguous &&
           other_side != Side::ambiguous &&
           side != other_side;
}

template<IntervalType Interval>
struct Edge {
private:
    Vector2<Interval> from_;
    Vector2<Interval> to_;

public:
    explicit Edge(const Vector2<Interval>& to) : from_(Vector2<Interval>(Interval(0), Interval(0))), to_(to) {}

    explicit Edge(const Vector2<Interval>& from, const Vector2<Interval>& to) : from_(from), to_(to) {
        if(!from_.dist(to_).pos()) {
            throw std::runtime_error("Zero length edge");
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

    Side side(const Vector2<Interval>& vector) const {
        const Interval cross = dir().cross(vector - from_);
        if(cross.pos()) {
            return Side::left;
        }
        if(cross.neg()) {
            return Side::right;
        }
        return Side::ambiguous;
    }

    bool intersects(const Edge& edge) const {
        return diff_side(side(edge.from_), side(edge.to_)) && diff_side(edge.side(from_), edge.side(to_));
    }

    bool avoids(const Vector2<Interval>& vector) const {
        return side(vector) != Side::ambiguous ||
               mid().dist(vector) > len() / Interval(2);
    }

    bool avoids(const Edge& edge) const {
        return same_side(side(edge.from_), side(edge.to_)) ||
               same_side(edge.side(from_), edge.side(to_)) ||
               mid().dist(edge.mid()) > (len() + edge.len()) / Interval(2);
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Edge& edge) {
        return ostream << edge.from_ << "->" << edge.to_;
    }
};
