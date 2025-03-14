#pragma once

#include "pipeline/config.hpp"
#include <fstream>

template<IntervalType Interval>
struct Exporter {
private:
    const Config<Interval>& config_;

    static void size_to_stream(std::ostream& os, const size_t size) {
        os.write(reinterpret_cast<const char*>(&size), sizeof(size));
    }

public:
    explicit Exporter(const Config<Interval>& config) : config_(config) {}

    void export_terminal_boxes(const std::vector<TerminalBox>& terminal_boxes) {
        const std::filesystem::path path = config_.terminal_boxes_path();
        std::ofstream file(path, std::ios::binary | std::ios::app);
        if(!file.is_open()) {
            throw std::runtime_error("Failed to open " + path.string());
        }

        for(const TerminalBox& terminal_box: terminal_boxes) {
            terminal_box.to_stream(file);
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

        size_to_stream(file, boxes.size());

        for(const Box& box: boxes) {
            box.to_stream(file);
        }

        if(file.fail()) {
            throw std::runtime_error("Failed to write to " + path.string());
        }
        std::cout << "Exported " << boxes.size() << " boxes to " << path << std::endl;
    }
};
