#include <iostream>
#include <optional>
#include <cmath>
#include <queue>
#include <mutex>
#include <algorithm>
#include <thread>
#include <chrono>
#include <random>
#include <boost/numeric/interval.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/adapted/boost_tuple.hpp>

constexpr double TWO_PI = 6.28318530717958647692528676655900576839433879875021164194;
constexpr double PI = 3.141592653589793238462643383279502884197169399375105820974944;
constexpr double HALF_PI = 1.5707963267948966192313216916397514420985846996875529104;
constexpr double QUARTER_PI = 0.7853981633974483096156608458198757210492923498437764;

template<typename T>
struct Vector2 {
    T x, y;

    friend std::ostream &operator<<(std::ostream &os, const Vector2 &v) {
        os << "(" << v.x << ", " << v.y << ")";
        return os;
    }

    Vector2 operator+() const {
        return {x, y};
    }

    Vector2 operator+(const T s) const {
        return {x + s, y + s};
    }

    Vector2 operator+(const Vector2 &v) const {
        return {x + v.x, y + v.y};
    }

    Vector2 operator-() const {
        return {-x, -y};
    }

    Vector2 operator-(const T s) const {
        return {x - s, y - s};
    }

    Vector2 operator-(const Vector2 &v) const {
        return {x - v.x, y - v.y};
    }

    Vector2 operator*(const T s) const {
        return {x * s, y * s};
    }

    Vector2 operator*(const Vector2 &v) const {
        return {x * v.x, y * v.y};
    }

    Vector2 operator/(const T s) const {
        return {x / s, y / s};
    }

    Vector2 operator/(const Vector2 &v) const {
        return {x / v.x, y / v.y};
    }

    T r() const {
        return sqrt(x * x + y * y);
    }

    Vector2 unit() const {
        return *this / r();
    }

    T dot(const Vector2 &v) const {
        return x * v.x + y * v.y;
    }

    T cross(const Vector2 &v) const {
        return x * v.y - y * v.x;
    }

    T angle() const {
        const T angle = atan2(y, x);
        return angle > 0 ? angle : angle + TWO_PI;
    }

    Vector2 rotate(const T angle) const {
        const T cos_angle = cos(angle);
        const T sin_angle = sin(angle);
        return {
                    x * cos_angle - y * sin_angle,
                    x * sin_angle + y * cos_angle
                };
    }
};

using Vector2d = Vector2<double>;

template<typename T>
struct Vector3 {
    T x, y, z;

    friend std::ostream &operator<<(std::ostream &os, const Vector3 &v) {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }

    Vector3 operator+(const T s) const {
        return {x + s, y + s, z + s};
    }

    Vector3 operator+(const Vector3 &v) const {
        return {x + v.x, y + v.y, z + v.z};
    }

    Vector3 operator-(const T s) const {
        return {x - s, y - s, z - s};
    }

    Vector3 operator-(const Vector3 &v) const {
        return {x - v.x, y - v.y, z - v.z};
    }

    Vector3 operator*(const T s) const {
        return {x * s, y * s, z * s};
    }

    Vector3 operator*(const Vector3 &v) const {
        return {x * v.x, y * v.y, z * v.z};
    }

    Vector3 operator/(const T s) const {
        return {x / s, y / s, z / s};
    }

    Vector3 operator/(const Vector3 &v) const {
        return {x / v.x, y / v.y, z / v.z};
    }

    T r() const {
        return sqrt(x * x + y * y + z * z);
    }

    Vector3 unit() const {
        return *this / r();
    }

    T dot(const Vector3 &v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    Vector3 cross(const Vector3 &v) const {
        return {
                    y * v.z - z * v.y,
                    z * v.x - x * v.z,
                    x * v.y - y * v.x
                };
    }

    Vector2<T> project(const T theta, const T phi) const {
        const T sin_theta = sin(theta);
        const T cos_theta = cos(theta);
        return {
                    -x * sin_theta + y * cos_theta,
                    (x * cos_theta + y * sin_theta) * cos(phi) - z * sin(phi)
                };
    }
};

using Vector3d = Vector3<double>;

struct Interval {
    double low;
    double up;

    Interval() : low(0.0), up(0.0) {}

    Interval(const double v) : low(v), up(v) {}

    Interval(const double low, const double up) : low(low), up(up) {}

    friend std::ostream &operator<<(std::ostream &os, const Interval &interval) {
        os << "[" << interval.low << ", " << interval.up << "]";
        return os;
    }

    Interval operator+() const {
        return {low, up};
    }

    Interval operator+(const double v) const {
        return {low + v, up + v};
    }

    Interval operator+(const Interval &interval) const {
        return {low + interval.low, up + interval.up};
    }

    Interval operator-() const {
        return {-up, -low};
    }

    Interval operator-(const double v) const {
        return {low - v, up - v};
    }

    Interval operator-(const Interval &interval) const {
        return {low - interval.up, up - interval.low};
    }

    Interval operator*(const double v) const {
        const double ll = low * v;
        const double uu = up * v;
        return {std::min(ll, uu), std::max(ll, uu)};
    }

    Interval operator*(const Interval &interval) const {
        const double ll = low * interval.low;
        const double lu = low * interval.up;
        const double ul = up * interval.low;
        const double uu = up * interval.up;
        return {std::min({ll, lu, ul, uu}), std::max({ll, lu, ul, uu})};
    }

    Interval operator/(const double v) const {
        if(v == 0.0) {
            return {
                        -std::numeric_limits<double>::infinity(),
                        std::numeric_limits<double>::infinity()
                    };
        }
        const double l = low / v;
        const double u = up / v;
        return {std::min(l, u), std::max(l, u)};
    }

    Interval operator/(const Interval &interval) const {
        if(interval.contains(0.0)) {
            return {
                        -std::numeric_limits<double>::infinity(),
                        std::numeric_limits<double>::infinity()
                    };
        }
        const double ll = low / interval.low;
        const double lu = low / interval.up;
        const double ul = up / interval.low;
        const double uu = up / interval.up;
        return {std::min({ll, lu, ul, uu}), std::max({ll, lu, ul, uu})};
    }

    double lower() const {
        return low;
    }

    double upper() const {
        return up;
    }

    double width() const {
        return up - low;
    }

    double median() const {
        return (low + up) / 2.0;
    }

    bool contains(const double v) const {
        return low <= v && v <= up;
    }

    std::vector<Interval> divide(const int parts) const {
        std::vector<double> cutoffs;
        cutoffs.push_back(low);
        const double step = width() / parts;
        for(int i = 1; i < parts; i++) {
            cutoffs.push_back(low + i * step);
        }
        cutoffs.push_back(up);
        std::vector<Interval> intervals;
        for(size_t i = 0; i < cutoffs.size() - 1; i++) {
            intervals.emplace_back(cutoffs[i], cutoffs[i + 1]);
        }
        return intervals;
    }
};

using Vector2I = Vector2<Interval>;
using Vector3I = Vector3<Interval>;

inline Interval cos(const Interval &interval) {
    const double lower = interval.lower();
    const double upper = interval.upper();
    const double cos_lower = cos(lower);
    const double cos_upper = cos(upper);
    double cos_min = std::min(cos_lower, cos_upper);
    double cos_max = std::max(cos_lower, cos_upper);
    if(std::floor((lower - PI) / TWO_PI) != std::floor((upper - PI) / TWO_PI)) {
        cos_min = -1.0;
    }
    if(std::floor(lower / TWO_PI) != std::floor(upper / TWO_PI)) {
        cos_max = 1.0;
    }
    return {cos_min, cos_max};
}

inline Interval sin(const Interval &interval) {
    const double lower = interval.lower();
    const double upper = interval.upper();
    const double sin_lower = sin(lower);
    const double sin_upper = sin(upper);
    double sin_min = std::min(sin_lower, sin_upper);
    double sin_max = std::max(sin_lower, sin_upper);
    if(std::floor((lower - HALF_PI) / TWO_PI) != std::floor((upper - HALF_PI) / TWO_PI)) {
        sin_max = 1.0;
    }
    if(std::floor((lower + HALF_PI) / TWO_PI) != std::floor((upper + HALF_PI) / TWO_PI)) {
        sin_min = -1.0;
    }
    return {sin_min, sin_max};
}

inline bool line_intersects_line(const Vector2d &a, const Vector2d &b, const Vector2d &c, const Vector2d &d) {
    const Vector2d ab = b - a;
    const Vector2d cd = d - c;
    const Vector2d ac = c - a;
    const Vector2d bd = d - b;
    return ab.cross(ac) * ab.cross(bd) < 0 && cd.cross(ac) * cd.cross(bd) < 0;
}

inline bool line_intersects_polygon(const Vector2d &a, const Vector2d &b, const std::vector<Vector2d> &vertices) {
    for(size_t i = 0; i < vertices.size(); i++) {
        if(line_intersects_line(a, b, vertices[i], vertices[(i + 1) % vertices.size()])) {
            return true;
        }
    }
    return false;
}

inline bool point_inside_polygon(const Vector2d &point, const std::vector<Vector2d> &vertices) {
    std::vector<double> angles;
    for(const Vector2d &vertex: vertices) {
        angles.push_back(vertex.angle());
    }
    const double angle = point.angle();
    const int upper_bound = static_cast<int>(std::ranges::upper_bound(angles, angle) - angles.begin());
    const int index_0 = upper_bound == 0 ? static_cast<int>(angles.size()) - 1 : upper_bound - 1;
    const int index_1 = upper_bound == static_cast<int>(angles.size()) ? 0 : upper_bound;
    const Vector2d d = vertices[index_1] - vertices[index_0];
    return d.cross(point - vertices[index_0]) > 0;
}

struct Box2 {
    inline static const Interval theta_interval{0, TWO_PI};
    inline static const Interval phi_interval{0, PI};

    Interval theta, phi;

    friend std::ostream &operator<<(std::ostream &os, const Box2 &box) {
        os << "[" << box.theta << ", " << box.phi << "]";
        return os;
    }

    Vector2d center() const {
        return {theta.median(), phi.median()};
    }

    std::vector<Vector2d> vertices() const {
        return {
                    {theta.lower(), phi.lower()},
                    {theta.lower(), phi.upper()},
                    {theta.upper(), phi.upper()},
                    {theta.upper(), phi.lower()}
                };
    }

    std::vector<Box2> sub_boxes() const {
        const auto [median_theta, median_phi] = center();
        return {
                    {{theta.lower(), median_theta}, {phi.lower(), median_phi}},
                    {{theta.lower(), median_theta}, {median_phi, phi.upper()}},
                    {{median_theta, theta.upper()}, {phi.lower(), median_phi}},
                    {{median_theta, theta.upper()}, {median_phi, phi.upper()}}
                };
    }
};

inline bool box_intersects_polygon(const Box2 &box, const Vector3d &point, const std::vector<Vector2d> &vertices) {
    const auto [x, y] = Vector3I(point.x, point.y, point.z).project(box.theta, box.phi);
    return point_inside_polygon({x.median(), y.median()}, vertices) ||
           line_intersects_polygon({x.lower(), y.lower()}, {x.lower(), y.upper()}, vertices) ||
           line_intersects_polygon({x.upper(), y.lower()}, {x.upper(), y.upper()}, vertices) ||
           line_intersects_polygon({x.lower(), y.lower()}, {x.upper(), y.lower()}, vertices) ||
           line_intersects_polygon({x.lower(), y.upper()}, {x.upper(), y.upper()}, vertices) ||
           x.contains(0.0) && y.contains(0.0);
}

struct Box3 {
    inline static const Interval theta_interval{0, TWO_PI};
    inline static const Interval phi_interval{0, PI};
    inline static const Interval alpha_interval{0, TWO_PI};

    Interval theta, phi, alpha;

    friend std::ostream &operator<<(std::ostream &os, const Box3 &box) {
        os << "[" << box.theta << ", " << box.phi << ", " << box.alpha << "]";
        return os;
    }

    std::vector<Box3> sub_boxes() const {
        const double theta_median = theta.median();
        const double phi_median = phi.median();
        const double alpha_median = alpha.median();
        return {
                    {{theta.lower(), theta_median}, {phi.lower(), phi_median}, {alpha.lower(), alpha_median}},
                    {{theta.lower(), theta_median}, {phi.lower(), phi_median}, {alpha_median, alpha.upper()}},
                    {{theta.lower(), theta_median}, {phi_median, phi.upper()}, {alpha.lower(), alpha_median}},
                    {{theta.lower(), theta_median}, {phi_median, phi.upper()}, {alpha_median, alpha.upper()}},
                    {{theta_median, theta.upper()}, {phi.lower(), phi_median}, {alpha.lower(), alpha_median}},
                    {{theta_median, theta.upper()}, {phi.lower(), phi_median}, {alpha_median, alpha.upper()}},
                    {{theta_median, theta.upper()}, {phi_median, phi.upper()}, {alpha.lower(), alpha_median}},
                    {{theta_median, theta.upper()}, {phi_median, phi.upper()}, {alpha_median, alpha.upper()}}
                };
    }
};

BOOST_GEOMETRY_REGISTER_BOOST_TUPLE_CS(cs::cartesian)

inline std::vector<Vector2d> convex_hull(const std::vector<Vector2d> &points) {
    namespace bg = boost::geometry;
    using BoostPoint = boost::tuple<double, double>;
    using BoostPolygon = bg::model::polygon<BoostPoint>;
    BoostPolygon input_polygon;
    for(const auto &[x, y]: points) {
        bg::append(input_polygon.outer(), BoostPoint(x, y));
    }
    BoostPolygon hull_polygon;
    bg::convex_hull(input_polygon, hull_polygon);
    std::vector<Vector2d> hull_points;
    for(const auto &[x, y]: hull_polygon.outer()) {
        hull_points.emplace_back(x, y);
    }
    return hull_points;
}

class Queue2 {
    std::queue<Box2> queue;

public:
    explicit Queue2(const int theta_parts = 1, const int phi_parts = 1) {
        for(const Interval &theta: Box2::theta_interval.divide(theta_parts)) {
            for(const Interval &phi: Box2::phi_interval.divide(phi_parts)) {
                queue.push({theta, phi});
            }
        }
    }

    bool empty() const {
        return queue.empty();
    }

    void process(const std::vector<Vector3d> &polyhedron, const std::vector<Vector2d> &hole) {
        if(queue.empty()) {
            return;
        }
        const Box2 box = queue.front();
        queue.pop();
        for(const Vector3d &vertex: polyhedron) {
            if(!box_intersects_polygon(box, vertex, hole)) {
                return;
            }
        }
        for(const Box2 &sub_box: box.sub_boxes()) {
            queue.push(sub_box);
        }
    }

    bool check(const std::vector<Vector3d> &polyhedron, const std::vector<Vector2d> &hole) const {
        const auto [theta, phi] = queue.front();
        return std::ranges::all_of(polyhedron, [&](const Vector3d &vertex) {
            return point_inside_polygon(vertex.project(theta.median(), phi.median()), hole);
        });
    }
};

class Queue3 {
    std::queue<Box3> queue;
    std::mutex mutex;

public:
    explicit Queue3(const int theta_parts = 1, const int phi_parts = 1, const int alpha_parts = 1) {
        for(const Interval &theta: Box3::theta_interval.divide(theta_parts)) {
            for(const Interval &phi: Box3::phi_interval.divide(phi_parts)) {
                for(const Interval &alpha: Box3::alpha_interval.divide(alpha_parts)) {
                    queue.emplace(theta, phi, alpha);
                }
            }
        }
    }

    void push(const Box3 &box) {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(box);
    }

    std::optional<Box3> pop() {
        std::lock_guard<std::mutex> lock(mutex);
        if(queue.empty()) {
            return std::nullopt;
        }
        const Box3 box = queue.front();
        queue.pop();
        return std::make_optional(box);
    }

    void process(const std::vector<Vector3d> &polyhedron, const int resolution = 2, const int iterations = 10000, const int frequency = 10) {
        const std::optional<Box3> optional_box = pop();
        if(!optional_box.has_value()) {
            return;
        }
        const Box3 &box = optional_box.value();
        std::vector<Vector2d> hole_all;
        for(const Interval &theta: box.theta.divide(resolution)) {
            for(const Interval &phi: box.phi.divide(resolution)) {
                for(const Interval &alpha: box.alpha.divide(resolution)) {
                    for(const auto &[x, y, z]: polyhedron) {
                        const auto [x_interval, y_interval] = Vector3I(x, y, z).project(theta, phi).rotate(alpha);
                        hole_all.emplace_back(x_interval.lower(), y_interval.lower());
                        hole_all.emplace_back(x_interval.lower(), y_interval.upper());
                        hole_all.emplace_back(x_interval.upper(), y_interval.lower());
                        hole_all.emplace_back(x_interval.upper(), y_interval.upper());
                    }
                }
            }
        }
        std::vector<Vector2d> hole = convex_hull(hole_all);
        std::ranges::sort(hole, [](const Vector2d &vertex_0, const Vector2d &vertex_1) {
            return vertex_0.angle() < vertex_1.angle();
        });

        Queue2 queue2;
        for(int i = 0; i < iterations; i++) {
            if(iterations % frequency == 0) {
                if(queue2.check(polyhedron, hole)) {
                    break;
                }
            }
            queue2.process(polyhedron, hole);
        }

        if(!queue2.empty()) {
            for(const Box3 &sub_box: box.sub_boxes()) {
                push(sub_box);
            }
        } else {
            std::cout << box << std::endl;
        }
    }
};

struct Polyhedron {
    static std::vector<Vector3d> normalised(const std::vector<Vector3d> &vertices) {
        std::vector<Vector3d> normalised_vertices;
        for(const Vector3d &vertex: vertices) {
            normalised_vertices.push_back(vertex.unit());
        }
        return normalised_vertices;
    }

    static std::vector<Vector3d> permutations(const std::vector<Vector3d> &vertices) {
        std::vector<Vector3d> permutations;
        for(const auto &[x, y, z]: vertices) {
            permutations.push_back({x, y, z});
            permutations.push_back({z, x, y});
            permutations.push_back({y, z, x});
            permutations.push_back({x, z, y});
            permutations.push_back({y, x, z});
            permutations.push_back({z, y, x});
        }
        return permutations;
    }

    static std::vector<Vector3d> rotations(const std::vector<Vector3d> &vertices) {
        std::vector<Vector3d> rotations;
        for(const auto &[x, y, z]: vertices) {
            rotations.push_back({x, y, z});
            rotations.push_back({z, x, y});
            rotations.push_back({y, z, x});
        }
        return rotations;
    }

    static std::vector<Vector3d> flips(const std::vector<Vector3d> &vertices, bool flip_x, bool flip_y, bool flip_z) {
        std::vector<double> signs_x{1};
        if(flip_x) {
            signs_x.push_back(-1);
        }
        std::vector<double> signs_y{1};
        if(flip_y) {
            signs_y.push_back(-1);
        }
        std::vector<double> signs_z{1};
        if(flip_z) {
            signs_z.push_back(-1);
        }
        std::vector<Vector3d> flips;
        for(const double sign_x: signs_x) {
            for(const double sign_y: signs_y) {
                for(const double sign_z: signs_z) {
                    Vector3d signs(sign_x, sign_y, sign_z);
                    for(const Vector3d &vertex: vertices) {
                        flips.push_back(vertex * signs);
                    }
                }
            }
        }
        return flips;
    }

    static std::vector<Vector3d> combine(const std::vector<std::vector<Vector3d>> &vertices) {
        std::vector<Vector3d> combine;
        for(const std::vector<Vector3d> &vertex: vertices) {
            combine.insert(combine.end(), vertex.begin(), vertex.end());
        }
        return combine;
    }

    static std::vector<Vector3d> octahedron() {
        return normalised(rotations(flips({{1, 0, 0}}, true, false, false)));
    }

    static std::vector<Vector3d> cube() {
        return normalised(flips({{1, 1, 1}}, true, true, true));
    }

    static std::vector<Vector3d> icosahedron() {
        const double phi = (1 + std::sqrt(5)) / 2;
        return normalised(rotations(flips({{1, phi, 0}}, true, true, false)));
    }

    static std::vector<Vector3d> dodecahedron() {
        const double phi = (1 + std::sqrt(5)) / 2;
        const double phi_reciprocal = 2 / (1 + std::sqrt(5));
        return normalised(combine({
                flips({{1, 1, 1}}, true, true, true),
                rotations(flips({{phi, phi_reciprocal, 0}}, true, true, false))
            }));
    }

    static std::vector<Vector3d> rhombicosidodecahedron() {
        const double phi = (1 + std::sqrt(5)) / 2;
        const double phi_square = (3 + std::sqrt(5)) / 2;
        const double phi_cube = 2 + std::sqrt(5);
        const double phi_double = 1 + 2 * std::sqrt(5);
        const double phi_plus_two = (5 + std::sqrt(5)) / 2;
        return normalised(rotations(combine({
                flips({{1, 1, phi_cube}}, true, true, true),
                flips({{phi_square, phi, phi_double}}, true, true, true),
                flips({{phi_plus_two, 0, phi_square}}, true, false, true),
            })));
    }
};

using BoostInterval = boost::numeric::interval<
    double,
    boost::numeric::interval_lib::policies<
        boost::numeric::interval_lib::save_state<
            boost::numeric::interval_lib::rounded_transc_std<double>
        >,
        boost::numeric::interval_lib::checking_base<double>
    >
>;

class RandomNumberGenerator {
    static constexpr uint32_t default_seed = 42;
    std::mt19937_64 engine;

public:
    RandomNumberGenerator() : engine(default_seed) {}

    explicit RandomNumberGenerator(const std::mt19937_64::result_type seed) : engine(seed) {}

    void seed(const std::mt19937_64::result_type seed) {
        engine.seed(seed);
    }

    double uniform(const double min, const double max) {
        std::uniform_real_distribution<double> distribution(min, max);
        return distribution(engine);
    }
};

inline bool is_close(const double value, const double target_value, const double absolute_tolerance = 1e-6, const double relative_tolerance = 1e-6) {
    return std::abs(value - target_value) < absolute_tolerance + relative_tolerance * std::abs(target_value);
}

inline bool interval_is_close(const Interval &interval, const BoostInterval &target_interval, const double absolute_tolerance = 1e-6, const double relative_tolerance = 1e-6) {
    return is_close(interval.lower(), target_interval.lower(), absolute_tolerance, relative_tolerance) &&
           is_close(interval.upper(), target_interval.upper(), absolute_tolerance, relative_tolerance);
}

inline std::ostream &operator<<(std::ostream &os, const BoostInterval &interval) {
    os << "[" << interval.lower() << ", " << interval.upper() << "]";
    return os;
}

void test_intervals() {
    RandomNumberGenerator rng;

    for(int i = 0; i < 1000000; ++i) {
        constexpr double absolute_tolerance = 1e-6;
        constexpr double relative_tolerance = 1e-6;
        double l1 = rng.uniform(-3, 3);
        double u1 = rng.uniform(-3, 3);
        if(l1 > u1) std::swap(l1, u1);

        double l2 = rng.uniform(-3, 3);
        double u2 = rng.uniform(-3, 3);
        if(l2 > u2) std::swap(l2, u2);

        BoostInterval b1(l1, u1);
        BoostInterval b2(l2, u2);
        Interval c1(l1, u1);
        Interval c2(l2, u2);

        assert(interval_is_close(c1 + c2, b1 + b2, absolute_tolerance, relative_tolerance));
        assert(interval_is_close(c1 - c2, b1 - b2, absolute_tolerance, relative_tolerance));
        assert(interval_is_close(c1 * c2, b1 * b2, absolute_tolerance, relative_tolerance));
        if(!c2.contains(0.0)) {
            assert(interval_is_close(c1 / c2, b1 / b2, absolute_tolerance, relative_tolerance));
        }
        if(!interval_is_close(cos(c1), cos(b1), absolute_tolerance, relative_tolerance)) {
            std::cout << "sin(" << c1 << ") => " << cos(c1) << " != " << cos(b1) << std::endl;
            assert(false);
        }
        if(!interval_is_close(sin(c1), sin(b1), absolute_tolerance, relative_tolerance)) {
            std::cout << "sin(" << c1 << ") => " << sin(c1) << " != " << sin(b1) << std::endl;
            assert(false);
        }
    }

    std::cout << "All tests passed!\n";
}

template<typename T, typename... Args>
void print(T &&first, Args &&... args) {
    std::cout << first;
    if constexpr(sizeof...(args) > 0) {
        std::cout << " ";
        print(std::forward<Args>(args)...);
    } else {
        std::cout << std::endl;
    }
}

int main() {
    const std::vector<Vector3d> polyhedron = Polyhedron::rhombicosidodecahedron();

    Queue3 queue3;
    std::vector<std::thread> threads;
    for(int i = 0; i < 20; i++) {
        threads.emplace_back([&] {
            while(true) {
                queue3.process(polyhedron, 5, 10000, 10);
            }
        });
    }
    for(std::thread &thread: threads) {
        thread.join();
    }

    return 0;
}
