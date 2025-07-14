#pragma once

#include "geometry/edge.hpp"
#include <vector>
#include <boost/algorithm/string/join.hpp>

template<IntervalType Interval>
class Polygon {
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

    bool avoids_edges(const Vector2<Interval>& vector) const {
        return std::ranges::all_of(edges_, [&](const Edge<Interval>& edge) {
            return edge.avoids(vector);
        });
    }

    bool inside(const Vector2<Interval>& vector) const {
        return avoids_edges(vector) && std::ranges::all_of(edges_, [&](const Edge<Interval>& edge) {
            return edge.side(vector) != Side::right;
        });
    }

    bool outside(const Vector2<Interval>& vector) const {
        return avoids_edges(vector) && std::ranges::any_of(edges_, [&](const Edge<Interval>& edge) {
            return edge.side(vector) == Side::right;
        });
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Polygon& polygon) {
        std::vector<std::string> edges_strings;
        for(const Edge<Interval>& edge: polygon.edges_) {
            std::stringstream stream;
            stream << edge;
            edges_strings.push_back(stream.str());
        }
        const std::string edges_string = boost::algorithm::join(edges_strings, ", ");

        return ostream << "[" << edges_string << "]";
    }
};
