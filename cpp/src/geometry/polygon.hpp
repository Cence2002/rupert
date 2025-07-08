#pragma once

#include "geometry/edge.hpp"
#include <vector>

template<IntervalType Interval>
struct Polygon {
private:
    std::vector<Edge<Interval>> edges_;

public:
    explicit Polygon(const std::vector<Edge<Interval>>& edges) : edges_(edges) {}

    ~Polygon() = default;

    Polygon(const Polygon& polygon) = default;

    Polygon(Polygon&& polygon) = default;

    Polygon& operator=(const Polygon&) = delete;

    Polygon& operator=(Polygon&&) = delete;

    const std::vector<Edge<Interval>>& edges() const {
        return edges_;
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Polygon& polygon) {
        ostream << "[";
        bool first = true;
        for(const Edge<Interval>& edge: polygon.edges_) {
            if(!first) {
                ostream << ",";
            } else {
                first = false;
            }
            ostream << edge;
        }
        ostream << "]";
        return ostream;
    }
};
