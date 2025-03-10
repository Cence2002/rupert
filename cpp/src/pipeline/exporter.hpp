#pragma once

#include "interval/interval.hpp"
#include "config.hpp"
#include "terminal_box.hpp"

template<IntervalType Interval>
struct Exporter {
private:
    const Config<Interval>& config_;

public:
    void export_terminal_boxes(const std::vector<TerminalBox>& terminal_boxes);

    void export_boxes(const std::vector<Box>& boxes);
};
