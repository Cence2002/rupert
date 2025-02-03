#pragma once

#include "geometry/vector_2.hpp"

template<IntervalType I>
struct Line {
private:
    Vector2<I> from_;
    Vector2<I> to_;

public:
    explicit Line(const Vector2<I>& from, const Vector2<I>& to) : from_(from), to_(to) {}

    explicit Line(const Vector2<I>& to) : from_(Vector2<I>(0, 0)), to_(to) {}

    Orientation orientation(const Vector2<I>& v) const {
        return (to_ - from_).orientation(v - from_);
    }

    bool opposite_side(const Line& l) const {
        const Orientation orientation_from = orientation(l.from_);
        const Orientation orientation_to = orientation(l.to_);
        if(orientation_from == Orientation::COLLINEAR || orientation_to == Orientation::COLLINEAR) {
            return false;
        }
        return orientation_from != orientation_to;
    }

    bool same_side(const Line& l) const {
        const Orientation orientation_from = orientation(l.from_);
        const Orientation orientation_to = orientation(l.to_);
        if(orientation_from == Orientation::COLLINEAR || orientation_to == Orientation::COLLINEAR) {
            return false;
        }
        return orientation_from == orientation_to;
    }

    bool intersects(const Line& l) const {
        return opposite_side(l) && l.opposite_side(*this);
    }

    bool avoids(const Line& l) const {
        return same_side(l) || l.same_side(*this);
    }

    friend std::ostream& operator<<(std::ostream& os, const Line& line) {
        return os << line.from_ << " -> " << line.to_;
    }
};
