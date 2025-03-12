#pragma once

#include "geometry/polyhedron.hpp"
#include <cstdint>
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
    size_t export_size_threshold_;
    std::filesystem::path directory_;
    std::string terminal_boxes_filename_;
    std::string boxes_filename_;

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
        const size_t export_size_threshold,
        std::string directory,
        std::string terminal_boxes_filename = "terminal_boxes.bin",
        std::string boxes_filename = "boxes.bin"
    ) : description_(std::move(description)),
        hole_(hole),
        plug_(plug),
        thread_count_(thread_count),
        box_iteration_limit_(box_iteration_limit),
        rectangle_iteration_limit_(rectangle_iteration_limit),
        projection_resolution_(projection_resolution),
        rotation_resolution_(rotation_resolution),
        export_size_threshold_(export_size_threshold),
        directory_(std::move(directory)),
        terminal_boxes_filename_(std::move(terminal_boxes_filename)),
        boxes_filename_(std::move(boxes_filename)) {
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

    size_t export_size_threshold() const {
        return export_size_threshold_;
    }

    std::filesystem::path directory() const {
        return directory_;
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
};
