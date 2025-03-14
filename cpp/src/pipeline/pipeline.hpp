#pragma once

#include "pipeline/box_processor.hpp"
#include "pipeline/importer.hpp"
#include "pipeline/exporter.hpp"
#include <thread>

template<IntervalType Interval>
struct Pipeline {
private:
    const Config<Interval>& config_;

    BoxQueue box_queue_;
    TerminalBoxQueue terminal_box_queue_;

    Importer<Interval> importer_;
    Exporter<Interval> exporter_;
    DebugExporter debug_exporter_;

    std::vector<std::thread> processor_threads_;
    std::thread exporter_thread_;
    std::atomic<uint32_t> processed_box_count_;
    std::atomic<bool> terminated_;
    std::atomic<uint8_t> terminated_thread_count_;

    void start_box_processor() {
        BoxProcessor<Interval> processor(config_, debug_exporter_, box_queue_, terminal_box_queue_);
        while(!terminated_) {
            if(processor.process()) {
                if(++processed_box_count_ > config_.box_iteration_limit()) {
                    stop();
                }
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

        debug_exporter_.cover_builder.set_description("Exported Cover");
        debug_exporter_.cover_builder.set_hole(debug_exporter_.builder, config_.hole());
        debug_exporter_.cover_builder.set_plug(debug_exporter_.builder, config_.plug());
        debug_exporter_.save("../../web/static/cover.bin");

        mpfr_free_cache();
    }

public:
    explicit Pipeline(const Config<Interval>& config) : config_(config),
                                                        box_queue_(),
                                                        terminal_box_queue_(),
                                                        importer_(config),
                                                        exporter_(config),
                                                        debug_exporter_(),
                                                        processor_threads_(),
                                                        exporter_thread_(),
                                                        processed_box_count_(0),
                                                        terminated_(false),
                                                        terminated_thread_count_(0) {}

    void init() {
        if(importer_.can_import()) {
            const std::vector<Box> boxes = importer_.import_boxes();
            box_queue_.push_all(boxes);
        } else {
            // box_queue_.push(Box()); // TODO: Use this instead of the magic numbers
            box_queue_.push(Box(
                Id(4, 0b0101),
                Id(4, 0b0011),
                Id(4, 0b1010)
            ));
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
