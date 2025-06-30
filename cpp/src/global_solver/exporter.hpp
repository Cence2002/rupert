#pragma once

#include "global_solver/config.hpp"
#include "global_solver/terminal_box.hpp"
#include <fstream>

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

    inline void terminal_box_to_stream(std::ostream& os, const TerminalBox& terminal_box) {
        box_to_stream(os, terminal_box.range3());
        size_to_stream(os, static_cast<uint32_t>(terminal_box.range2s().size()));
        for(const Range2& rectangle: terminal_box.range2s()) {
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

    inline void export_terminal_boxes(const std::filesystem::path& path, const std::vector<TerminalBox>& terminal_boxes) {
        std::ofstream file(path, std::ios::binary | std::ios::app);
        if(!file.is_open()) {
            throw std::runtime_error("Failed to open " + path.string());
        }

        for(const TerminalBox& terminal_box: terminal_boxes) {
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
