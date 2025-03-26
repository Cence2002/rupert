#pragma once

#include "pipeline/config.hpp"
#include <fstream>

template<IntervalType Interval>
struct Exporter {
private:
    const Config<Interval>& config_;

    static void size_to_stream(std::ostream& os, const uint32_t size) {
        os.write(reinterpret_cast<const char*>(&size), sizeof(size));
    }

    static void float_to_stream(std::ostream& os, const double value) {
        os.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }

    static void vertex_to_stream(std::ostream& os, const Vertex<Interval>& vertex) {
        float_to_stream(os, vertex.x().mid().float_value());
        float_to_stream(os, vertex.y().mid().float_value());
        float_to_stream(os, vertex.z().mid().float_value());
    }

    static void polyhedron_to_stream(std::ostream& os, const Polyhedron<Interval>& polyhedron) {
        size_to_stream(os, static_cast<uint32_t>(polyhedron.vertices().size()));
        for(const Vertex<Interval>& vertex: polyhedron.vertices()) {
            vertex_to_stream(os, vertex);
        }
    }

    static void id_to_stream(std::ostream& os, const Id& id) {
        const uint16_t packed = id.pack();
        os.write(reinterpret_cast<const char*>(&packed), sizeof(packed));
    }

    static void box_to_stream(std::ostream& os, const Box& box) {
        id_to_stream(os, box.theta_id());
        id_to_stream(os, box.phi_id());
        id_to_stream(os, box.alpha_id());
    }

    static void rectangle_to_stream(std::ostream& os, const Rectangle& rectangle) {
        id_to_stream(os, rectangle.theta_id());
        id_to_stream(os, rectangle.phi_id());
    }

    static void terminal_box_to_stream(std::ostream& os, const TerminalBox& terminal_box) {
        box_to_stream(os, terminal_box.box());
        size_to_stream(os, static_cast<uint32_t>(terminal_box.rectangles().size()));
        for(const Rectangle& rectangle: terminal_box.rectangles()) {
            rectangle_to_stream(os, rectangle);
        }
    }

public:
    explicit Exporter(const Config<Interval>& config) : config_(config) {}

    void create_directory() {
        const std::filesystem::path directory_path = config_.directory_path();
        if(!std::filesystem::exists(directory_path)) {
            std::filesystem::create_directory(directory_path);
        }
    }

    void export_polyhedra(const Polyhedron<Interval>& hole, const Polyhedron<Interval>& plug) {
        const std::filesystem::path path = config_.polyhedra_path();
        std::ofstream file(path, std::ios::binary | std::ios::trunc);
        if(!file.is_open()) {
            throw std::runtime_error("Failed to open " + path.string());
        }

        polyhedron_to_stream(file, hole);
        polyhedron_to_stream(file, plug);

        if(file.fail()) {
            throw std::runtime_error("Failed to write to " + path.string());
        }
        std::cout << "Exported polyhedra to " << path << std::endl;
    }

    void export_terminal_boxes(const std::vector<TerminalBox>& terminal_boxes) {
        const std::filesystem::path path = config_.terminal_boxes_path();
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

    void export_boxes(const std::vector<Box>& boxes) {
        const std::filesystem::path path = config_.boxes_path();
        std::ofstream file(path, std::ios::binary | std::ios::trunc);
        if(!file.is_open()) {
            throw std::runtime_error("Failed to open " + path.string());
        }

        size_to_stream(file, static_cast<uint32_t>(boxes.size()));
        for(const Box& box: boxes) {
            box_to_stream(file, box);
        }

        if(file.fail()) {
            throw std::runtime_error("Failed to write to " + path.string());
        }
        std::cout << "Exported " << boxes.size() << " boxes to " << path << std::endl;
    }
};
