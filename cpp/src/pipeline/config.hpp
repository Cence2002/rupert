#pragma once

#include "geometry/geometry.hpp"
#include "space/space.hpp"
#include <cstdint>
#include <regex>
#include <filesystem>

template<IntervalType Interval>
struct Config {
private:
    std::string description_;

    Polyhedron<Interval> hole_;
    Polyhedron<Interval> plug_;

    uint8_t thread_count_;
    uint32_t box_iteration_limit_;
    uint32_t rectangle_iteration_limit_;
    uint8_t projection_resolution_;
    uint8_t rotation_resolution_;
    bool restart_;

    std::filesystem::path directory_;
    size_t export_size_threshold_;
    bool debug_enabled_;

    std::string terminal_boxes_filename_;
    std::string boxes_filename_;
    std::string debug_filename_;

public:
    Config(
        std::string description,
        const Polyhedron<Interval>& hole,
        const Polyhedron<Interval>& plug,
        const uint8_t thread_count,
        const uint32_t box_iteration_limit,
        const uint32_t rectangle_iteration_limit,
        const uint8_t projection_resolution,
        const uint8_t rotation_resolution,
        const bool restart,
        std::string directory,
        const size_t export_size_threshold,
        const bool debug_enabled = false,
        std::string terminal_boxes_filename = "terminal_boxes.bin",
        std::string boxes_filename = "boxes.bin",
        std::string debug_filename = "debug.bin"
    ) : description_(std::move(description)),
        hole_(hole),
        plug_(plug),
        thread_count_(thread_count),
        box_iteration_limit_(box_iteration_limit),
        rectangle_iteration_limit_(rectangle_iteration_limit),
        projection_resolution_(projection_resolution),
        rotation_resolution_(rotation_resolution),
        restart_(restart),
        directory_(std::move(directory)),
        export_size_threshold_(export_size_threshold),
        debug_enabled_(debug_enabled),
        terminal_boxes_filename_(std::move(terminal_boxes_filename)),
        boxes_filename_(std::move(boxes_filename)),
        debug_filename_(std::move(debug_filename)) {
        if(!std::regex_match(description_, std::regex("^[a-zA-Z0-9_]+$"))) {
            throw std::runtime_error(description_ + " is not a valid description (only letters, digits, and underscores are allowed)");
        }
        if(!std::filesystem::exists(directory_)) {
            throw std::runtime_error(directory_.string() + " does not exist");
        }
        if(!std::filesystem::is_directory(directory_)) {
            throw std::runtime_error(directory_.string() + " is not a directory");
        }
    }

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

    uint32_t box_iteration_limit() const {
        return box_iteration_limit_;
    }

    uint32_t rectangle_iteration_limit() const {
        return rectangle_iteration_limit_;
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

    std::filesystem::path path(const std::string& suffix) const {
        return directory_ / (description_ + "_" + suffix);
    }

    std::filesystem::path terminal_boxes_path() const {
        return path(terminal_boxes_filename_);
    }

    std::filesystem::path boxes_path() const {
        return path(boxes_filename_);
    }

    std::filesystem::path debug_path() const {
        if(!debug_enabled_) {
            throw std::runtime_error("Debug is disabled");
        }
        return path(debug_filename_);
    }

    bool debug_enabled() const {
        return debug_enabled_;
    }
};
