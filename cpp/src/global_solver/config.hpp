#pragma once

#include "geometry/geometry.hpp"
#include "geometry/polyhedron.hpp"
#include "queue/queue.hpp"
#include "range/range2.hpp"
#include "range/range3.hpp"
#include <cstdint>
#include <regex>
#include <filesystem>

template<IntervalType Interval>
struct Config {
    // main parameters
    Polyhedron<Interval> polyhedron;
    Interval epsilon;

    // mathematical hyperparameters
    uint32_t box_iteration_limit;
    uint32_t range2_iteration_limit;
    uint8_t projection_resolution;
    uint8_t rotation_resolution;

    // execution parameters
    uint8_t thread_count;
    std::filesystem::path root_directory;
    std::string name;
    size_t export_size_threshold;
    bool restart;
    bool debug;

    void validate() const {
        if(!std::regex_match(name, std::regex("^[a-zA-Z0-9_]+$"))) {
            throw std::runtime_error(name + " is not a valid name (only letters, digits, and underscores are allowed)");
        }
        if(!std::filesystem::exists(root_directory)) {
            throw std::runtime_error(root_directory.string() + " does not exist");
        }
        if(!std::filesystem::is_directory(root_directory)) {
            throw std::runtime_error(root_directory.string() + " is not a directory");
        }
    }

    std::filesystem::path working_directory() const {
        return root_directory / name;
    }
};
