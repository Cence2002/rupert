#pragma once

#include "geometry/geometry.hpp"
#include <cstdint>
#include <filesystem>

template<IntervalType Interval>
struct Config {
private:
    Polyhedron<Interval> hole_;
    Polyhedron<Interval> plug_;
    uint8_t thread_count_;
    uint32_t hole_iteration_limit_;
    uint32_t plug_iteration_limit_;
    uint8_t projection_resolution_;
    uint8_t rotation_resolution_;
    std::filesystem::path path_;
};