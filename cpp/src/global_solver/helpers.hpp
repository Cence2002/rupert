#pragma once

#include "geometry/geometry.hpp"
#include "range/ranges.hpp"
#include <vector>
#include <optional>
#include <algorithm>
#include <queue>

struct TerminalBox {
private:
    Range3 range3_;
    std::vector<Range2> range2s_;

public:
    explicit TerminalBox(const Range3& range3, const std::vector<Range2>& range2s) : range3_(range3), range2s_(range2s) {}

    const Range3& range3() const {
        return range3_;
    }

    const std::vector<Range2>& range2s() const {
        return range2s_;
    }
};

template<IntervalType Interval>
Interval harmonic_trivial(const Interval& cos_amplitude, const Interval& sin_amplitude, const Interval& angle) {
    return cos_amplitude * angle.cos() + sin_amplitude * angle.sin();
}

template<IntervalType Interval>
Interval harmonic_combined(const Interval& cos_amplitude, const Interval& sin_amplitude, const Interval& angle) {
    if(cos_amplitude.nonz()) {
        const Interval amplitude = (cos_amplitude.sqr() + sin_amplitude.sqr()).sqrt();
        const Interval phase = (sin_amplitude / cos_amplitude).atan();
        const int sign = cos_amplitude.pos() ? 1 : -1;
        return Interval(sign) * amplitude * (angle - phase).cos();
    }
    if(sin_amplitude.nonz()) {
        const Interval amplitude = (cos_amplitude.sqr() + sin_amplitude.sqr()).sqrt();
        const Interval phase = -(cos_amplitude / sin_amplitude).atan();
        const int sign = sin_amplitude.pos() ? 1 : -1;
        return Interval(sign) * amplitude * (angle - phase).sin();
    }
    return harmonic_trivial(cos_amplitude, sin_amplitude, angle);
}

// (X, Y) = R(alpha) * (x, y)
// X = x * cos(alpha) - y * sin(alpha)
// Y = y * cos(alpha) + x * sin(alpha)

template<IntervalType Interval>
Vector2<Interval> rotation_trivial(const Vector2<Interval>& vector, const Interval& alpha) {
    return Vector2<Interval>(
        harmonic_trivial(vector.x(), -vector.y(), alpha),
        harmonic_trivial(vector.y(), vector.x(), alpha)
    );
}

template<IntervalType Interval>
Vector2<Interval> rotation_combined(const Vector2<Interval>& vector, const Interval& alpha) {
    return Vector2<Interval>(
        harmonic_combined(vector.x(), -vector.y(), alpha),
        harmonic_combined(vector.y(), vector.x(), alpha)
    );
}

// (X, Y, _) = Rx(phi) * Rz(theta) * (x, y, z)
// X = x * cos(theta) - y * sin(theta)
// Y = (y * cos(theta) + x * sin(theta)) * cos(phi) - z * sin(phi)

template<IntervalType Interval>
Vector2<Interval> projection_trivial(const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    return Vector2<Interval>(
        harmonic_trivial(vertex.x(), -vertex.y(), theta),
        harmonic_trivial(harmonic_trivial(vertex.y(), vertex.x(), theta), -vertex.z(), phi)
    );
}

template<IntervalType Interval>
Vector2<Interval> projection_combined(const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    return Vector2<Interval>(
        harmonic_combined(vertex.x(), -vertex.y(), theta),
        harmonic_combined(harmonic_combined(vertex.y(), vertex.x(), theta), -vertex.z(), phi)
    );
}

template<IntervalType Interval>
std::vector<Vector2<Interval>> vector_hull(const Vector2<Interval>& vector2) {
    return std::vector<Vector2<Interval>>{
        Vector2<Interval>(Interval(vector2.x().min()), Interval(vector2.y().min())),
        Vector2<Interval>(Interval(vector2.x().max()), Interval(vector2.y().min())),
        Vector2<Interval>(Interval(vector2.x().max()), Interval(vector2.y().max())),
        Vector2<Interval>(Interval(vector2.x().min()), Interval(vector2.y().max())),
    };
}

template<IntervalType Interval>
std::vector<Vector2<Interval>> rotation_hull_trivial(const Vector2<Interval>& projected_vertex, const Interval& alpha) {
    return vector_hull(rotation_trivial(projected_vertex, alpha));
}

template<IntervalType Interval>
std::vector<Vector2<Interval>> rotation_hull_combined(const Vector2<Interval>& projected_vertex, const Interval& alpha) {
    return vector_hull(rotation_combined(projected_vertex, alpha));
}

template<IntervalType Interval>
std::vector<Vector2<Interval>> rotation_hull_polygon(const Vector2<Interval>& vector2, const Interval& alpha, const int resolution) {
    if(resolution < 1) {
        throw std::invalid_argument("Resolution must be at least 1");
    }
    if(Interval(alpha.len()) > Interval::pi() / Interval(2) * Interval(resolution)) {
        return rotation_hull_combined(vector2, alpha);
    }
    std::vector<Vector2<Interval>> rotated_vectors;
    rotated_vectors.emplace_back(rotation_trivial(vector2, Interval(alpha.min())));
    const Interval scaling_factor = (Interval(alpha.rad()) / Interval(resolution)).cos().inv();
    const int pieces = 2 * resolution;
    for(int i = 1; i < pieces; i += 2) {
        const Interval alpha_i = Interval(alpha.min()) * Interval(pieces - i) / Interval(pieces) + Interval(alpha.max()) * Interval(i) / Interval(pieces);
        const Vector2<Interval> rotated_vertex = rotation_trivial(vector2, alpha_i);
        const Vector2<Interval> scaled_rotated_vertex = rotated_vertex * scaling_factor;
        rotated_vectors.emplace_back(scaled_rotated_vertex);
    }
    rotated_vectors.emplace_back(rotation_trivial(vector2, Interval(alpha.max())));
    return rotated_vectors;
}

template<IntervalType Interval>
std::vector<Vector2<Interval>> projection_hull_trivial(const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    return vector_hull(projection_trivial(vertex, theta, phi));
}

template<IntervalType Interval>
std::vector<Vector2<Interval>> projection_hull_combined(const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    return vector_hull(projection_combined(vertex, theta, phi));
}

template<IntervalType Interval>
std::vector<Vector2<Interval>> projection_hull_polygon(const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi, const int resolution) {
    if(Interval(theta.len()) > Interval::pi() / Interval(2) * Interval(resolution)) {
        return projection_hull_combined(vertex, theta, phi);
    }
    const Vector2<Interval> vector = Vector2<Interval>(vertex.x(), vertex.y());
    const std::vector<Vector2<Interval>> rotation_hull = rotation_hull_polygon(vector, theta, resolution);
    std::vector<Vector2<Interval>> projected_vertices;
    for(const Vector2<Interval>& rotated_vertex: rotation_hull) {
        const Interval harmonic = harmonic_combined(rotated_vertex.y(), -vertex.z(), phi);
        projected_vertices.emplace_back(rotated_vertex.x(), Interval(harmonic.min()));
        projected_vertices.emplace_back(rotated_vertex.x(), Interval(harmonic.max()));
    }
    return projected_vertices;
}

template<typename Interval>
Interval max_uncertainty(const std::vector<Vector2<Interval>>& vector2s) {
    size_t max_index = 0;
    for(size_t i = 1; i < vector2s.size(); i++) {
        if(vector2s[i].len().len() > vector2s[max_index].len().len()) {
            max_index = i;
        }
    }
    return vector2s[max_index].len().len();
}

// template<typename Interval>
// std::vector<Vector<Interval>> merge_vectors(const std::vector<Vector<Interval>>& vectors) {
//     const Interval epsilon = Interval(max_uncertainty(vectors)) * Interval(static_cast<int>(vectors.size())) * Interval(2);
//     ...
// }

template<IntervalType Interval>
Polygon<Interval> convex_hull(const std::vector<Vector2<Interval>>& vector2s) {
    std::vector<Edge<Interval>> edges;

    std::vector<bool> is_duplicate(vector2s.size(), false); // TODO: replace ignoring duplicates with merging them
    bool any_non_duplicate = false;
    for(size_t i = 1; i < vector2s.size(); i++) {
        for(size_t j = 0; j < i; j++) {
            if(!vector2s[i].diff(vector2s[j])) {
                is_duplicate[i] = true;
                break;
            }
        }
        if(!is_duplicate[i]) {
            any_non_duplicate = true;
        }
    }
    if(!any_non_duplicate) {
        throw std::runtime_error("All vectors are duplicates");
    }

    std::queue<size_t> queue;
    std::vector<bool> visited_indices(vector2s.size(), false);

    size_t start_index = 0;
    for(size_t new_start_index = 1; new_start_index < vector2s.size(); new_start_index++) {
        if(is_duplicate[new_start_index]) {
            continue;
        }
        if(vector2s[new_start_index].len().max() > vector2s[start_index].len().max()) {
            start_index = new_start_index;
        }
    }
    queue.emplace(start_index);

    while(!queue.empty()) {
        const size_t from_index = queue.front();
        queue.pop();
        if(visited_indices[from_index]) {
            continue;
        }
        visited_indices[from_index] = true;

        std::optional<size_t> most_clockwise_index;
        for(size_t new_most_clockwise_index = 0; new_most_clockwise_index < vector2s.size(); new_most_clockwise_index++) {
            if(is_duplicate[new_most_clockwise_index] || new_most_clockwise_index == from_index || Edge<Interval>(vector2s[from_index]).side(vector2s[new_most_clockwise_index]) == Side::right) {
                continue;
            }
            if(!vector2s[from_index].diff(vector2s[new_most_clockwise_index])) {
                throw std::runtime_error("Zero length edge found");
            }
            if(!most_clockwise_index.has_value() || Edge<Interval>(vector2s[from_index], vector2s[most_clockwise_index.value()]).side(vector2s[new_most_clockwise_index]) == Side::right) {
                most_clockwise_index = new_most_clockwise_index;
            }
        }
        if(!most_clockwise_index.has_value()) {
            throw std::runtime_error("No most clockwise vertex found");
        }

        std::vector<size_t> to_indices;
        const Edge<Interval> most_clockwise_edge(vector2s[from_index], vector2s[most_clockwise_index.value()]);
        for(size_t to_index = 0; to_index < vector2s.size(); to_index++) {
            if(is_duplicate[to_index] || to_index == from_index || Edge<Interval>(vector2s[from_index]).side(vector2s[to_index]) == Side::right) {
                continue;
            }
            if(!vector2s[from_index].diff(vector2s[to_index])) {
                throw std::runtime_error("Zero length edge found");
            }
            if(most_clockwise_edge.side(vector2s[to_index]) == Side::left) {
                continue;
            }
            bool is_most_clockwise = true;
            const Edge<Interval> edge(vector2s[from_index], vector2s[to_index]);
            for(size_t index = 0; index < vector2s.size(); index++) {
                if(index == from_index || index == to_index) {
                    continue;
                }
                if(edge.side(vector2s[index]) == Side::right) {
                    is_most_clockwise = false;
                    break;
                }
            }
            if(is_most_clockwise) {
                to_indices.push_back(to_index);
            }
        }
        std::ranges::stable_sort(to_indices, [&](const size_t index, const size_t other_index) {
            const Interval dist = vector2s[from_index].dist(vector2s[other_index]).max();
            const Interval other_dist = vector2s[from_index].dist(vector2s[index]).max();
            return dist < other_dist;
        });
        std::vector<std::pair<size_t, size_t>> new_edge_indices;
        for(const size_t to_index: to_indices) {
            const bool is_most_clockwise = std::ranges::none_of(new_edge_indices, [&](const std::pair<size_t, size_t>& new_edge_index) {
                const size_t from_edge_index = new_edge_index.first;
                const size_t to_edge_index = new_edge_index.second;
                const Edge<Interval> new_edge(vector2s[from_edge_index], vector2s[to_edge_index]);
                return new_edge.side(vector2s[to_index]) == Side::left;
            });
            if(!is_most_clockwise) {
                continue;
            }
            new_edge_indices.emplace_back(from_index, to_index);
            if(!visited_indices[to_index]) {
                queue.emplace(to_index);
            }
        }
        for(const auto& [from_edge_index, to_edge_index]: new_edge_indices) {
            edges.emplace_back(vector2s[from_edge_index], vector2s[to_edge_index]);
        }
    }
    return Polygon(edges);
}

template<IntervalType Interval>
bool is_vector_inside_polygon(const Polygon<Interval>& polygon, const Vector2<Interval>& vector2) {
    if(std::ranges::any_of(polygon.edges(), [&](const Edge<Interval>& edge) {
        return !edge.avoids(vector2);
    })) {
        return false;
    }
    if(std::ranges::any_of(polygon.edges(), [&](const Edge<Interval>& edge) {
        return edge.side(vector2) == Side::right;
    })) {
        return false;
    }
    return true;
}

template<IntervalType Interval>
bool is_projected_vertex_inside_polygon_trivial(const Polygon<Interval>& polygon, const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    return is_vector_inside_polygon(polygon, projection_trivial(vertex, theta, phi));
}

template<IntervalType Interval>
bool is_projected_vertex_inside_polygon_combined(const Polygon<Interval>& polygon, const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    return is_vector_inside_polygon(polygon, projection_combined(vertex, theta, phi));
}

template<IntervalType Interval>
bool is_vector_outside_polygon(const Polygon<Interval>& polygon, const Vector2<Interval>& vector2) {
    if(std::ranges::any_of(polygon.edges(), [&](const Edge<Interval>& edge) {
        return !edge.avoids(vector2);
    })) {
        return false;
    }
    if(std::ranges::any_of(polygon.edges(), [&](const Edge<Interval>& edge) {
        return edge.side(vector2) == Side::right;
    })) {
        return true;
    }
    return false;
}

template<IntervalType Interval>
bool is_projected_vertex_outside_polygon_trivial(const Polygon<Interval>& polygon, const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    return is_vector_outside_polygon(polygon, projection_trivial(vertex, theta, phi));
}

template<IntervalType Interval>
bool is_projected_vertex_outside_polygon_combined(const Polygon<Interval>& polygon, const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    return is_vector_outside_polygon(polygon, projection_combined(vertex, theta, phi));
}

template<IntervalType Interval>
bool is_projected_vertex_avoiding_polygon_advanced_fixed_theta(const Polygon<Interval>& polygon, const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    const Vector2<Interval> projected_vertex = projection_combined(vertex, theta, phi);
    const Edge projected_edge(
        Vector2<Interval>(projected_vertex.x(), projected_vertex.y().min()),
        Vector2<Interval>(projected_vertex.x(), projected_vertex.y().max())
    );
    return std::ranges::all_of(polygon.edges(), [&](const Edge<Interval>& edge) {
        return projected_edge.avoids(edge);
    });
}

template<IntervalType Interval>
bool is_projected_vertex_avoiding_edge_advanced_fixed_phi(const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi, const Edge<Interval>& edge) {
    const Interval translation_factor = vertex.z() * phi.sin();
    const Interval scaling_factor = phi.cos();
    const Vector2<Interval> transformed_edge_from(
        edge.from().x(),
        (edge.from().y() + translation_factor) / scaling_factor
    );
    const Vector2<Interval> transformed_edge_to(
        edge.to().x(),
        (edge.to().y() + translation_factor) / scaling_factor
    );
    const Edge<Interval> transformed_edge(transformed_edge_from, transformed_edge_to);

    const Interval radius_squared = vertex.x().sqr() + vertex.y().sqr();
    const Interval quadratic_term = transformed_edge.len().sqr();
    const Interval linear_term = Interval(2) * transformed_edge.dir().dot(transformed_edge.from());
    const Interval constant_term = transformed_edge.from().len().sqr() - radius_squared;
    const Interval discriminant = linear_term.sqr() - Interval(4) * quadratic_term * constant_term;
    if(!discriminant.pos()) {
        return true;
    }
    const Interval sqrt_discriminant = discriminant.sqrt();
    const std::array<Interval, 2> solutions = {
        (-linear_term + sqrt_discriminant) / (Interval(2) * quadratic_term),
        (-linear_term - sqrt_discriminant) / (Interval(2) * quadratic_term)
    };

    const Vector2<Interval> min_projected_vertex = projection_trivial(vertex, theta.min(), phi);
    const Vector2<Interval> max_projected_vertex = projection_trivial(vertex, theta.max(), phi);
    const Vector2<Interval> transformed_min_projected_vertex = Vector2<Interval>(
        min_projected_vertex.x(),
        (min_projected_vertex.y() + translation_factor) / scaling_factor
    );
    const Vector2<Interval> transformed_max_projected_vertex = Vector2<Interval>(
        max_projected_vertex.x(),
        (max_projected_vertex.y() + translation_factor) / scaling_factor
    );
    const Edge<Interval> transformed_projected_vertex_edge(transformed_min_projected_vertex, transformed_max_projected_vertex);
    for(const Interval& solution: solutions) {
        if(solution.neg() || solution > transformed_edge.len()) {
            continue;
        }
        const Vector2<Interval> intersection = transformed_edge.from() + transformed_edge.dir() * transformed_edge.len() * solution;
        if(!transformed_projected_vertex_edge.avoids(Edge<Interval>(Vector2<Interval>(Interval(0), Interval(0)), intersection))) {
            return false;
        }
    }
    return true;
}

template<IntervalType Interval>
bool is_projected_vertex_avoiding_polygon_advanced_fixed_phi(const Polygon<Interval>& polygon, const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    if(!phi.cos().nonz()) {
        return is_projected_vertex_outside_polygon_combined(polygon, vertex, theta, phi);
    }
    return std::ranges::all_of(polygon.edges(), [&](const Edge<Interval>& edge) {
        return is_projected_vertex_avoiding_edge_advanced_fixed_phi(vertex, theta, phi, edge);
    });
}

template<IntervalType Interval>
bool is_projected_vertex_outside_polygon_advanced(const Polygon<Interval>& polygon, const Vector3<Interval>& vertex, const Interval& theta, const Interval& phi) {
    if(!(theta.len() < Interval::pi() / Interval(2))) {
        return is_projected_vertex_outside_polygon_combined(polygon, vertex, theta, phi);
    }
    return is_projected_vertex_outside_polygon_combined(polygon, vertex, theta.min(), phi.min()) &&
           is_projected_vertex_outside_polygon_combined(polygon, vertex, theta.max(), phi.max()) &&
           is_projected_vertex_outside_polygon_combined(polygon, vertex, theta.min(), phi.max()) &&
           is_projected_vertex_outside_polygon_combined(polygon, vertex, theta.max(), phi.min()) &&
           is_projected_vertex_avoiding_polygon_advanced_fixed_theta(polygon, vertex, theta.min(), phi) &&
           is_projected_vertex_avoiding_polygon_advanced_fixed_theta(polygon, vertex, theta.max(), phi) &&
           is_projected_vertex_avoiding_polygon_advanced_fixed_phi(polygon, vertex, theta, phi.min()) &&
           is_projected_vertex_avoiding_polygon_advanced_fixed_phi(polygon, vertex, theta, phi.max());
}
