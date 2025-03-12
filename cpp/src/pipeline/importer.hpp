#pragma once

#include "pipeline/config.hpp"
#include "pipeline/box.hpp"
#include <fstream>

template<IntervalType Interval>
struct Importer {
private:
    const Config<Interval>& config_;

    static size_t size_from_stream(std::istream& is) {
        size_t size;
        is.read(reinterpret_cast<char*>(&size), sizeof(size));
        return size;
    }

public:
    explicit Importer(const Config<Interval>& config) : config_(config) {}

    bool can_import() {
        const std::filesystem::path path = config_.boxes_path();
        if(!std::filesystem::exists(path)) {
            return false;
        }
        std::ifstream file(path, std::ios::binary);
        if(!file.is_open()) {
            return false;
        }
        return size_from_stream(file) > 0;
    }

    std::vector<Box> import_boxes() {
        const std::filesystem::path path = config_.boxes_path();
        std::ifstream file(path, std::ios::binary);
        if(!file.is_open()) {
            throw std::runtime_error("Failed to open " + path.string());
        }

        const size_t size = size_from_stream(file);
        std::vector<Box> boxes;
        std::ranges::generate_n(std::back_inserter(boxes), size, [&] {
            return Box::from_bytes(file);
        });

        if(file.fail()) {
            throw std::runtime_error("Failed to read from " + path.string());
        }
        std::cout << "Imported " << size << " boxes from " << path << std::endl;

        return boxes;
    }
};
