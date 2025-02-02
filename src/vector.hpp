#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include "interval.hpp"

template<typename V, typename I>
concept VectorType =
        IntervalType<I> &&

        std::is_default_constructible_v<V> &&
        std::is_destructible_v<V> &&
        std::is_copy_constructible_v<V> &&
        std::is_copy_assignable_v<V> &&
        std::is_move_constructible_v<V> &&
        std::is_move_assignable_v<V> &&

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

        requires(const V a, std::ostream& os) {
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
            { vector.x() } -> std::convertible_to<I&>;
            { vector.y() } -> std::convertible_to<I&>;
        } &&

        requires(const V vector) {
            { vector.x() } -> std::convertible_to<const I&>;
            { vector.y() } -> std::convertible_to<const I&>;
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

        requires(V vector) {
            { vector.x() } -> std::convertible_to<I&>;
            { vector.y() } -> std::convertible_to<I&>;
            { vector.z() } -> std::convertible_to<I&>;
        } &&

        requires(const V vector) {
            { vector.x() } -> std::convertible_to<const I&>;
            { vector.y() } -> std::convertible_to<const I&>;
            { vector.z() } -> std::convertible_to<const I&>;
        };

enum class Orientation {
    COUNTERCLOCKWISE,
    CLOCKWISE,
    COLLINEAR
};

inline std::ostream& operator<<(std::ostream& os, const Orientation& orientation) {
    switch(orientation) {
        case Orientation::COUNTERCLOCKWISE: return os << "COUNTERCLOCKWISE";
        case Orientation::CLOCKWISE: return os << "CLOCKWISE";
        case Orientation::COLLINEAR: return os << "COLLINEAR";
        default: throw std::invalid_argument("Invalid orientation");
    }
}

template<IntervalType I>
struct Vector2 {
private:
    I x_;
    I y_;

public:
    explicit Vector2() : x_(), y_() {}

    ~Vector2() = default;

    Vector2(const Vector2& v) = default;

    Vector2& operator=(const Vector2& v) = default;

    Vector2(Vector2&& v) = default;

    Vector2& operator=(Vector2&& v) = default;

    explicit Vector2(const I& x, const I& y) : x_(x), y_(y) {}

    template<IntegerType Int>
    explicit Vector2(const I& x, const Int y) : x_(x), y_(I(y)) {}

    template<IntegerType Int>
    explicit Vector2(const Int x, const I& y) : x_(I(x)), y_(y) {}

    template<IntegerType Int>
    explicit Vector2(const Int x, const Int y) : x_(I(x)), y_(I(y)) {}

    I& x() {
        return x_;
    }

    I& y() {
        return y_;
    }

    const I& x() const {
        return x_;
    }

    const I& y() const {
        return y_;
    }

    Vector2 operator+() const {
        return Vector2(+x_, +y_);
    }

    Vector2 operator-() const {
        return Vector2(-x_, -y_);
    }

    Vector2 operator+(const Vector2& v) const {
        return Vector2(x_ + v.x_, y_ + v.y_);
    }

    Vector2 operator+(const I& i) const {
        return Vector2(x_ + i, y_ + i);
    }

    template<IntegerType Int>
    Vector2 operator+(const Int n) const {
        return Vector2(x_ + n, y_ + n);
    }

    friend Vector2 operator+(const I& i, const Vector2& v) {
        return Vector2(i + v.x_, i + v.y_);
    }

    template<IntegerType Int>
    friend Vector2 operator+(const Int n, const Vector2& v) {
        return Vector2(I(n) + v.x_, I(n) + v.y_);
    }

    Vector2 operator-(const Vector2& v) const {
        return Vector2(x_ - v.x_, y_ - v.y_);
    }

    Vector2 operator-(const I& i) const {
        return Vector2(x_ - i, y_ - i);
    }

    template<IntegerType Int>
    Vector2 operator-(const Int n) const {
        return Vector2(x_ - n, y_ - n);
    }

    friend Vector2 operator-(const I& i, const Vector2& v) {
        return Vector2(i - v.x_, i - v.y_);
    }

    template<IntegerType Int>
    friend Vector2 operator-(const Int n, const Vector2& v) {
        return Vector2(I(n) - v.x_, I(n) - v.y_);
    }

    Vector2 operator*(const Vector2& v) const {
        return Vector2(x_ * v.x_, y_ * v.y_);
    }

    Vector2 operator*(const I& i) const {
        return Vector2(x_ * i, y_ * i);
    }

    template<IntegerType Int>
    Vector2 operator*(const Int n) const {
        return Vector2(x_ * n, y_ * n);
    }

    friend Vector2 operator*(const I& i, const Vector2& v) {
        return Vector2(i * v.x_, i * v.y_);
    }

    template<IntegerType Int>
    friend Vector2 operator*(const Int n, const Vector2& v) {
        return Vector2(I(n) * v.x_, I(n) * v.y_);
    }

    Vector2 operator/(const Vector2& v) const {
        return Vector2(x_ / v.x_, y_ / v.y_);
    }

    Vector2 operator/(const I& i) const {
        return Vector2(x_ / i, y_ / i);
    }

    template<IntegerType Int>
    Vector2 operator/(const Int n) const {
        return Vector2(x_ / n, y_ / n);
    }

    friend Vector2 operator/(const I& i, const Vector2& v) {
        return Vector2(i / v.x_, i / v.y_);
    }

    template<IntegerType Int>
    friend Vector2 operator/(const Int n, const Vector2& v) {
        return Vector2(I(n) / v.x_, I(n) / v.y_);
    }

    I len() const {
        return (x_.sqr() + y_.sqr()).sqrt();
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector2& v) {
        return os << "(" << v.x_ << " , " << v.y_ << ")";
    }

    Vector2 rotate(const I& angle) const {
        const I sin_angle = angle.sin();
        const I cos_angle = angle.cos();
        return Vector2(
            x_ * cos_angle - y_ * sin_angle,
            x_ * sin_angle + y_ * cos_angle
        );
    }

    I cross(const Vector2& v) const {
        return x_ * v.y_ - y_ * v.x_;
    }

    Orientation orientation(const Vector2& v) const {
        const I cross_product = cross(v);
        if(cross_product.pos()) {
            return Orientation::COUNTERCLOCKWISE;
        }
        if(cross_product.neg()) {
            return Orientation::CLOCKWISE;
        }
        return Orientation::COLLINEAR;
    }
};

static_assert(Vector2Type<Vector2<FastInterval>, FastInterval>);
static_assert(Vector2Type<Vector2<BoostInterval>, BoostInterval>);
static_assert(Vector2Type<Vector2<PreciseInterval>, PreciseInterval>);

template<IntervalType I>
struct Vector3 {
private:
    I x_;
    I y_;
    I z_;

public:
    explicit Vector3() = default;

    ~Vector3() = default;

    Vector3(const Vector3& v) = default;

    Vector3& operator=(const Vector3& v) = default;

    Vector3(Vector3&& v) = default;

    Vector3& operator=(Vector3&& v) = default;

    explicit Vector3(const I& x, const I& y, const I& z) : x_(x), y_(y), z_(z) {}

    template<IntegerType Int>
    explicit Vector3(const I& x, const I& y, const Int z) : x_(x), y_(y), z_(I(z)) {}

    template<IntegerType Int>
    explicit Vector3(const I& x, const Int y, const I& z) : x_(x), y_(I(y)), z_(z) {}

    template<IntegerType Int>
    explicit Vector3(const I& x, const Int y, const Int z) : x_(x), y_(I(y)), z_(I(z)) {}

    template<IntegerType Int>
    explicit Vector3(const Int x, const I& y, const I& z) : x_(I(x)), y_(y), z_(z) {}

    template<IntegerType Int>
    explicit Vector3(const Int x, const I& y, const Int z) : x_(I(x)), y_(y), z_(I(z)) {}

    template<IntegerType Int>
    explicit Vector3(const Int x, const Int y, const I& z) : x_(I(x)), y_(I(y)), z_(z) {}

    template<IntegerType Int>
    explicit Vector3(const Int x, const Int y, const Int z) : x_(I(x)), y_(I(y)), z_(I(z)) {}

    I& x() {
        return x_;
    }

    I& y() {
        return y_;
    }

    I& z() {
        return z_;
    }

    const I& x() const {
        return x_;
    }

    const I& y() const {
        return y_;
    }

    const I& z() const {
        return z_;
    }

    Vector3 operator+() const {
        return Vector3(+x_, +y_, +z_);
    }

    Vector3 operator-() const {
        return Vector3(-x_, -y_, -z_);
    }

    Vector3 operator+(const Vector3& v) const {
        return Vector3(x_ + v.x_, y_ + v.y_, z_ + v.z_);
    }

    Vector3 operator+(const I& i) const {
        return Vector3(x_ + i, y_ + i, z_ + i);
    }

    template<IntegerType Int>
    Vector3 operator+(const Int n) const {
        return Vector3(x_ + n, y_ + n, z_ + n);
    }

    friend Vector3 operator+(const I& i, const Vector3& v) {
        return Vector3(i + v.x_, i + v.y_, i + v.z_);
    }

    template<IntegerType Int>
    friend Vector3 operator+(const Int n, const Vector3& v) {
        return Vector3(I(n) + v.x_, I(n) + v.y_, I(n) + v.z_);
    }

    Vector3 operator-(const Vector3& v) const {
        return Vector3(x_ - v.x_, y_ - v.y_, z_ - v.z_);
    }

    Vector3 operator-(const I& i) const {
        return Vector3(x_ - i, y_ - i, z_ - i);
    }

    template<IntegerType Int>
    Vector3 operator-(const Int n) const {
        return Vector3(x_ - n, y_ - n, z_ - n);
    }

    friend Vector3 operator-(const I& i, const Vector3& v) {
        return Vector3(i - v.x_, i - v.y_, i - v.z_);
    }

    template<IntegerType Int>
    friend Vector3 operator-(const Int n, const Vector3& v) {
        return Vector3(I(n) - v.x_, I(n) - v.y_, I(n) - v.z_);
    }

    Vector3 operator*(const Vector3& v) const {
        return Vector3(x_ * v.x_, y_ * v.y_, z_ * v.z_);
    }

    Vector3 operator*(const I& i) const {
        return Vector3(x_ * i, y_ * i, z_ * i);
    }

    template<IntegerType Int>
    Vector3 operator*(const Int n) const {
        return Vector3(x_ * n, y_ * n, z_ * n);
    }

    friend Vector3 operator*(const I& i, const Vector3& v) {
        return Vector3(i * v.x_, i * v.y_, i * v.z_);
    }

    template<IntegerType Int>
    friend Vector3 operator*(const Int n, const Vector3& v) {
        return Vector3(I(n) * v.x_, I(n) * v.y_, I(n) * v.z_);
    }

    Vector3 operator/(const Vector3& v) const {
        return Vector3(x_ / v.x_, y_ / v.y_, z_ / v.z_);
    }

    Vector3 operator/(const I& i) const {
        return Vector3(x_ / i, y_ / i, z_ / i);
    }

    template<IntegerType Int>
    Vector3 operator/(const Int n) const {
        return Vector3(x_ / n, y_ / n, z_ / n);
    }

    friend Vector3 operator/(const I& i, const Vector3& v) {
        return Vector3(i / v.x_, i / v.y_, i / v.z_);
    }

    template<IntegerType Int>
    friend Vector3 operator/(const Int n, const Vector3& v) {
        return Vector3(I(n) / v.x_, I(n) / v.y_, I(n) / v.z_);
    }

    I len() const {
        return (x_.sqr() + y_.sqr() + z_.sqr()).sqrt();
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector3& v) {
        return os << "(" << v.x_ << " , " << v.y_ << ", " << v.z_ << ")";
    }

    Vector2<I> project(const I& theta, const I& phi) const {
        const I sin_theta = theta.sin();
        const I cos_theta = theta.cos();
        return Vector2<I>(
            -x_ * sin_theta + y_ * cos_theta,
            (x_ * cos_theta + y_ * sin_theta) * phi.cos() - z_ * phi.sin()
        );
    }
};

static_assert(Vector3Type<Vector3<FastInterval>, FastInterval>);
static_assert(Vector3Type<Vector3<BoostInterval>, BoostInterval>);
static_assert(Vector3Type<Vector3<PreciseInterval>, PreciseInterval>);

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

template<IntervalType I>
struct Polygon {
private:
    std::vector<Line<I>> edges_;

public:
    explicit Polygon() = default;

    Polygon(const Polygon& polygon) = default;

    Polygon& operator=(const Polygon& polygon) = default;

    explicit Polygon(const std::vector<Line<I>>& edges) : edges_(edges) {}

    static Polygon convex_hull(const std::vector<Vector2<I>>& vertices) {
        std::vector<Line<I>> edges;

        std::vector<bool> visited(vertices.size(), false);

        size_t max_index = 0;
        for(size_t better_max_index = 1; better_max_index < vertices.size(); better_max_index++) {
            const I better_max = vertices[better_max_index].x();
            if(better_max > vertices[max_index].x()) {
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

    bool is_inside(const Vector2<I>& v) const {
        bool all_counter_clockwise = true;
        for(const Line<I>& edge: edges_) {
            const Orientation orientation = edge.orientation(v);
            if(orientation == Orientation::COLLINEAR) {
                return false;
            }
            all_counter_clockwise &= orientation == Orientation::COUNTERCLOCKWISE;
        }
        return all_counter_clockwise;
    }

    bool is_outside(const Vector2<I>& v) const {
        bool any_clockwise = false;
        for(const Line<I>& edge: edges_) {
            const Orientation orientation = edge.orientation(v);
            if(orientation == Orientation::COLLINEAR) {
                return false;
            }
            any_clockwise |= orientation == Orientation::CLOCKWISE;
        }
        return any_clockwise;
    }

    friend std::ostream& operator<<(std::ostream& os, const Polygon& polygon) {
        for(const Line<I>& edge: polygon.edges_) {
            os << edge << std::endl;
        }
        return os;
    }
};
