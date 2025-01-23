#include "interval.hpp"
#include "vector.hpp"
#include "cover.hpp"
#include "polyhedron.hpp"
#include "util.hpp"

#include <thread>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/adapted/boost_tuple.hpp>

BOOST_GEOMETRY_REGISTER_BOOST_TUPLE_CS(cs::cartesian)

inline std::vector<Vector2<PointInterval>> convex_hull(const std::vector<Vector2<PointInterval>> &points) {
    namespace bg = boost::geometry;
    using BoostPoint = boost::tuple<double, double>;
    using BoostPolygon = bg::model::polygon<BoostPoint>;
    BoostPolygon input_polygon;
    for(const auto &[x, y]: points) {
        bg::append(input_polygon.outer(), BoostPoint(x.min(), y.min()));
    }
    BoostPolygon hull_polygon;
    bg::convex_hull(input_polygon, hull_polygon);
    std::vector<Vector2<PointInterval>> hull_points;
    for(const auto &[x, y]: hull_polygon.outer()) {
        hull_points.emplace_back(PointInterval(x), PointInterval(y));
    }
    return hull_points;
}

void process2(Queue2 &queue, const std::vector<Vector3<PointInterval>> &polyhedron, const std::vector<Vector2<BoostInterval>> &hole) {
    const std::optional<Box2> optional_box = queue.pop();
    if(!optional_box.has_value()) {
        return;
    }
    const Box2 &box = optional_box.value();
    for(const Vector3<PointInterval> &vertex: polyhedron) {
        // const Vector2<BoostInterval> projection = Vector3<BoostInterval>(point.x, point.y, point.z).project(box.theta, box.phi);
        const Vector3<BoostInterval> boost_vertex = Vector3(BoostInterval(vertex.x.min()), BoostInterval(vertex.y.min()), BoostInterval(vertex.z.min()));
        const Vector2<BoostInterval> projection = boost_vertex.project(BoostInterval(box.theta.min, box.theta.max), BoostInterval(box.phi.min, box.phi.max));
        if(!projection.intersects_polygon(hole)) {
            return;
        }
    }
    for(const Box2 &sub_box: box.split()) {
        queue.push(sub_box);
    }
}

int main() {
    static_assert(IntervalType<PointInterval>);
    static_assert(IntervalType<BoostInterval>);
    static_assert(Vector2Type<Vector2<BoostInterval>, BoostInterval>);
    static_assert(Vector3Type<Vector3<BoostInterval>, BoostInterval>);

    print(Line2<BoostInterval>(Vector2<BoostInterval>(-1, 0), Vector2<BoostInterval>(1, 0)).intersects(
        Line2<BoostInterval>(Vector2<BoostInterval>(0, 0), Vector2<BoostInterval>(0, 1))));

    print(Line2<BoostInterval>(Vector2<BoostInterval>(-1, 0), Vector2<BoostInterval>(1, 0)).avoids(
        Line2<BoostInterval>(Vector2<BoostInterval>(0, 0), Vector2<BoostInterval>(0, 1))));

    print(Line2<BoostInterval>(Vector2<BoostInterval>(-1, 0), Vector2<BoostInterval>(1, 0)).intersects(
        Line2<BoostInterval>(Vector2<BoostInterval>(0, -1e-9), Vector2<BoostInterval>(0, 1))));

    print(Line2<BoostInterval>(Vector2<BoostInterval>(-1, 0), Vector2<BoostInterval>(1, 0)).avoids(
        Line2<BoostInterval>(Vector2<BoostInterval>(0, -1e-9), Vector2<BoostInterval>(0, 1))));

    print(Line2<BoostInterval>(Vector2<BoostInterval>(-1, 0), Vector2<BoostInterval>(1, 0)).intersects(
        Line2<BoostInterval>(Vector2<BoostInterval>(0, 1e-9), Vector2<BoostInterval>(0, 1))));

    print(Line2<BoostInterval>(Vector2<BoostInterval>(-1, 0), Vector2<BoostInterval>(1, 0)).avoids(
        Line2<BoostInterval>(Vector2<BoostInterval>(0, 1e-9), Vector2<BoostInterval>(0, 1))));

    int n = 10000;
    Queue3 queue;
    auto start = current_time();
    for(int i = 0; i < n; i++) {
        queue.pop();
        std::this_thread::sleep_for(std::chrono::microseconds(20));
        queue.push(Box3::init());
        queue.push(Box3::init());
    }
    print(elapsed_milliseconds(start), "ms");

    Queue3 queue3;
    start = current_time();
    std::vector<std::thread> threads;
    for(int i = 0; i < 10; i++) {
        threads.emplace_back([&queue3, n] {
            for(int i = 0; i < n / 10; i++) {
                queue3.pop();
                std::this_thread::sleep_for(std::chrono::microseconds(10));
                queue3.push(Box3::init());
                queue3.push(Box3::init());
            }
        });
    }
    for(auto &thread: threads) {
        thread.join();
    }
    print(elapsed_milliseconds(start), "ms");

    auto poly = Polyhedron<BoostInterval>::rhombicosidodecahedron();

    return 0;
}
