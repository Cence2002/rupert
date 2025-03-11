#pragma once

#include "interval/interval.hpp"
#include "config.hpp"
#include "terminal_box.hpp"
#include <fstream>

template<IntervalType Interval>
struct Exporter {
private:
    const Config<Interval>& config_;

public:
    explicit Exporter(const Config<Interval>& config) : config_(config) {}

    void export_terminal_boxes(const std::vector<TerminalBox>& terminal_boxes) {
        std::ostringstream buffer;
        for(const TerminalBox& terminal_box: terminal_boxes) {
            terminal_box.to_stream(buffer);
        }

        std::filesystem::path path = config_.path("terminal_boxes.bin");
        std::ofstream file(path, std::ios::binary | std::ios::app);
        if(!file.is_open()) {
            throw std::runtime_error("Failed to open " + path.string());
        }

        file.write(buffer.str().c_str(), buffer.str().size());

        if(file.fail()) {
            throw std::runtime_error("Failed to write to " + path.string());
        }
        print("Exported ", terminal_boxes.size(), " terminal boxes to ", path);
    }

    void export_boxes(const std::vector<Box>& boxes) {
        std::ostringstream buffer;
        for(const Box& box: boxes) {
            box.to_stream(buffer);
        }

        std::filesystem::path path = config_.path("boxes.bin");
        std::ofstream file(path, std::ios::binary | std::ios::trunc);
        if(!file.is_open()) {
            throw std::runtime_error("Failed to open " + path.string());
        }

        file.write(buffer.str().c_str(), buffer.str().size());

        if(file.fail()) {
            throw std::runtime_error("Failed to write to " + path.string());
        }
        print("Exported ", boxes.size(), " boxes to ", path);
    }
};
