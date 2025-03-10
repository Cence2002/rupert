#pragma once

#include "interval/interval.hpp"
#include "config.hpp"
#include "box.hpp"
#include "box_queue.hpp"
#include "terminal_box_queue.hpp"

template<IntervalType Interval>
struct BoxProcessor {
private:
    const Config<Interval>& config_;
    BoxQueue& box_queue_;
    TerminalBoxQueue& terminal_box_queue_;

public:
    void process();
};
