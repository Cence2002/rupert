#pragma once

#include "interval/interval.hpp"
#include "config.hpp"
#include "terminal_box.hpp"

template<IntervalType Interval>
struct Exporter {
private:
    const Config<Interval>& config_;

public:
    explicit Exporter(const Config<Interval>& config) : config_(config) {}

    void export_terminal_boxes(const std::vector<TerminalBox>& terminal_boxes) {
        print("Exporting ", terminal_boxes.size(), " terminal boxes to ", config_.path("terminal_boxes.bin"));
    }

    void export_boxes(const std::vector<Box>& boxes) {
        print("Exporting ", boxes.size(), " boxes to ", config_.path("boxes.bin"));
    }
};
