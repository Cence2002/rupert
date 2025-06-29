#pragma once

#include "geometry/geometry.hpp"
#include "geometry/polyhedron.hpp"
#include "pipeline/queue.hpp"
#include "range/range2.hpp"
#include "range/range3.hpp"
#include <cstdint>
#include <regex>
#include <filesystem>

template<IntervalType Interval>
struct Config {
private:
    std::string run_name_;

    Polyhedron<Interval> hole_;
    Polyhedron<Interval> plug_;
    Interval epsilon_;

    uint8_t thread_count_;
    uint32_t box_iteration_limit_;
    uint32_t range2_iteration_limit_;
    uint8_t projection_resolution_;
    uint8_t rotation_resolution_;
    bool restart_;

    std::filesystem::path directory_;
    size_t export_size_threshold_;
    bool debug_enabled_;

public:
    Config(
        std::string run_name,
        const Polyhedron<Interval>& hole,
        const Polyhedron<Interval>& plug,
        const Interval& epsilon,
        const uint8_t thread_count,
        const uint32_t box_iteration_limit,
        const uint32_t range2_iteration_limit,
        const uint8_t projection_resolution,
        const uint8_t rotation_resolution,
        const bool restart,
        std::string directory,
        const size_t export_size_threshold,
        const bool debug_enabled = false
    ) : run_name_(std::move(run_name)),
        hole_(hole),
        plug_(plug),
        epsilon_(epsilon),
        thread_count_(thread_count),
        box_iteration_limit_(box_iteration_limit),
        range2_iteration_limit_(range2_iteration_limit),
        projection_resolution_(projection_resolution),
        rotation_resolution_(rotation_resolution),
        restart_(restart),
        directory_(std::move(directory)),
        export_size_threshold_(export_size_threshold),
        debug_enabled_(debug_enabled) {
        if(!std::regex_match(run_name_, std::regex("^[a-zA-Z0-9_]+$"))) {
            throw std::runtime_error(run_name_ + " is not a valid name (only letters, digits, and underscores are allowed)");
        }
        if(!std::filesystem::exists(directory_)) {
            throw std::runtime_error(directory_.string() + " does not exist");
        }
        if(!std::filesystem::is_directory(directory_)) {
            throw std::runtime_error(directory_.string() + " is not a directory");
        }
    }

    std::string run_name() const {
        return run_name_;
    }

    const Polyhedron<Interval>& hole() const {
        return hole_;
    }

    const Polyhedron<Interval>& plug() const {
        return plug_;
    }

    Interval epsilon() const {
        return epsilon_;
    }

    uint8_t thread_count() const {
        return thread_count_;
    }

    uint32_t box_iteration_limit() const {
        return box_iteration_limit_;
    }

    uint32_t range2_iteration_limit() const {
        return range2_iteration_limit_;
    }

    uint8_t projection_resolution() const {
        return projection_resolution_;
    }

    uint8_t rotation_resolution() const {
        return rotation_resolution_;
    }

    bool restart() const {
        return restart_;
    }

    std::filesystem::path directory() const {
        return directory_;
    }

    size_t export_size_threshold() const {
        return export_size_threshold_;
    }

    bool debug_enabled() const {
        return debug_enabled_;
    }

    std::filesystem::path path(const std::string& file_name) const {
        return directory_ / run_name_ / file_name;
    }

    std::filesystem::path directory_path() const {
        return directory_ / run_name_;
    }

    std::filesystem::path polyhedra_path() const {
        return path("polyhedra.bin");
    }

    std::filesystem::path terminal_boxes_path() const {
        return path("terminal_boxes.bin");
    }

    std::filesystem::path boxes_path() const {
        return path("boxes.bin");
    }

    std::filesystem::path debug_path() const {
        return path("debug.bin");
    }
};
