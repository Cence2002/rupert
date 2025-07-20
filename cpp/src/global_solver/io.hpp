#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#include "flatbuffers/flatbuffers_generated.h"
#pragma GCC diagnostic pop

#include "global_solver/helpers.hpp"
#include "range/ranges.hpp"
#include <fstream>
#include <filesystem>

struct EliminatedHoleOrientation {
    Range3 hole_orientation;
    std::vector<Range2> plug_orientations;
};

namespace Importer {
    inline uint32_t size_from_stream(std::istream& is) {
        uint32_t size;
        is.read(reinterpret_cast<char*>(&size), sizeof(size));
        return size;
    }

    inline Range id_from_stream(std::istream& is) {
        uint16_t packed;
        is.read(reinterpret_cast<char*>(&packed), sizeof(packed));
        return Range::unpack(packed);
    }

    inline Range3 box_from_stream(std::istream& is) {
        const Range theta_id = id_from_stream(is);
        const Range phi_id = id_from_stream(is);
        const Range alpha_id = id_from_stream(is);
        return Range3(theta_id, phi_id, alpha_id);
    }

    inline void restart(const std::filesystem::path& working_directory) {
        if(std::filesystem::exists(working_directory)) {
            std::filesystem::remove_all(working_directory);
        }
    }

    inline std::vector<Range3> import_boxes(const std::filesystem::path& path) {
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
}

namespace Exporter {
    inline void size_to_stream(std::ostream& os, const uint32_t size) {
        os.write(reinterpret_cast<const char*>(&size), sizeof(size));
    }

    inline void float_to_stream(std::ostream& os, const double value) {
        os.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }

    template<IntervalType Interval>
    void vertex_to_stream(std::ostream& os, const Vector3<Interval>& vertex) {
        float_to_stream(os, vertex.x().mid().to_float());
        float_to_stream(os, vertex.y().mid().to_float());
        float_to_stream(os, vertex.z().mid().to_float());
    }

    template<IntervalType Interval>
    void polyhedron_to_stream(std::ostream& os, const Polyhedron<Interval>& polyhedron) {
        size_to_stream(os, static_cast<uint32_t>(polyhedron.vertices().size()));
        for(const Vector3<Interval>& vertex: polyhedron.vertices()) {
            vertex_to_stream(os, vertex);
        }
    }

    inline void id_to_stream(std::ostream& os, const Range& id) {
        const uint16_t packed = id.pack();
        os.write(reinterpret_cast<const char*>(&packed), sizeof(packed));
    }

    inline void box_to_stream(std::ostream& os, const Range3& range3) {
        id_to_stream(os, range3.theta_range());
        id_to_stream(os, range3.phi_range());
        id_to_stream(os, range3.alpha_range());
    }

    inline void rectangle_to_stream(std::ostream& os, const Range2& range2) {
        id_to_stream(os, range2.theta_range());
        id_to_stream(os, range2.phi_range());
    }

    inline void terminal_box_to_stream(std::ostream& os, const EliminatedHoleOrientation& terminal_box) {
        box_to_stream(os, terminal_box.hole_orientation);
        size_to_stream(os, static_cast<uint32_t>(terminal_box.plug_orientations.size()));
        for(const Range2& rectangle: terminal_box.plug_orientations) {
            rectangle_to_stream(os, rectangle);
        }
    }

    inline void create_working_directory(const std::filesystem::path& working_directory) {
        if(!std::filesystem::exists(working_directory)) {
            std::filesystem::create_directory(working_directory);
        }
    }

    template<IntervalType Interval>
    void export_polyhedra(const std::filesystem::path& path, const Polyhedron<Interval>& polyhedron) {
        std::ofstream file(path, std::ios::binary | std::ios::trunc);
        if(!file.is_open()) {
            throw std::runtime_error("Failed to open " + path.string());
        }

        polyhedron_to_stream(file, polyhedron);
        polyhedron_to_stream(file, polyhedron); // TODO remove this

        if(file.fail()) {
            throw std::runtime_error("Failed to write to " + path.string());
        }
        std::cout << "Exported polyhedra to " << path << std::endl;
    }

    inline void export_terminal_boxes(const std::filesystem::path& path, const std::vector<EliminatedHoleOrientation>& terminal_boxes) {
        std::ofstream file(path, std::ios::binary | std::ios::app);
        if(!file.is_open()) {
            throw std::runtime_error("Failed to open " + path.string());
        }

        for(const EliminatedHoleOrientation& terminal_box: terminal_boxes) {
            terminal_box_to_stream(file, terminal_box);
        }

        if(file.fail()) {
            throw std::runtime_error("Failed to write to " + path.string());
        }
        std::cout << "Exported " << terminal_boxes.size() << " terminal boxes to " << path << std::endl;
    }

    inline void export_boxes(const std::filesystem::path& path, const std::vector<Range3>& boxes) {
        std::ofstream file(path, std::ios::binary | std::ios::trunc);
        if(!file.is_open()) {
            throw std::runtime_error("Failed to open " + path.string());
        }

        size_to_stream(file, static_cast<uint32_t>(boxes.size()));
        for(const Range3& box: boxes) {
            box_to_stream(file, box);
        }

        if(file.fail()) {
            throw std::runtime_error("Failed to write to " + path.string());
        }
        std::cout << "Exported " << boxes.size() << " boxes to " << path << std::endl;
    }
}
