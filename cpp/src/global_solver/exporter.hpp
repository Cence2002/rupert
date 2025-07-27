#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#include "flatbuffers/flatbuffers_generated.h"
#pragma GCC diagnostic pop

#include "global_solver/helpers.hpp"
#include "box/range.hpp"
#include <fstream>
#include <filesystem>

struct CombinedBoxes {
    Box3 hole_box;
    std::vector<Box2> plug_boxes;
};

namespace Exporter {
    inline void size_to_stream(std::ostream& os, const uint32_t size) {
        os.write(reinterpret_cast<const char*>(&size), sizeof(size));
    }

    inline void float_to_stream(std::ostream& os, const double value) {
        os.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }

    template<IntervalType Interval>
    void polyhedron_to_stream(std::ostream& os, const Polyhedron<Interval>& polyhedron) {
        size_to_stream(os, static_cast<uint32_t>(polyhedron.vertices().size()));
        for(const Vector3<Interval>& vertex: polyhedron.vertices()) {
            float_to_stream(os, vertex.x().to_float());
            float_to_stream(os, vertex.y().to_float());
            float_to_stream(os, vertex.z().to_float());
        }
    }

    inline void range_to_stream(std::ostream& os, const Range& range) {
        const uint32_t packed = range.pack();
        os.write(reinterpret_cast<const char*>(&packed), sizeof(packed));
    }

    inline void box2_to_stream(std::ostream& os, const Box2& box) {
        range_to_stream(os, Angle::theta_range(box));
        range_to_stream(os, Angle::phi_range(box));
    }

    inline void box3_to_stream(std::ostream& os, const Box3& box) {
        range_to_stream(os, Angle::theta_range(box));
        range_to_stream(os, Angle::phi_range(box));
        range_to_stream(os, Angle::alpha_range(box));
    }

    inline void combined_box_to_stream(std::ostream& os, const CombinedBoxes& combined_box) {
        box3_to_stream(os, combined_box.hole_box);
        size_to_stream(os, static_cast<uint32_t>(combined_box.plug_boxes.size()));
        for(const Box2& box: combined_box.plug_boxes) {
            box2_to_stream(os, box);
        }
    }

    inline void create_empty_working_directory(const std::filesystem::path& working_directory) {
        if(std::filesystem::exists(working_directory)) {
            std::filesystem::remove_all(working_directory);
        }
        std::filesystem::create_directory(working_directory);
    }

    template<IntervalType Interval>
    void export_polyhedron(const std::filesystem::path& path, const Polyhedron<Interval>& polyhedron) {
        std::ofstream file(path, std::ios::binary | std::ios::trunc);
        if(!file.is_open()) {
            throw std::runtime_error("Failed to open " + path.string());
        }

        polyhedron_to_stream(file, polyhedron);

        if(file.fail()) {
            throw std::runtime_error("Failed to write to " + path.string());
        }
        std::cout << "Exported polyhedra to " << path << std::endl;
    }

    inline void export_combined_boxes(const std::filesystem::path& path, const std::vector<CombinedBoxes>& combined_boxes) {
        std::ofstream file(path, std::ios::binary | std::ios::app);
        if(!file.is_open()) {
            throw std::runtime_error("Failed to open " + path.string());
        }

        for(const CombinedBoxes& combined_box: combined_boxes) {
            combined_box_to_stream(file, combined_box);
        }

        if(file.fail()) {
            throw std::runtime_error("Failed to write to " + path.string());
        }
        std::cout << "Exported " << combined_boxes.size() << " combined boxes to " << path << std::endl;
    }
}
