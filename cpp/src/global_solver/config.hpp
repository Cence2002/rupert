#pragma once

#include "geometry/polyhedron.hpp"
#include <regex>
#include <filesystem>

template<IntervalType Interval>
class Config {
    Polyhedron<Interval> polyhedron;
    Interval epsilon;
    uint8_t projection_resolution = 1;
    uint8_t rotation_resolution = 1;

    // execution parameters
    uint8_t threads = 1;

    // io parameters
    std::filesystem::path root_directory;
    std::string name;
    size_t export_threshold = 0;
    bool restart = true;
    bool debug = true;

    void validate() const {
        if(epsilon.min().neg()) {
            throw std::runtime_error("Epsilon must be non-negative");
        }
        if(projection_resolution < 1) {
            throw std::runtime_error("Projection resolution must be at least 1");
        }
        if(rotation_resolution < 1) {
            throw std::runtime_error("Rotation resolution must be at least 1");
        }
        if(threads < 1) {
            throw std::runtime_error("Threads must be at least 1");
        }
        if(!std::filesystem::exists(root_directory)) {
            throw std::runtime_error(root_directory.string() + " does not exist");
        }
        if(!std::filesystem::is_directory(root_directory)) {
            throw std::runtime_error(root_directory.string() + " is not a directory");
        }
        if(!std::regex_match(name, std::regex("^[a-zA-Z0-9_]+$"))) {
            throw std::runtime_error(name + " is not a valid name (only letters, digits, and underscores are allowed)");
        }
        if(debug && threads > 1) {
            throw std::runtime_error("Debug mode is not compatible with multiple threads");
        }
    }

    std::filesystem::path working_directory() const {
        return root_directory / name;
    }
};
