#pragma once

#include "global_solver/box_processor.hpp"
#include "global_solver/importer.hpp"
#include "global_solver/exporter.hpp"
#include "global_solver/geometry_helpers.hpp"
#include <thread>

template<IntervalType Interval>
struct GlobalSolver {
private:
    const Config<Interval>& config_;

    ConcurrentQueue<Range3> box_queue_;
    ConcurrentQueue<TerminalBox> terminal_box_queue_;

    Importer<Interval> importer_;
    Exporter<Interval> exporter_;
    DebugExporter<Interval> debug_exporter_;

    std::vector<std::thread> processor_threads_;
    std::thread exporter_thread_;
    std::atomic<uint32_t> processed_box_count_;
    std::atomic<bool> terminated_;
    std::atomic<uint8_t> terminated_thread_count_;

    void test_polyhedra() {
        if(!is_centrally_symmetric(config_.hole().vertices())) {
            throw std::runtime_error("Hole polyhedron is not centrally symmetric");
        }
        if(!is_centrally_symmetric(config_.plug().vertices())) {
            throw std::runtime_error("Plug polyhedron is not centrally symmetric");
        }
    }

    void start_box_processor() {
        BoxProcessor<Interval> processor(config_, debug_exporter_, box_queue_, terminal_box_queue_);
        while(!terminated_) {
            if(processor.process()) {
                if(++processed_box_count_ >= config_.box_iteration_limit()) {
                    stop();
                }
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }

        ++terminated_thread_count_;

        mpfr_free_cache2(MPFR_FREE_LOCAL_CACHE);
    }

    void start_exporter() {
        Exporter<Interval> exporter(config_);
        while(!terminated_) {
            if(terminal_box_queue_.size() < config_.export_size_threshold()) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }
            exporter.export_terminal_boxes(terminal_box_queue_.pop_all());
        }
        while(terminated_thread_count_ < config_.thread_count()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        exporter.export_terminal_boxes(terminal_box_queue_.pop_all());
        exporter.export_boxes(box_queue_.pop_all());

        if(config_.debug_enabled()) {
            debug_exporter_.export_debug();
        }

        mpfr_free_cache();
    }

public:
    explicit GlobalSolver(const Config<Interval>& config) : config_(config),
                                                            box_queue_(),
                                                            terminal_box_queue_(),
                                                            importer_(config),
                                                            exporter_(config),
                                                            debug_exporter_(config),
                                                            processor_threads_(),
                                                            exporter_thread_(),
                                                            processed_box_count_(0),
                                                            terminated_(false),
                                                            terminated_thread_count_(0) {
        test_polyhedra();
        if(config_.debug_enabled()) {
            debug_exporter_.debug_builder.set_hole(config_.hole());
            debug_exporter_.debug_builder.set_plug(config_.plug());
        }
    }

    void init() {
        if(config_.restart()) {
            importer_.restart();
        }
        exporter_.create_directory();
        exporter_.export_polyhedra(config_.hole(), config_.plug());
        const std::vector<Range3> boxes = importer_.import_boxes();
        if(boxes.empty()) {
            // box_queue_.push(Box()); // TODO: Use this instead of the magic numbers
            box_queue_.push(Range3(
                Range(4, 0b0110),
                Range(4, 0b1100),
                Range(4, 0b1010)
            ));
        } else {
            box_queue_.push_all(boxes);
        }
    }

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
