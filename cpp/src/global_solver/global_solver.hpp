#pragma once

#include "global_solver/config.hpp"
#include "global_solver/exporter.hpp"
#include "global_solver/helpers.hpp"
#include "queue/queues.hpp"
#include <thread>
#include <latch>

const std::string polyhedron_file_name = "polyhedron.bin";
const std::string pruned_hole_boxes_file_name = "pruned_hole_boxes.bin";
const std::string unpruned_hole_boxes_file_name = "unpruned_hole_boxes.bin";

/*
HB = Hole Box
PB = Plug Box

HBs, prunedHBs, unprunedHBs = [full], [], []
for HB in HBs:
    if HB outside base symmetries: continue (redundant)
    prunable, PBs, prunedPBs, unprunedPBs, collect_unpruned = true, [full], [], [], |HB| < threshold

    for PB in PBs:
        if PB outside base rotations: continue (redundant)
        if |PB, HB| < threshold: continue (out of scope)

        if PB sample inside HB sample: export(HB, PB), terminate (rupert)
        if PB sample inside HB and not close:
            if collect_unpruned: set prunable to false, add PB to unprunedPBs, continue (unpruned)
            set prunable to false, break (shortcut)

        if PB outside HB: add PB to prunedPBs, continue (pruned)
        if |PB| < threshold: set prunable to false, add PB to unprunedPBs, continue (too small)

        add pieces of PB to PBs

    if prunable: add HB and prunedPBs to prunedHBs, continue (pruned)
    if collect_unpruned: add HB and unprunedPBs to unprunedHBs, continue (too small)

    add pieces of HB to HBs

export(prunedHBs, unprunedHBs)
*/

template<IntervalType Interval>
class GlobalSolver {
    const Config<Interval>& config_;

    ConcurrentPriorityQueue<Box3> hole_boxes_{};
    std::vector<std::thread> threads_{};
    std::atomic<bool> interrupted_{false};

    ConcurrentQueue<CombinedBoxes> pruned_hole_boxes_{};
    ConcurrentQueue<CombinedBoxes> unpruned_hole_boxes_{};
    std::latch exporter_latch_;

    std::tuple<bool, std::vector<Box2>, std::vector<Box2>> process_plug_boxes(const Box3& hole_box, const bool collect_unpruned_plug_boxes) {
        const Polygon<Interval> projected_hole = project_polyhedron(config_.polyhedron, hole_box, config_.resolution);
        bool prunable = true;
        SerialQueue<Box2> plug_boxes;
        plug_boxes.add(Box2(std::array{Range(Bitset(0, 0)), Range(Bitset(0, 0))}));
        std::vector<Box2> pruned_plug_boxes;
        std::vector<Box2> unpruned_plug_boxes;
        while(plug_boxes.size() > 0) {
            const std::optional<Box2> optional_plug_box = plug_boxes.fetch();
            if(!optional_plug_box.has_value()) {
                throw std::runtime_error("plug_boxes is empty");
            }
            const Box2& plug_box = optional_plug_box.value();
            if(hole_box_close_to_plug_box(config_.polyhedron, hole_box, plug_box, config_.epsilon - Angle::angle_radius<Interval>(hole_box) - Angle::angle_radius<Interval>(plug_box))) {
                plug_boxes.ack();
                continue;
            }
            if(plug_box_sample_inside_hole_box_sample(config_.polyhedron, hole_box, plug_box)) {
                std::cout << "Rupert passage found for hole box: " << hole_box << " and plug box: " << plug_box << std::endl;
                throw std::runtime_error("Rupert passage found");
            }
            if(plug_box_sample_inside_hole_box(config_.polyhedron, projected_hole, plug_box) &&
               !hole_box_close_to_plug_box(config_.polyhedron, hole_box, plug_box, config_.epsilon - Angle::angle_radius<Interval>(hole_box))) {
                if(collect_unpruned_plug_boxes) {
                    prunable = false;
                    unpruned_plug_boxes.push_back(plug_box);
                    plug_boxes.ack();
                    continue;
                }
                return std::make_tuple(false, std::vector<Box2>(), std::vector<Box2>());
            }
            if(plug_box_outside_hole_box(config_.polyhedron, plug_box, projected_hole)) {
                pruned_plug_boxes.push_back(plug_box);
                plug_boxes.ack();
                continue;
            }
            if(Angle::angle_radius<Interval>(plug_box) < config_.plug_epsilon) {
                if(collect_unpruned_plug_boxes) {
                    prunable = false;
                    unpruned_plug_boxes.push_back(plug_box);
                    plug_boxes.ack();
                    continue;
                }
                return std::make_tuple(false, std::vector<Box2>(), std::vector<Box2>());
            }
            for(const Box2& rectangle_part: plug_box.parts()) {
                plug_boxes.add(rectangle_part);
            }
            plug_boxes.ack();
        }
        return std::make_tuple(prunable, pruned_plug_boxes, unpruned_plug_boxes);
    }

    void process_hole_box(const Box3& hole_box) {
        if(!(Angle::angle_radius<Interval>(hole_box) < Interval::pi() / Interval(2) * Interval(config_.resolution))) {
            std::cout << "Skippable: " << hole_box << std::endl;
            for(const Box3& hole_box_part: hole_box.parts()) {
                hole_boxes_.add(hole_box_part);
            }
            return;
        }
        const bool collect_unpruned_plug_boxes = Angle::angle_radius<Interval>(hole_box) < config_.hole_epsilon;
        const auto& [prunable, pruned_plug_boxes, unpruned_plug_boxes] = process_plug_boxes(hole_box, collect_unpruned_plug_boxes);
        if(prunable) {
            std::cout << "Prunable: " << hole_box << std::endl;
            pruned_hole_boxes_.add(CombinedBoxes(hole_box, pruned_plug_boxes));
            return;
        }
        if(collect_unpruned_plug_boxes) {
            std::cout << "Not prunable: " << hole_box << std::endl;
            unpruned_hole_boxes_.add(CombinedBoxes(hole_box, unpruned_plug_boxes));
            return;
        }
        for(const Box3& hole_box_part: hole_box.parts()) {
            hole_boxes_.add(hole_box_part);
        }
    }

    void processor_hole_boxes() {
        while(!interrupted_ && hole_boxes_.size() > 0) {
            const std::optional<Box3> optional_hole_box = hole_boxes_.fetch();
            if(optional_hole_box.has_value()) {
                process_hole_box(optional_hole_box.value());
                hole_boxes_.ack();
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        mpfr_free_cache2(MPFR_FREE_LOCAL_CACHE);
        exporter_latch_.count_down();
    }

public:
    explicit GlobalSolver(const Config<Interval>& config) : config_(config), exporter_latch_(config.threads) {}

    void run() {
        hole_boxes_.add(Box3(std::array{Range(Bitset(0, 0)), Range(Bitset(1, 0)), Range(Bitset(1, 0))}));
        Exporter::create_empty_working_directory(config_.working_directory());
        Exporter::export_polyhedron(config_.working_directory() / polyhedron_file_name, config_.polyhedron);
        std::ranges::generate_n(std::back_inserter(threads_), config_.threads, [this] {
            return std::thread([this] {
                processor_hole_boxes();
            });
        });
        for(std::thread& thread: threads_) {
            thread.join();
        }
        exporter_latch_.wait();
        Exporter::export_combined_boxes(config_.working_directory() / pruned_hole_boxes_file_name, pruned_hole_boxes_.flush());
        Exporter::export_combined_boxes(config_.working_directory() / unpruned_hole_boxes_file_name, unpruned_hole_boxes_.flush());
        mpfr_free_cache();
    }

    void interrupt() {
        interrupted_ = true;
    }
};
