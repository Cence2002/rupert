#pragma once

#include "geometry/util.hpp"
#include "geometry/line.hpp"
#include <vector>

template<IntervalType I>
struct Polygon {
private:
    std::vector<Line<I>> lines_;

public:
    explicit Polygon(const std::vector<Line<I>>& lines) : lines_(lines) {}

    std::vector<Line<I>> lines() const {
        return lines_;
    }

    bool is_inside(const Vector2<I>& vector2) const {
        bool all_counter_clockwise = true;
        for(const Line<I>& line: lines_) {
            const Orientation orientation = line.orientation(vector2);
            if(orientation == Orientation::COLLINEAR) {
                return false;
            }
            const bool counter_clockwise = orientation == Orientation::COUNTERCLOCKWISE;
            all_counter_clockwise &= counter_clockwise;
        }
        return all_counter_clockwise;
    }

    bool is_outside(const Vector2<I>& vector2) const {
        bool any_clockwise = false;
        for(const Line<I>& line: lines_) {
            const Orientation orientation = line.orientation(vector2);
            if(!line.avoids(vector2)) {
                return false;
            }
            const bool clockwise = orientation == Orientation::CLOCKWISE;
            any_clockwise |= clockwise;
        }
        return any_clockwise;
    }

    friend std::ostream& operator<<(std::ostream& os, const Polygon& polygon) {
        for(const Line<I>& line: polygon.lines_) {
            os << line << "\n";
        }
        return os;
    }
};
