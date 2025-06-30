#pragma once

#include "global_solver/box_processor.hpp"
#include "global_solver/importer.hpp"
#include "global_solver/exporter.hpp"
#include "global_solver/geometry_helpers.hpp"
#include <thread>

const std::string polyhedra_file_name = "polyhedra.bin";
const std::string terminal_boxes_file_name = "terminal_boxes.bin";
const std::string boxes_file_name = "boxes.bin";
const std::string debug_file_name = "debug.bin";

template<IntervalType Interval>
struct GlobalSolver {
private:
    const Config<Interval>& config_;

    ConcurrentQueue<Range3> box_queue_;
    ConcurrentQueue<TerminalBox> terminal_box_queue_;

    DebugExporter<Interval> debug_exporter_;

    std::vector<std::thread> processor_threads_;
    std::thread exporter_thread_;
    std::atomic<uint32_t> processed_box_count_;
    std::atomic<bool> terminated_;
    std::atomic<uint8_t> terminated_thread_count_;

    void test_polyhedra() {
        if(!is_centrally_symmetric(config_.polyhedron.vertices())) {
            throw std::runtime_error("Hole polyhedron is not centrally symmetric");
        }
        if(!is_centrally_symmetric(config_.polyhedron.vertices())) {
            throw std::runtime_error("Plug polyhedron is not centrally symmetric");
        }
    }

    void start_box_processor() {
        BoxProcessor<Interval> processor(config_, debug_exporter_, box_queue_, terminal_box_queue_);
        while(!terminated_) {
            if(processor.process()) {
                if(++processed_box_count_ >= config_.box_iteration_limit) {
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
        while(!terminated_) {
            if(terminal_box_queue_.size() < config_.export_size_threshold) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }
            Exporter::export_terminal_boxes(config_.working_directory() / terminal_boxes_file_name, terminal_box_queue_.pop_all());
        }
        while(terminated_thread_count_ < config_.thread_count) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        Exporter::export_terminal_boxes(config_.working_directory() / terminal_boxes_file_name, terminal_box_queue_.pop_all());
        Exporter::export_boxes(config_.working_directory() / boxes_file_name, box_queue_.pop_all());

        if(config_.debug) {
            debug_exporter_.export_debug(config_.working_directory() / debug_file_name);
        }

        mpfr_free_cache();
    }

public:
    explicit GlobalSolver(const Config<Interval>& config) : config_(config),
                                                            box_queue_(),
                                                            terminal_box_queue_(),
                                                            debug_exporter_(),
                                                            processor_threads_(),
                                                            exporter_thread_(),
                                                            processed_box_count_(0),
                                                            terminated_(false),
                                                            terminated_thread_count_(0) {
        test_polyhedra();
        if(config_.debug) {
            debug_exporter_.debug_builder.set_hole(config_.polyhedron);
            debug_exporter_.debug_builder.set_plug(config_.polyhedron);
        }
    }

    void init() {
        if(config_.restart) {
            Importer::restart(config_.working_directory());
        }
        Exporter::create_working_directory(config_.working_directory());
        Exporter::export_polyhedra(config_.working_directory() / polyhedra_file_name, config_.polyhedron);
        const std::vector<Range3> boxes = Importer::import_boxes(config_.working_directory() / boxes_file_name);
        if(boxes.empty()) {
            // box_queue_.push(Box()); // TODO: Use this instead of the magic numbers
            box_queue_.push(Range3(
                Range(9, 0b011011010),
                Range(10, 0b1011001001),
                Range(9, 0b101101110)
            ));
        } else {
            box_queue_.push_all(boxes);
        }
    }

    void start() {
        std::ranges::generate_n(std::back_inserter(processor_threads_), config_.thread_count, [this] {
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
