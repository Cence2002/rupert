#include <iomanip>

#include "interval.hpp"
#include "vector.hpp"
#include "cover.hpp"
#include "polyhedron.hpp"
#include <iostream>

template<typename T, typename... Args>
void print(T t, Args... args) {
    std::cout << t;
    if constexpr(sizeof...(args) > 0) {
        std::cout << " ";
        print(args...);
    } else {
        std::cout << std::endl;
    }
}

int main() {
    static_assert(IntervalType<PointInterval>);
    static_assert(IntervalType<BoostInterval>);
    static_assert(Vector2Type<Vector2<BoostInterval>, BoostInterval>);
    static_assert(Vector3Type<Vector3<BoostInterval>, BoostInterval>);

    print(Vector2<BoostInterval>::line_intersects_line(
        Vector2<BoostInterval>(BoostInterval(-1), BoostInterval(0)),
        Vector2<BoostInterval>(BoostInterval(1), BoostInterval(0)),
        Vector2<BoostInterval>(BoostInterval(0), BoostInterval(0)),
        Vector2<BoostInterval>(BoostInterval(0), BoostInterval(1))));

    print(Vector2<BoostInterval>::line_avoids_line(
        Vector2<BoostInterval>(BoostInterval(-1), BoostInterval(0)),
        Vector2<BoostInterval>(BoostInterval(1), BoostInterval(0)),
        Vector2<BoostInterval>(BoostInterval(0), BoostInterval(0)),
        Vector2<BoostInterval>(BoostInterval(0), BoostInterval(1))));

    print(Vector2<BoostInterval>::line_intersects_line(
        Vector2<BoostInterval>(BoostInterval(-1), BoostInterval(0)),
        Vector2<BoostInterval>(BoostInterval(1), BoostInterval(0)),
        Vector2<BoostInterval>(BoostInterval(0), BoostInterval(-1e-20)),
        Vector2<BoostInterval>(BoostInterval(0), BoostInterval(1))));

    print(Vector2<BoostInterval>::line_avoids_line(
        Vector2<BoostInterval>(BoostInterval(-1), BoostInterval(0)),
        Vector2<BoostInterval>(BoostInterval(1), BoostInterval(0)),
        Vector2<BoostInterval>(BoostInterval(0), BoostInterval(1e-20)),
        Vector2<BoostInterval>(BoostInterval(0), BoostInterval(1))));

    return 0;
}
