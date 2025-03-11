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
    const Config<Interval>& config_;
    BoxQueue box_queue_;
    TerminalBoxQueue terminal_box_queue_;
    Exporter<Interval> exporter_;
    std::vector<std::thread> processor_threads_;
    std::thread exporter_thread_;
    std::atomic<uint32_t> processed_box_count_;
    std::atomic<bool> terminated_;
    std::atomic<uint8_t> terminated_thread_count_;

    void start_box_processor() {
        BoxProcessor<Interval> processor(config_, box_queue_, terminal_box_queue_);
        while(!terminated_) {
            if(processor.process()) {
                if(++processed_box_count_ > config_.box_iteration_limit()) {
                    stop();
                }
            }
        }
        ++terminated_thread_count_;
    }

    void start_exporter() {
        Exporter<Interval> exporter(config_);
        while(!terminated_) {
            if(terminal_box_queue_.size() < config_.export_size_threshold()) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }
            exporter.export_terminal_boxes(terminal_box_queue_.flush());
        }
        while(terminated_thread_count_ < config_.thread_count()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        exporter.export_terminal_boxes(terminal_box_queue_.flush());
        exporter.export_boxes(box_queue_.flush());
    }

public:
    explicit Pipeline(const Config<Interval>& config) : config_(config),
                                                        box_queue_(),
                                                        terminal_box_queue_(),
                                                        exporter_(config),
                                                        processor_threads_(),
                                                        exporter_thread_(),
                                                        processed_box_count_(0),
                                                        terminated_(false),
                                                        terminated_thread_count_(0) {}

    void start() {
        std::ranges::generate_n(std::back_inserter(processor_threads_), config_.thread_count(), [this] {
            return std::thread([this] {
                start_box_processor();
            });
        });
        exporter_thread_ = std::thread([this] {
            start_exporter();
        });

        for(std::thread& processor_thread: processor_threads_) {
            processor_thread.join();
        }
        exporter_thread_.join();
    }

    void stop() {
        terminated_ = true;
    }
};
