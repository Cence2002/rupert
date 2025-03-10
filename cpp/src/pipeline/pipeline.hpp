#pragma once

#include "geometry/geometry.hpp"
#include "id.hpp"
#include "rectangle.hpp"
#include "box.hpp"
#include "terminal_box.hpp"
#include "rectangle_queue.hpp"
#include "box_queue.hpp"
#include "terminal_box_queue.hpp"
#include "config.hpp"
#include "box_processor.hpp"
#include "exporter.hpp"
#include "debug_exporter.hpp"
#include <thread>

template<IntervalType Interval>
struct Pipeline {
private:
    BoxQueue box_queue_;
    TerminalBoxQueue terminal_box_queue_;
    Exporter<Interval> exporter_;
    std::vector<std::thread> processor_threads_;
    std::thread exporter_thread_;
    std::atomic<bool> terminated_;

public:
    void start();

    void stop();
};
