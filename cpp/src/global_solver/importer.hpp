#pragma once

#include "pipeline/config.hpp"
#include <fstream>

template<IntervalType Interval>
struct Importer {
private:
    const Config<Interval>& config_;

    static uint32_t size_from_stream(std::istream& is) {
        uint32_t size;
        is.read(reinterpret_cast<char*>(&size), sizeof(size));
        return size;
    }

    static Range id_from_stream(std::istream& is) {
        uint16_t packed;
        is.read(reinterpret_cast<char*>(&packed), sizeof(packed));
        return Range::unpack(packed);
    }

    static Range3 box_from_stream(std::istream& is) {
        const Range theta_id = id_from_stream(is);
        const Range phi_id = id_from_stream(is);
        const Range alpha_id = id_from_stream(is);
        return Range3(theta_id, phi_id, alpha_id);
    }

public:
    explicit Importer(const Config<Interval>& config) : config_(config) {}

    void restart() {
        const std::filesystem::path directory_path = config_.directory_path();
        if(std::filesystem::exists(directory_path)) {
            std::filesystem::remove_all(directory_path);
        }
    }

    std::vector<Range3> import_boxes() {
        const std::filesystem::path path = config_.boxes_path();
        std::ifstream file(path, std::ios::binary);
        if(!file.is_open()) {
            return {};
        }

        const uint32_t size = size_from_stream(file);
        std::vector<Range3> boxes;
        boxes.reserve(size);
        std::ranges::generate_n(std::back_inserter(boxes), size, [&] {
            return box_from_stream(file);
        });

        if(file.fail()) {
            throw std::runtime_error("Failed to read from " + path.string());
        }
        std::cout << "Imported " << size << " boxes from " << path << std::endl;

        return boxes;
    }
};
