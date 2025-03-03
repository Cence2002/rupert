#pragma once

#include "geometry/util.hpp"
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

    bool is_vector2_inside_polygon(const Vector2<Interval>& vector2) const {
        bool all_counter_clockwise = true;
        for(const Edge<Interval>& edge: edges()) {
            const Orientation orientation = edge.orientation(vector2);
            if(orientation == Orientation::COLLINEAR) {
                return false;
            }
            const bool counter_clockwise = orientation == Orientation::COUNTERCLOCKWISE;
            all_counter_clockwise &= counter_clockwise;
        }
        return all_counter_clockwise;
    }

    bool is_projected_vertex_inside_polygon_trivial(const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) const {
        const Vector2<Interval> projected_vertex = projection_trivial(vertex, theta, phi);
        return is_vector2_inside_polygon(projected_vertex);
    }

    bool is_projected_vertex_inside_polygon_combined(const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) const {
        const Vector2<Interval> projected_vertex = projection_combined(vertex, theta, phi);
        return is_vector2_inside_polygon(projected_vertex);
    }

    bool is_vector2_outside_polygon(const Vector2<Interval>& vector2) const {
        bool any_clockwise = false;
        for(const Edge<Interval>& edge: edges()) {
            const Orientation orientation = edge.orientation(vector2);
            if(!edge.avoids(vector2)) {
                return false;
            }
            const bool clockwise = orientation == Orientation::CLOCKWISE;
            any_clockwise |= clockwise;
        }
        return any_clockwise;
    }

    bool is_projected_vertex_outside_polygon_trivial(const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) const {
        const Vector2<Interval> projected_vertex = projection_trivial(vertex, theta, phi);
        return is_vector2_outside_polygon(projected_vertex);
    }

    bool is_projected_vertex_outside_polygon_combined(const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) const {
        const Vector2<Interval> projected_vertex = projection_combined(vertex, theta, phi);
        return is_vector2_outside_polygon(projected_vertex);
    }

    bool is_projected_vertex_avoiding_polygon_advanced_fixed_theta(const Vector3<Interval>& vertex, const typename Interval::Number& theta, const Interval& phi) const {
        const Vector2<Interval> projected_vertex = projection_combined(vertex, Interval(theta), phi);
        const Edge projected_edge(
            Vector2<Interval>(projected_vertex.x(), Interval(projected_vertex.y().min())),
            Vector2<Interval>(projected_vertex.x(), Interval(projected_vertex.y().max()))
        );
        for(const Edge<Interval>& edge: edges()) {
            if(!projected_edge.avoids(edge)) {
                return false;
            }
        }
        return true;
    }

    bool is_projected_vertex_avoiding_edge_advanced_fixed_phi(const Vector3<Interval>& vertex, const Interval& theta, const typename Interval::Number& phi, const Edge<Interval>& edge) const {
        const Interval transformation_addition = vertex.z() * Interval(phi).sin();
        const Interval transformation_division = Interval(phi).cos();
        const Edge<Interval> transformed_edge(
            Vector2<Interval>(
                edge.from().x(),
                (edge.from().y() + transformation_addition) / transformation_division
            ),
            Vector2<Interval>(
                edge.to().x(),
                (edge.to().y() + transformation_addition) / transformation_division
            )
        );
        const Vector2<Interval> transformed_edge_direction = transformed_edge.direction();

        const Interval radius_squared = vertex.x() * vertex.x() + vertex.y() * vertex.y();
        const Interval double_quadratic_term = 2 * transformed_edge_direction.len_sqr();
        const Interval linear_term = 2 * Vector2<Interval>::dot(transformed_edge_direction, transformed_edge.from());
        const Interval constant_term = transformed_edge.from().len_sqr() - radius_squared;
        const Interval discriminant = linear_term.sqr() - 2 * double_quadratic_term * constant_term;
        if(!discriminant.is_positive()) {
            return true;
        }
        const Interval sqrt_discriminant = discriminant.sqrt();
        std::array<Interval, 2> solutions = {
            (-linear_term + sqrt_discriminant) / double_quadratic_term,
            (-linear_term - sqrt_discriminant) / double_quadratic_term
        };

        const Vector2<Interval> min_projected_vertex = projection_trivial(vertex, Interval(theta.min()), Interval(phi));
        const Vector2<Interval> max_projected_vertex = projection_trivial(vertex, Interval(theta.max()), Interval(phi));
        const Vector2<Interval> transformed_max_projected_vertex = Vector2<Interval>(
            max_projected_vertex.x(),
            (max_projected_vertex.y() + transformation_addition) / transformation_division
        );
        const Edge<Interval> projected_vertex_edge(
            Vector2<Interval>(
                min_projected_vertex.x(),
                (min_projected_vertex.y() + transformation_addition) / transformation_division
            ),
            Vector2<Interval>(
                max_projected_vertex.x(),
                (max_projected_vertex.y() + transformation_addition) / transformation_division
            )
        );
        for(const Interval& solution: solutions) {
            if(solution.is_negative() || solution > 1) {
                continue;
            }
            const Vector2<Interval> intersection(
                transformed_edge.from().x() + solution * transformed_edge_direction.x(),
                transformed_edge.from().y() + solution * transformed_edge_direction.y()
            );
            if(!projected_vertex_edge.avoids(Edge<Interval>(Vector2<Interval>(Interval(0), Interval(0)), intersection))) {
                return false;
            }
        }
        return true;
    }

    bool is_projected_vertex_avoiding_polygon_advanced_fixed_phi(const Vector3<Interval>& vertex, const Interval& theta, const typename Interval::Number& phi) const {
        if(!Interval(phi).cos().is_nonzero()) {
            // degenerate case, division by zero would occur in the inverse transformation
            // default to combined
            return is_projected_vertex_outside_polygon_combined(vertex, theta, Interval(phi));
        }
        for(const Edge<Interval>& edge: edges()) {
            if(!is_projected_vertex_avoiding_edge_advanced_fixed_phi(vertex, theta, phi, edge)) {
                return false;
            }
        }
        return true;
    }

    bool is_projected_vertex_outside_polygon_advanced(const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) const {
        if(!(theta.len() < Interval::pi() / 3)) {
            // projected vertex might surround the polygon, meaning it avoids, but is not outside
            // default to combined
            return is_projected_vertex_outside_polygon_combined(vertex, theta, phi);
        }
        return is_projected_vertex_outside_polygon_combined(vertex, Interval(theta.mid()), Interval(phi.mid())) &&
               is_projected_vertex_avoiding_polygon_advanced_fixed_theta(vertex, theta.min(), phi) &&
               is_projected_vertex_avoiding_polygon_advanced_fixed_theta(vertex, theta.max(), phi) &&
               is_projected_vertex_avoiding_polygon_advanced_fixed_phi(vertex, theta, phi.min()) &&
               is_projected_vertex_avoiding_polygon_advanced_fixed_phi(vertex, theta, phi.max());
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Polygon& polygon) {
        for(const Edge<Interval>& edge: polygon.edges_) {
            ostream << edge << "\n";
        }
        return ostream;
    }
};
