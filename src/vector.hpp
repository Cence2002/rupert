#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include "interval.hpp"

template<typename V, typename I>
concept VectorType =
        IntervalType<I> &&

        std::is_default_constructible_v<V> &&
        std::is_copy_constructible_v<V> &&
        std::is_copy_assignable_v<V> &&
        std::is_destructible_v<V> &&

        requires(const V vector, const V v, const I i, const int n) {
            { +vector } -> std::same_as<V>;
            { -vector } -> std::same_as<V>;

            { vector + v } -> std::same_as<V>;
            { vector + i } -> std::same_as<V>;
            { vector + n } -> std::same_as<V>;
            { i + vector } -> std::same_as<V>;
            { n + vector } -> std::same_as<V>;

            { vector - v } -> std::same_as<V>;
            { vector - i } -> std::same_as<V>;
            { vector - n } -> std::same_as<V>;
            { i - vector } -> std::same_as<V>;
            { n - vector } -> std::same_as<V>;

            { vector * v } -> std::same_as<V>;
            { vector * i } -> std::same_as<V>;
            { vector * n } -> std::same_as<V>;
            { i * vector } -> std::same_as<V>;
            { n * vector } -> std::same_as<V>;

            { vector / v } -> std::same_as<V>;
            { vector / i } -> std::same_as<V>;
            { vector / n } -> std::same_as<V>;
            { i / vector } -> std::same_as<V>;
            { n / vector } -> std::same_as<V>;

            { vector.len() } -> std::same_as<I>;
        } &&

        requires(const V a, std::ostream &os) {
            { os << a } -> std::same_as<std::ostream&>;
        };

template<typename V, typename I>
concept Vector2Type =
        VectorType<V, I> &&

        std::is_constructible_v<V, I, I> &&
        std::is_constructible_v<V, I, int> &&
        std::is_constructible_v<V, int, I> &&
        std::is_constructible_v<V, int, int> &&

        requires(V vector) {
            { vector.x } -> std::same_as<I>;
            { vector.y } -> std::same_as<I>;
        };

template<typename V, typename I>
concept Vector3Type =
        VectorType<V, I> &&

        std::is_constructible_v<V, I, I, I> &&
        std::is_constructible_v<V, I, I, int> &&
        std::is_constructible_v<V, I, int, I> &&
        std::is_constructible_v<V, I, int, int> &&
        std::is_constructible_v<V, int, I, I> &&
        std::is_constructible_v<V, int, I, int> &&
        std::is_constructible_v<V, int, int, I> &&
        std::is_constructible_v<V, int, int, int> &&

        requires(const V vector) {
            { vector.x } -> std::same_as<I>;
            { vector.y } -> std::same_as<I>;
            { vector.z } -> std::same_as<I>;
        };

enum class Orientation {
    COUNTERCLOCKWISE,
    CLOCKWISE,
    COLLINEAR
};

inline std::ostream &operator<<(std::ostream &os, const Orientation &orientation) {
    switch(orientation) {
        case Orientation::COUNTERCLOCKWISE: return os << "COUNTERCLOCKWISE";
        case Orientation::CLOCKWISE: return os << "CLOCKWISE";
        case Orientation::COLLINEAR: return os << "COLLINEAR";
        default: throw std::invalid_argument("Invalid orientation");
    }
}

template<IntervalType I>
struct Vector2 {
    I x, y;

    explicit Vector2() = default;

    Vector2(const Vector2 &v) = default;

    Vector2 &operator=(const Vector2 &v) = default;

    ~Vector2() = default;

    explicit Vector2(const I x, const I y) : x(x), y(y) {}

    template<IntegerType Int>
    explicit Vector2(const I x, const Int y) : x(x), y(I(y)) {}

    template<IntegerType Int>
    explicit Vector2(const Int x, const I y) : x(I(x)), y(y) {}

    template<IntegerType Int>
    explicit Vector2(const Int x, const Int y) : x(I(x)), y(I(y)) {}

    Vector2 operator+() const {
        return Vector2(+x, +y);
    }

    Vector2 operator-() const {
        return Vector2(-x, -y);
    }

    Vector2 operator+(const Vector2 &v) const {
        return Vector2(x + v.x, y + v.y);
    }

    Vector2 operator+(const I i) const {
        return Vector2(x + i, y + i);
    }

    template<IntegerType Int>
    Vector2 operator+(const Int n) const {
        return Vector2(x + n, y + n);
    }

    friend Vector2 operator+(const I i, const Vector2 &v) {
        return Vector2(i + v.x, i + v.y);
    }

    template<IntegerType Int>
    friend Vector2 operator+(const Int n, const Vector2 &v) {
        return Vector2(I(n) + v.x, I(n) + v.y);
    }

    Vector2 operator-(const Vector2 &v) const {
        return Vector2(x - v.x, y - v.y);
    }

    Vector2 operator-(const I i) const {
        return Vector2(x - i, y - i);
    }

    template<IntegerType Int>
    Vector2 operator-(const Int n) const {
        return Vector2(x - n, y - n);
    }

    friend Vector2 operator-(const I i, const Vector2 &v) {
        return Vector2(i - v.x, i - v.y);
    }

    template<IntegerType Int>
    friend Vector2 operator-(const Int n, const Vector2 &v) {
        return Vector2(I(n) - v.x, I(n) - v.y);
    }

    Vector2 operator*(const Vector2 &v) const {
        return Vector2(x * v.x, y * v.y);
    }

    Vector2 operator*(const I i) const {
        return Vector2(x * i, y * i);
    }

    template<IntegerType Int>
    Vector2 operator*(const Int n) const {
        return Vector2(x * n, y * n);
    }

    friend Vector2 operator*(const I i, const Vector2 &v) {
        return Vector2(i * v.x, i * v.y);
    }

    template<IntegerType Int>
    friend Vector2 operator*(const Int n, const Vector2 &v) {
        return Vector2(I(n) * v.x, I(n) * v.y);
    }

    Vector2 operator/(const Vector2 &v) const {
        return Vector2(x / v.x, y / v.y);
    }

    Vector2 operator/(const I i) const {
        return Vector2(x / i, y / i);
    }

    template<IntegerType Int>
    Vector2 operator/(const Int n) const {
        return Vector2(x / n, y / n);
    }

    friend Vector2 operator/(const I i, const Vector2 &v) {
        return Vector2(i / v.x, i / v.y);
    }

    template<IntegerType Int>
    friend Vector2 operator/(const Int n, const Vector2 &v) {
        return Vector2(I(n) / v.x, I(n) / v.y);
    }

    I len() const {
        return (x.sqr() + y.sqr()).sqrt();
    }

    friend std::ostream &operator<<(std::ostream &os, const Vector2 &v) {
        return os << "(" << v.x << " , " << v.y << ")";
    }

    Vector2 rotate(const I angle) const {
        const I sin_angle = angle.sin();
        const I cos_angle = angle.cos();
        return Vector2(
            x * cos_angle - y * sin_angle,
            x * sin_angle + y * cos_angle
        );
    }

    I cross(const Vector2 &v) const {
        return x * v.y - y * v.x;
    }

    Orientation orientation(const Vector2 &v) const {
        const I cross_product = cross(v);
        if(cross_product.is_pos()) {
            return Orientation::COUNTERCLOCKWISE;
        }
        if(cross_product.is_neg()) {
            return Orientation::CLOCKWISE;
        }
        return Orientation::COLLINEAR;
    }
};

template<IntervalType I>
struct Vector3 {
    I x, y, z;

    explicit Vector3() = default;

    Vector3(const Vector3 &v) = default;

    Vector3 &operator=(const Vector3 &v) = default;

    ~Vector3() = default;

    explicit Vector3(const I x, const I y, const I z) : x(x), y(y), z(z) {}

    template<IntegerType Int>
    explicit Vector3(const I x, const I y, const Int z) : x(x), y(y), z(I(z)) {}

    template<IntegerType Int>
    explicit Vector3(const I x, const Int y, const I z) : x(x), y(I(y)), z(z) {}

    template<IntegerType Int>
    explicit Vector3(const I x, const Int y, const Int z) : x(x), y(I(y)), z(I(z)) {}

    template<IntegerType Int>
    explicit Vector3(const Int x, const I y, const I z) : x(I(x)), y(y), z(z) {}

    template<IntegerType Int>
    explicit Vector3(const Int x, const I y, const Int z) : x(I(x)), y(y), z(I(z)) {}

    template<IntegerType Int>
    explicit Vector3(const Int x, const Int y, const I z) : x(I(x)), y(I(y)), z(z) {}

    template<IntegerType Int>
    explicit Vector3(const Int x, const Int y, const Int z) : x(I(x)), y(I(y)), z(I(z)) {}

    Vector3 operator+() const {
        return Vector3(+x, +y, +z);
    }

    Vector3 operator-() const {
        return Vector3(-x, -y, -z);
    }

    Vector3 operator+(const Vector3 &v) const {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }

    Vector3 operator+(const I i) const {
        return Vector3(x + i, y + i, z + i);
    }

    template<IntegerType Int>
    Vector3 operator+(const Int n) const {
        return Vector3(x + n, y + n, z + n);
    }

    friend Vector3 operator+(const I i, const Vector3 &v) {
        return Vector3(i + v.x, i + v.y, i + v.z);
    }

    template<IntegerType Int>
    friend Vector3 operator+(const Int n, const Vector3 &v) {
        return Vector3(I(n) + v.x, I(n) + v.y, I(n) + v.z);
    }

    Vector3 operator-(const Vector3 &v) const {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }

    Vector3 operator-(const I i) const {
        return Vector3(x - i, y - i, z - i);
    }

    template<IntegerType Int>
    Vector3 operator-(const Int n) const {
        return Vector3(x - n, y - n, z - n);
    }

    friend Vector3 operator-(const I i, const Vector3 &v) {
        return Vector3(i - v.x, i - v.y, i - v.z);
    }

    template<IntegerType Int>
    friend Vector3 operator-(const Int n, const Vector3 &v) {
        return Vector3(I(n) - v.x, I(n) - v.y, I(n) - v.z);
    }

    Vector3 operator*(const Vector3 &v) const {
        return Vector3(x * v.x, y * v.y, z * v.z);
    }

    Vector3 operator*(const I i) const {
        return Vector3(x * i, y * i, z * i);
    }

    template<IntegerType Int>
    Vector3 operator*(const Int n) const {
        return Vector3(x * n, y * n, z * n);
    }

    friend Vector3 operator*(const I i, const Vector3 &v) {
        return Vector3(i * v.x, i * v.y, i * v.z);
    }

    template<IntegerType Int>
    friend Vector3 operator*(const Int n, const Vector3 &v) {
        return Vector3(I(n) * v.x, I(n) * v.y, I(n) * v.z);
    }

    Vector3 operator/(const Vector3 &v) const {
        return Vector3(x / v.x, y / v.y, z / v.z);
    }

    Vector3 operator/(const I i) const {
        return Vector3(x / i, y / i, z / i);
    }

    template<IntegerType Int>
    Vector3 operator/(const Int n) const {
        return Vector3(x / n, y / n, z / n);
    }

    friend Vector3 operator/(const I i, const Vector3 &v) {
        return Vector3(i / v.x, i / v.y, i / v.z);
    }

    template<IntegerType Int>
    friend Vector3 operator/(const Int n, const Vector3 &v) {
        return Vector3(I(n) / v.x, I(n) / v.y, I(n) / v.z);
    }

    I len() const {
        return (x.sqr() + y.sqr() + z.sqr()).sqrt();
    }

    friend std::ostream &operator<<(std::ostream &os, const Vector3 &v) {
        return os << "(" << v.x << " , " << v.y << ", " << v.z << ")";
    }

    Vector2<I> project(const I theta, const I phi) const {
        const I sin_theta = theta.sin();
        const I cos_theta = theta.cos();
        return Vector2<I>(
            -x * sin_theta + y * cos_theta,
            (x * cos_theta + y * sin_theta) * phi.cos() - z * phi.sin()
        );
    }
};

template<IntervalType I>
struct Line {
private:
    Vector2<I> from;
    Vector2<I> to;

public:
    explicit Line(const Vector2<I> &from, const Vector2<I> &to) : from(from), to(to) {}

    explicit Line(const Vector2<I> &to) : from(Vector2<I>(0, 0)), to(to) {}

    Orientation orientation(const Vector2<I> &v) const {
        return (to - from).orientation(v - from);
    }

    bool opposite_side(const Line &l) const {
        const Orientation orientation_from = orientation(l.from);
        const Orientation orientation_to = orientation(l.to);
        if(orientation_from == Orientation::COLLINEAR || orientation_to == Orientation::COLLINEAR) {
            return false;
        }
        return orientation_from != orientation_to;
    }

    bool same_side(const Line &l) const {
        const Orientation orientation_from = orientation(l.from);
        const Orientation orientation_to = orientation(l.to);
        if(orientation_from == Orientation::COLLINEAR || orientation_to == Orientation::COLLINEAR) {
            return false;
        }
        return orientation_from == orientation_to;
    }

    bool intersects(const Line &l) const {
        return opposite_side(l) && l.opposite_side(*this);
    }

    bool avoids(const Line &l) const {
        return same_side(l) || l.same_side(*this);
    }

    friend std::ostream &operator<<(std::ostream &os, const Line &line) {
        return os << line.from << " -> " << line.to;
    }
};

template<IntervalType I>
struct Polygon {
private:
    std::vector<Line<I>> edges;

public:
    explicit Polygon() = default;

    Polygon(const Polygon &polygon) = default;

    Polygon &operator=(const Polygon &polygon) = default;

    explicit Polygon(const std::vector<Line<I>> &edges) : edges(edges) {}

    static Polygon convex_hull(const std::vector<Vector2<I>> &vertices) {
        std::vector<Line<I>> edges;

        std::vector<bool> visited(vertices.size(), false);

        size_t max_index = 0;
        for(size_t better_max_index = 1; better_max_index < vertices.size(); better_max_index++) {
            const I better_max = vertices[better_max_index].x;
            if(better_max > vertices[max_index].x) {
                max_index = better_max_index;
            }
        }
        std::queue<size_t> queue({max_index});

        while(!queue.empty()) {
            const size_t from_index = queue.front();
            queue.pop();
            if(visited[from_index]) {
                continue;
            }
            visited[from_index] = true;
            const Vector2<I> from = vertices[from_index];
            size_t most_clockwise_index_sample = (from_index + 1) % vertices.size();
            for(size_t clockwise_index = 0; clockwise_index < vertices.size(); clockwise_index++) {
                if(clockwise_index == from_index) {
                    continue;
                }
                const Vector2<I> to = vertices[clockwise_index];
                if(Line<I>(from, vertices[most_clockwise_index_sample]).orientation(to) == Orientation::CLOCKWISE) {
                    most_clockwise_index_sample = clockwise_index;
                }
            }
            std::vector<size_t> most_clockwise_indices;
            for(size_t clockwise_index = 0; clockwise_index < vertices.size(); clockwise_index++) {
                if(from_index == clockwise_index) {
                    continue;
                }
                const Vector2<I> clockwise = vertices[clockwise_index];
                if(Line<I>(from, vertices[most_clockwise_index_sample]).orientation(clockwise) == Orientation::COUNTERCLOCKWISE) {
                    continue;
                }
                bool most_clockwise = true;
                for(size_t clockwise_clockwise_index = 0; clockwise_clockwise_index < vertices.size(); clockwise_clockwise_index++) {
                    if(clockwise_clockwise_index == from_index || clockwise_clockwise_index == clockwise_index) {
                        continue;
                    }
                    const Vector2<I> clockwise_clockwise = vertices[clockwise_clockwise_index];
                    if(Line<I>(from, clockwise).orientation(clockwise_clockwise) == Orientation::CLOCKWISE) {
                        most_clockwise = false;
                        break;
                    }
                }
                if(most_clockwise) {
                    most_clockwise_indices.push_back(clockwise_index);
                }
            }
            std::ranges::sort(most_clockwise_indices, [&](const size_t a, const size_t b) {
                return (vertices[a] - from).len().min() < (vertices[b] - from).len().min();
            });
            for(const size_t most_clockwise_index: most_clockwise_indices) {
                edges.push_back(Line<I>(vertices[from_index], vertices[most_clockwise_index]));
                if(!visited[most_clockwise_index]) {
                    queue.push(most_clockwise_index);
                }
            }
        }
        return Polygon(edges);
    }

    bool is_inside(const Vector2<I> &v) const {
        bool all_counter_clockwise = true;
        for(const Line<I> &edge: edges) {
            const Orientation orientation = edge.orientation(v);
            if(orientation == Orientation::COLLINEAR) {
                return false;
            }
            all_counter_clockwise &= orientation == Orientation::COUNTERCLOCKWISE;
        }
        return all_counter_clockwise;
    }

    bool is_outside(const Vector2<I> &v) const {
        bool any_clockwise = false;
        for(const Line<I> &edge: edges) {
            const Orientation orientation = edge.orientation(v);
            if(orientation == Orientation::COLLINEAR) {
                return false;
            }
            any_clockwise |= orientation == Orientation::CLOCKWISE;
        }
        return any_clockwise;
    }

    friend std::ostream &operator<<(std::ostream &os, const Polygon &polygon) {
        for(const Line<I> &edge: polygon.edges) {
            os << edge << std::endl;
        }
        return os;
    }
};
