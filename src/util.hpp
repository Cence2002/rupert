#pragma once

#include <iostream>
#include <chrono>
#include <boost/numeric/interval.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/adapted/boost_tuple.hpp>

inline void print() {
    std::cout << std::endl;
}

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

inline std::chrono::time_point<std::chrono::high_resolution_clock> current_time() {
    return std::chrono::high_resolution_clock::now();
}

inline std::chrono::duration<double>::rep elapsed_time(const std::chrono::time_point<std::chrono::high_resolution_clock> &start) {
    return std::chrono::duration<double>(current_time() - start).count();
}

inline std::chrono::duration<long>::rep elapsed_microseconds(const std::chrono::time_point<std::chrono::high_resolution_clock> &start) {
    return std::chrono::duration_cast<std::chrono::microseconds>(current_time() - start).count();
}

inline std::chrono::duration<long>::rep elapsed_milliseconds(const std::chrono::time_point<std::chrono::high_resolution_clock> &start) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(current_time() - start).count();
}

inline std::chrono::duration<long>::rep elapsed_seconds(const std::chrono::time_point<std::chrono::high_resolution_clock> &start) {
    return std::chrono::duration_cast<std::chrono::seconds>(current_time() - start).count();
}

const double pi_half_upper = boost::numeric::interval_lib::constants::pi_half_upper<double>();
const double pi_upper = boost::numeric::interval_lib::constants::pi_upper<double>();
const double pi_twice_upper = boost::numeric::interval_lib::constants::pi_twice_upper<double>();

BOOST_GEOMETRY_REGISTER_BOOST_TUPLE_CS(cs::cartesian)

inline std::vector<std::pair<double, double>> convex_hull(const std::vector<std::pair<double, double>> &vertices) {
    namespace bg = boost::geometry;
    using BoostPoint = boost::tuple<double, double>;
    using BoostPolygon = bg::model::polygon<BoostPoint>;
    BoostPolygon input_polygon;
    for(const auto &[x, y]: vertices) {
        bg::append(input_polygon.outer(), BoostPoint(x, y));
    }
    BoostPolygon hull_polygon;
    bg::convex_hull(input_polygon, hull_polygon);
    std::vector<std::pair<double, double>> hull_vertices;
    for(const auto &[x, y]: hull_polygon.outer()) {
        hull_vertices.emplace_back(x, y);
    }
    return hull_vertices;
}
