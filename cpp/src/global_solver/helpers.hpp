#pragma once

#include "geometry/geometry.hpp"
#include "range/ranges.hpp"
#include <vector>
#include <optional>
#include <algorithm>
#include <queue>

template<IntervalType Interval>
Interval trivial_harmonic(const Interval& cos_amplitude, const Interval& sin_amplitude, const Interval& angle) {
    return cos_amplitude * angle.cos() + sin_amplitude * angle.sin();
}

template<IntervalType Interval>
Interval combined_harmonic(const Interval& cos_amplitude, const Interval& sin_amplitude, const Interval& angle) {
    if(cos_amplitude.nonz()) {
        const Interval amplitude = (cos_amplitude.sqr() + sin_amplitude.sqr()).sqrt();
        const Interval phase = (sin_amplitude / cos_amplitude).atan();
        const Interval abs_harmonic = amplitude * (angle - phase).cos();
        return cos_amplitude.pos() ? abs_harmonic : -abs_harmonic;
    }
    if(sin_amplitude.nonz()) {
        const Interval amplitude = (cos_amplitude.sqr() + sin_amplitude.sqr()).sqrt();
        const Interval phase = -(cos_amplitude / sin_amplitude).atan();
        const Interval abs_harmonic = amplitude * (angle - phase).sin();
        return sin_amplitude.pos() ? abs_harmonic : -abs_harmonic;
    }
    return trivial_harmonic(cos_amplitude, sin_amplitude, angle);
}

// (X, Y) = R(alpha) * (x, y)
// X = x * cos(alpha) - y * sin(alpha)
// Y = y * cos(alpha) + x * sin(alpha)

template<IntervalType Interval>
Vector2<Interval> trivial_rotation(const Vector2<Interval>& vector, const Interval& alpha) {
    return Vector2<Interval>(
        trivial_harmonic(vector.x(), -vector.y(), alpha),
        trivial_harmonic(vector.y(), vector.x(), alpha)
    );
}

template<IntervalType Interval>
Vector2<Interval> combined_rotation(const Vector2<Interval>& vector, const Interval& alpha) {
    return Vector2<Interval>(
        combined_harmonic(vector.x(), -vector.y(), alpha),
        combined_harmonic(vector.y(), vector.x(), alpha)
    );
}

// (X, Y, _) = Rx(phi) * Rz(theta) * (x, y, z)
// X = x * cos(theta) - y * sin(theta)
// Y = (y * cos(theta) + x * sin(theta)) * cos(phi) - z * sin(phi)

template<IntervalType Interval>
Vector2<Interval> trivial_orientation(const Vector3<Interval>& vector, const Interval& theta, const Interval& phi) {
    return Vector2<Interval>(
        trivial_harmonic(vector.x(), -vector.y(), theta),
        trivial_harmonic(trivial_harmonic(vector.y(), vector.x(), theta), -vector.z(), phi)
    );
}

template<IntervalType Interval>
Vector2<Interval> combined_projected_orientation(const Vector3<Interval>& vector, const Interval& theta, const Interval& phi) {
    return Vector2<Interval>(
        combined_harmonic(vector.x(), -vector.y(), theta),
        combined_harmonic(combined_harmonic(vector.y(), vector.x(), theta), -vector.z(), phi)
    );
}

template<IntervalType Interval>
std::vector<Vector2<Interval>> vector_hull(const Vector2<Interval>& vector) {
    return std::vector<Vector2<Interval>>{
        Vector2<Interval>(vector.x().min(), vector.y().min()),
        Vector2<Interval>(vector.x().max(), vector.y().min()),
        Vector2<Interval>(vector.x().max(), vector.y().max()),
        Vector2<Interval>(vector.x().min(), vector.y().max()),
    };
}

template<IntervalType Interval>
std::vector<Vector2<Interval>> rotation_hull(const Vector2<Interval>& vector, const Interval& alpha, const int resolution) {
    if(alpha.len() > Interval::pi() / Interval(2) * Interval(resolution)) {
        return vector_hull(combined_rotation(vector, alpha));
    }
    std::vector<Vector2<Interval>> hull;
    hull.emplace_back(trivial_rotation(vector, alpha.min()));
    const Interval scaling_factor = (alpha.rad() / Interval(resolution)).cos().inv();
    const int pieces = 2 * resolution;
    for(int i = 1; i < pieces; i += 2) {
        const Interval alpha_i = (Interval(pieces - i) * alpha.min() + Interval(i) * alpha.max()) / Interval(pieces);
        hull.emplace_back(trivial_rotation(vector, alpha_i) * scaling_factor);
    }
    hull.emplace_back(trivial_rotation(vector, alpha.max()));
    return hull;
}

template<IntervalType Interval>
std::vector<Vector2<Interval>> projected_orientation_hull(const Vector3<Interval>& vector, const Interval& theta, const Interval& phi, const int resolution) {
    if(theta.len() > Interval::pi() / Interval(2) * Interval(resolution)) {
        return vector_hull(combined_projected_orientation(vector, theta, phi));
    }
    std::vector<Vector2<Interval>> hull;
    for(const Vector2<Interval>& rotated_vector: rotation_hull(Vector2<Interval>(vector.x(), vector.y()), theta, resolution)) {
        const Interval harmonic = combined_harmonic(rotated_vector.y(), -vector.z(), phi);
        hull.emplace_back(rotated_vector.x(), harmonic.min());
        hull.emplace_back(rotated_vector.x(), harmonic.max());
    }
    return hull;
}

template<IntervalType Interval>
bool projected_oriented_vector_avoids_polygon_fixed_theta(const Polygon<Interval>& polygon, const Vector3<Interval>& vector, const Interval& theta, const Interval& phi) {
    const Vector2<Interval> projected_vector = combined_projected_orientation(vector, theta, phi);
    const Edge projected_edge(
        Vector2<Interval>(projected_vector.x(), projected_vector.y().min()),
        Vector2<Interval>(projected_vector.x(), projected_vector.y().max())
    );
    return std::ranges::all_of(polygon.edges(), [&](const Edge<Interval>& edge) {
        return projected_edge.avoids(edge);
    });
}

template<IntervalType Interval>
bool projected_oriented_vector_avoids_edge_fixed_phi(const Vector3<Interval>& vector, const Interval& theta, const Interval& phi, const Edge<Interval>& edge) {
    const Interval translation_factor = vector.z() * phi.sin();
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

    const Interval radius_squared = vector.x().sqr() + vector.y().sqr();
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

    const Vector2<Interval> min_projected_vector = trivial_orientation(vector, theta.min(), phi);
    const Vector2<Interval> max_projected_vector = trivial_orientation(vector, theta.max(), phi);
    const Vector2<Interval> transformed_min_projected_vector = Vector2<Interval>(
        min_projected_vector.x(),
        (min_projected_vector.y() + translation_factor) / scaling_factor
    );
    const Vector2<Interval> transformed_max_projected_vector = Vector2<Interval>(
        max_projected_vector.x(),
        (max_projected_vector.y() + translation_factor) / scaling_factor
    );
    const Edge<Interval> transformed_projected_vector_edge(transformed_min_projected_vector, transformed_max_projected_vector);
    for(const Interval& solution: solutions) {
        if(solution.neg() || solution > transformed_edge.len()) {
            continue;
        }
        const Vector2<Interval> intersection = transformed_edge.from() + transformed_edge.dir() * transformed_edge.len() * solution;
        if(!transformed_projected_vector_edge.avoids(Edge<Interval>(Vector2<Interval>(Interval(0), Interval(0)), intersection))) {
            return false;
        }
    }
    return true;
}

template<IntervalType Interval>
bool projected_oriented_vector_avoids_polygon_fixed_phi(const Polygon<Interval>& polygon, const Vector3<Interval>& vector, const Interval& theta, const Interval& phi) {
    if(!phi.cos().nonz()) {
        return polygon.outside(combined_projected_orientation(vector, theta, phi));
    }
    return std::ranges::all_of(polygon.edges(), [&](const Edge<Interval>& edge) {
        return projected_oriented_vector_avoids_edge_fixed_phi(vector, theta, phi, edge);
    });
}

template<IntervalType Interval>
bool projected_oriented_vector_avoids_polygon(const Polygon<Interval>& polygon, const Vector3<Interval>& vector, const Interval& theta, const Interval& phi) {
    if(!(theta.len() < Interval::pi() / Interval(2))) {
        return polygon.outside(combined_projected_orientation(vector, theta, phi));
    }
    return polygon.outside(trivial_orientation(vector, theta.min(), phi.min())) &&
           polygon.outside(trivial_orientation(vector, theta.max(), phi.max())) &&
           polygon.outside(trivial_orientation(vector, theta.min(), phi.max())) &&
           polygon.outside(trivial_orientation(vector, theta.max(), phi.min())) &&
           projected_oriented_vector_avoids_polygon_fixed_theta(polygon, vector, theta.min(), phi) &&
           projected_oriented_vector_avoids_polygon_fixed_theta(polygon, vector, theta.max(), phi) &&
           projected_oriented_vector_avoids_polygon_fixed_phi(polygon, vector, theta, phi.min()) &&
           projected_oriented_vector_avoids_polygon_fixed_phi(polygon, vector, theta, phi.max());
}

template<IntervalType Interval>
Vector2<Interval> merge_vectors(const std::vector<Vector2<Interval>>& vectors) {
    if(vectors.empty()) {
        throw std::runtime_error("vectors is empty");
    }
    if(vectors.size() == 1) {
        return vectors.front();
    }
    return vectors[0].hull(merge_vectors(std::vector<Vector2<Interval>>(vectors.begin() + 1, vectors.end())));
}

template<typename Interval>
std::vector<Vector2<Interval>> deduplicate_vectors(const std::vector<Vector2<Interval>>& vectors) {
    size_t max_index = 0;
    for(size_t i = 1; i < vectors.size(); i++) {
        if(vectors[i].dist(vectors[i]).max() > vectors[max_index].dist(vectors[max_index]).max()) {
            max_index = i;
        }
    }
    const Interval max_dist = vectors[max_index].dist(vectors[max_index]).max() * Interval(static_cast<int>(vectors.size()));

    std::vector<size_t> parent(vectors.size());
    for(size_t i = 0; i < vectors.size(); i++) {
        parent[i] = i;
    }
    for(size_t i = 1; i < vectors.size(); i++) {
        for(size_t j = 0; j < i; j++) {
            if(vectors[i].dist(vectors[j]) < max_dist) {
                parent[i] = j;
                break;
            }
        }
    }

    std::vector<size_t> root(vectors.size());
    for(size_t i = 0; i < vectors.size(); i++) {
        size_t current = i;
        while(parent[current] != current) {
            current = parent[current];
        }
        root[i] = current;
    }

    std::vector<Vector2<Interval>> merged_vectors;
    bool merged_any = false;
    for(size_t i = 0; i < vectors.size(); i++) {
        std::vector<Vector2<Interval>> vector_group;
        for(size_t j = 0; j < vectors.size(); j++) {
            if(root[j] == i) {
                vector_group.push_back(vectors[j]);
            }
        }
        if(vector_group.empty()) {
            continue;
        }
        if(vector_group.size() == 1) {
            merged_vectors.push_back(vector_group.front());
            continue;
        }
        merged_vectors.push_back(merge_vectors(vector_group));
        merged_any = true;
    }
    if(!merged_any) {
        return merged_vectors;
    }
    return deduplicate_vectors(merged_vectors);
}

template<IntervalType Interval>
Polygon<Interval> convex_hull(const std::vector<Vector2<Interval>>& vectors) {
    std::vector<Edge<Interval>> edges;

    std::queue<size_t> queue;
    std::vector<bool> visited(vectors.size(), false);

    size_t start_index = 0;
    for(size_t i = 1; i < vectors.size(); i++) {
        if(vectors[i].len().max() > vectors[start_index].len().max()) {
            start_index = i;
        }
    }
    queue.emplace(start_index);

    while(!queue.empty()) {
        const size_t from_index = queue.front();
        queue.pop();
        if(visited[from_index]) {
            continue;
        }
        visited[from_index] = true;

        std::optional<size_t> most_clockwise_index;
        for(size_t i = 0; i < vectors.size(); i++) {
            if(i == from_index || Edge<Interval>(vectors[from_index]).side(vectors[i]) == Side::right) {
                continue;
            }
            if(!vectors[from_index].diff(vectors[i])) {
                throw std::runtime_error("Zero length edge found");
            }
            if(!most_clockwise_index.has_value() || Edge<Interval>(vectors[from_index], vectors[most_clockwise_index.value()]).side(vectors[i]) == Side::right) {
                most_clockwise_index = i;
            }
        }
        if(!most_clockwise_index.has_value()) {
            throw std::runtime_error("No most clockwise vector found");
        }

        std::vector<size_t> to_indices;
        const Edge<Interval> most_clockwise_edge(vectors[from_index], vectors[most_clockwise_index.value()]);
        for(size_t i = 0; i < vectors.size(); i++) {
            if(i == from_index || Edge<Interval>(vectors[from_index]).side(vectors[i]) == Side::right) {
                continue;
            }
            if(!vectors[from_index].diff(vectors[i])) {
                throw std::runtime_error("Zero length edge found");
            }
            if(most_clockwise_edge.side(vectors[i]) == Side::left) {
                continue;
            }
            bool is_most_clockwise = true;
            const Edge<Interval> edge(vectors[from_index], vectors[i]);
            for(size_t j = 0; j < vectors.size(); j++) {
                if(j == from_index || j == i) {
                    continue;
                }
                if(edge.side(vectors[j]) == Side::right) {
                    is_most_clockwise = false;
                    break;
                }
            }
            if(is_most_clockwise) {
                to_indices.push_back(i);
            }
        }
        std::ranges::stable_sort(to_indices, [&](const size_t index, const size_t other_index) {
            const Interval dist = vectors[from_index].dist(vectors[other_index]).max();
            const Interval other_dist = vectors[from_index].dist(vectors[index]).max();
            return dist < other_dist;
        });
        std::vector<std::pair<size_t, size_t>> new_edge_indices;
        for(const size_t to_index: to_indices) {
            const bool is_most_clockwise = std::ranges::none_of(new_edge_indices, [&](const std::pair<size_t, size_t>& new_edge_index) {
                const size_t from_edge_index = new_edge_index.first;
                const size_t to_edge_index = new_edge_index.second;
                const Edge<Interval> new_edge(vectors[from_edge_index], vectors[to_edge_index]);
                return new_edge.side(vectors[to_index]) == Side::left;
            });
            if(!is_most_clockwise) {
                continue;
            }
            new_edge_indices.emplace_back(from_index, to_index);
            if(!visited[to_index]) {
                queue.emplace(to_index);
            }
        }
        for(const auto& [from_edge_index, to_edge_index]: new_edge_indices) {
            edges.emplace_back(vectors[from_edge_index], vectors[to_edge_index]);
        }
    }
    return Polygon(edges);
}

template<IntervalType Interval>
Polygon<Interval> project_polyhedron(const Polyhedron<Interval>& polyhedron, const Range3& orientation, const int projection_resolution, const int rotation_resolution) {
    std::vector<Vector2<Interval>> projected_vectors;
    for(const Vector3<Interval>& vertex: polyhedron.vertices()) {
        for(const Vector2<Interval>& vectors: projected_orientation_hull(vertex, orientation.theta<Interval>(), orientation.phi<Interval>(), projection_resolution)) {
            for(const Vector2<Interval>& vector: rotation_hull(vectors, orientation.alpha<Interval>(), rotation_resolution)) {
                projected_vectors.push_back(vector);
            }
        }
    }
    return convex_hull(deduplicate_vectors(projected_vectors));
}

template<IntervalType Interval>
bool plug_orientation_sample_inside_hole_orientation_sample(const Polyhedron<Interval>& polyhedron, const Range3& hole_orientation, const Range2& plug_orientation) {
    const Interval hole_theta = hole_orientation.theta_mid<Interval>();
    const Interval hole_phi = hole_orientation.phi_mid<Interval>();
    const Interval hole_alpha = hole_orientation.alpha_mid<Interval>();
    const Vector3<Interval> direction = Vector3<Interval>(
        hole_theta.sin() * hole_phi.sin(),
        hole_theta.cos() * hole_phi.sin(),
        hole_phi.cos()
    );
    const Bitset normal_mask = polyhedron.get_normal_mask(direction);
    const auto outline_iterator = std::ranges::find_if(polyhedron.outlines(), [&](const Outline& candidate_outline) {
        return candidate_outline.normal_mask == normal_mask;
    });
    if(outline_iterator == polyhedron.outlines().end()) {
        return false;
    }
    const Outline& outline = *outline_iterator;
    const Matrix<Interval> hole_matrix = Matrix<Interval>::rotation_z(hole_alpha) * Matrix<Interval>::orientation(hole_theta, hole_phi);
    std::vector<Vector2<Interval>> projected_vertices;
    for(const size_t vertex_index: outline.vertex_indices) {
        const Vector3<Interval> vertex = polyhedron.vertices()[vertex_index];
        const Vector3<Interval> projected_vertex = hole_matrix * vertex;
        projected_vertices.emplace_back(
            projected_vertex.x(),
            projected_vertex.y()
        );
    }
    std::vector<Edge<Interval>> projected_edges;
    for(size_t index = 0; index < projected_vertices.size(); ++index) {
        const size_t next_index = (index + 1) % projected_vertices.size();
        projected_edges.emplace_back(projected_vertices[index], projected_vertices[next_index]);
    }
    const Polygon<Interval> projected_hole = Polygon<Interval>(projected_edges);
    return plug_orientation_sample_inside_hole_orientation(polyhedron, projected_hole, plug_orientation);
}

template<IntervalType Interval>
bool plug_orientation_sample_inside_hole_orientation(const Polyhedron<Interval>& polyhedron, const Polygon<Interval>& projected_hole, const Range2& plug_orientation) {
    const Interval plug_theta = plug_orientation.theta_mid<Interval>();
    const Interval plug_phi = plug_orientation.phi_mid<Interval>();
    return std::ranges::all_of(polyhedron.vertices(), [&](const Vector3<Interval>& vertex) {
        return projected_hole.inside(trivial_orientation(vertex, plug_theta, plug_phi));
    });
}

template<IntervalType Interval>
bool plug_orientation_skippable(const Polyhedron<Interval>& polyhedron, const Range3& hole_orientation, const Range2& plug_orientation, const Interval& epsilon) {
    const Interval hole_orientation_angle_radius = Vector2<Interval>(hole_orientation.theta<Interval>().rad() + hole_orientation.alpha<Interval>().rad(), hole_orientation.phi<Interval>().rad()).len();
    const Interval plug_orientation_angle_radius = Vector2<Interval>(plug_orientation.theta<Interval>().rad(), plug_orientation.phi<Interval>().rad()).len();
    const Interval remaining_angle = epsilon - hole_orientation_angle_radius - plug_orientation_angle_radius;
    if(!remaining_angle.pos()) {
        return false;
    }
    const Interval cos_remaining_angle = remaining_angle.cos();
    const Matrix<Interval> hole_matrix = Matrix<Interval>::rotation_z(hole_orientation.alpha<Interval>().mid()) * Matrix<Interval>::orientation(hole_orientation.theta<Interval>().mid(), hole_orientation.phi<Interval>().mid());
    const Matrix<Interval> plug_matrix = Matrix<Interval>::orientation(plug_orientation.theta<Interval>().mid(), plug_orientation.phi<Interval>().mid());
    return std::ranges::any_of(polyhedron.rotations(), [&](const Matrix<Interval>& rotation) {
               return cos_remaining_angle < Matrix<Interval>::relative_rotation(plug_matrix, hole_matrix * rotation).cos_angle();
           }) ||
           std::ranges::any_of(polyhedron.reflections(), [&](const Matrix<Interval>& reflection) {
               return cos_remaining_angle < Matrix<Interval>::relative_rotation(plug_matrix, Matrix<Interval>::reflection_z() * hole_matrix * reflection).cos_angle();
           });
}

template<IntervalType Interval>
bool plug_orientation_prunable(const Polyhedron<Interval>& polyhedron, const Range2& plug_orientation, const Polygon<Interval>& projected_hole) {
    return std::ranges::any_of(polyhedron.vertices(), [&](const Vector3<Interval>& vertex) {
        return projected_oriented_vector_avoids_polygon(projected_hole, vertex, plug_orientation.theta<Interval>(), plug_orientation.phi<Interval>());
    });
}
