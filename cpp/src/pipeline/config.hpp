#pragma once

#include "geometry/geometry.hpp"
#include <cstdint>
#include <filesystem>
#include <utility>

template<IntervalType Interval>
struct Config {
private:
    std::string description_;
    Polyhedron<Interval> hole_;
    Polyhedron<Interval> plug_;
    uint8_t thread_count_;
    uint32_t hole_iteration_limit_;
    uint32_t plug_iteration_limit_;
    uint8_t projection_resolution_;
    uint8_t rotation_resolution_;
    std::filesystem::path path_;

public:
    Config(
        std::string description,
        const Polyhedron<Interval>& hole,
        const Polyhedron<Interval>& plug,
        const uint8_t thread_count,
        const uint32_t hole_iteration_limit,
        const uint32_t plug_iteration_limit,
        const uint8_t projection_resolution,
        const uint8_t rotation_resolution,
        std::filesystem::path path
    ) : description_(std::move(description)),
        hole_(hole),
        plug_(plug),
        thread_count_(thread_count),
        hole_iteration_limit_(hole_iteration_limit),
        plug_iteration_limit_(plug_iteration_limit),
        projection_resolution_(projection_resolution),
        rotation_resolution_(rotation_resolution),
        path_(std::move(path)) {}

    std::string description() const {
        return description_;
    }

    const Polyhedron<Interval>& hole() const {
        return hole_;
    }

    const Polyhedron<Interval>& plug() const {
        return plug_;
    }

    uint8_t thread_count() const {
        return thread_count_;
    }

    uint32_t hole_iteration_limit() const {
        return hole_iteration_limit_;
    }

    uint32_t plug_iteration_limit() const {
        return plug_iteration_limit_;
    }

    uint8_t projection_resolution() const {
        return projection_resolution_;
    }

    uint8_t rotation_resolution() const {
        return rotation_resolution_;
    }

    std::filesystem::path path() const {
        return path_;
    }

    std::filesystem::path path(const std::string& suffix) const {
        return path_ / (description_ + "_" + suffix);
    }
};
