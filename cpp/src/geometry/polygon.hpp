#pragma once

#include "geometry/edge.hpp"
#include <vector>

template<IntervalType Interval>
struct Polygon {
private:
    std::vector<Edge<Interval>> edges_;

public:
    explicit Polygon(const std::vector<Edge<Interval>>& edges) : edges_(edges) {}

    std::vector<Edge<Interval>> edges() const {
        return edges_;
    }

    bool is_inside(const IntervalVector2<Interval>& interval_vector2) const {
        bool all_counter_clockwise = true;
        for(const Edge<Interval>& edge: edges_) {
            const Orientation orientation = edge.orientation(interval_vector2);
            if(orientation == Orientation::COLLINEAR) {
                return false;
            }
            const bool counter_clockwise = orientation == Orientation::COUNTERCLOCKWISE;
            all_counter_clockwise &= counter_clockwise;
        }
        return all_counter_clockwise;
    }

    bool is_outside(const IntervalVector2<Interval>& interval_vector2) const {
        bool any_clockwise = false;
        for(const Edge<Interval>& edge: edges_) {
            const Orientation orientation = edge.orientation(interval_vector2);
            if(!edge.avoids(interval_vector2)) {
                return false;
            }
            const bool clockwise = orientation == Orientation::CLOCKWISE;
            any_clockwise |= clockwise;
        }
        return any_clockwise;
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Polygon& polygon) {
        for(const Edge<Interval>& edge: polygon.edges_) {
            ostream << edge << "\n";
        }
        return ostream;
    }
};
