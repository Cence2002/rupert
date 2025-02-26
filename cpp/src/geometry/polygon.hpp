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

    friend std::ostream& operator<<(std::ostream& ostream, const Polygon& polygon) {
        for(const Edge<Interval>& edge: polygon.edges_) {
            ostream << edge << "\n";
        }
        return ostream;
    }
};
