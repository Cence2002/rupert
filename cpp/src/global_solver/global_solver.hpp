#pragma once

#include "global_solver/config.hpp"
#include "global_solver/exporter.hpp"
#include "global_solver/helpers.hpp"
#include "queue/queues.hpp"
#include <thread>
#include <latch>

const std::string polyhedron_file_name = "polyhedron.bin";
const std::string pruned_hole_orientations_file_name = "pruned_hole_orientations.bin";
const std::string unpruned_hole_orientations_file_name = "unpruned_hole_orientations.bin";

/*
HO = Hole Orientation
PO = Plug Orientation

HOs, prunedHOs, unprunedHOs = [full], [], []
for HO in HOs:
    if HO outside base symmetries: continue (redundant)
    prunable, POs, prunedPOs, unprunedPOs, collect_unpruned = true, [full], [], [], |HO| < threshold

    for PO in POs:
        if PO outside base rotations: continue (redundant)
        if |PO, HO| < threshold: continue (out of scope)

        if PO sample inside HO sample: export(HO, PO), terminate (rupert)
        if PO sample inside HO and not close:
            if collect_unpruned: set prunable to false, add PO to unprunedPOs, continue (unpruned)
            set prunable to false, break (shortcut)

        if PO outside HO: add PO to prunedPOs, continue (pruned)
        if |PO| < threshold: set prunable to false, add PO to unprunedPOs, continue (too small)

        add pieces of PO to POs

    if prunable: add HO and prunedPOs to prunedHOs, continue (pruned)
    if collect_unpruned: add HO and unprunedPOs to unprunedHOs, continue (too small)

    add pieces of HO to HOs

export(prunedHOs, unprunedHOs)
*/

template<IntervalType Interval>
class GlobalSolver {
    const Config<Interval>& config_;

    ConcurrentPriorityQueue<Box3> hole_orientations_{};
    std::vector<std::thread> threads_{};
    std::atomic<bool> interrupted_{false};

    ConcurrentQueue<CombinedOrientation> pruned_hole_orientations_{};
    ConcurrentQueue<CombinedOrientation> unpruned_hole_orientations_{};
    std::latch exporter_latch_;

    std::tuple<bool, std::vector<Box2>, std::vector<Box2>> process_plug_orientations(const Box3& hole_orientation, const bool collect_unpruned_plug_orientations) {
        const Polygon<Interval> projected_hole = project_polyhedron(config_.polyhedron, hole_orientation, config_.resolution);
        bool prunable = true;
        SerialQueue<Box2> plug_orientations;
        plug_orientations.add(Box2(std::array{Range(0, 0), Range(0, 0)}));
        std::vector<Box2> pruned_plug_orientations;
        std::vector<Box2> unpruned_plug_orientations;
        while(plug_orientations.size() > 0) {
            const std::optional<Box2> optional_plug_orientation = plug_orientations.fetch();
            if(!optional_plug_orientation.has_value()) {
                throw std::runtime_error("plug_orientations is empty");
            }
            const Box2& plug_orientation = optional_plug_orientation.value();
            if(hole_orientation_close_to_plug_orientation(
                config_.polyhedron,
                Angle::theta<Interval>(hole_orientation),
                Angle::phi<Interval>(hole_orientation),
                Angle::alpha<Interval>(hole_orientation),
                Angle::theta<Interval>(plug_orientation),
                Angle::phi<Interval>(plug_orientation),
                config_.epsilon
            )) {
                plug_orientations.ack();
                continue;
            }
            if(plug_orientation_sample_inside_hole_orientation_sample(config_.polyhedron, hole_orientation, plug_orientation)) {
                std::cout << "Rupert passage found for hole orientation: " << hole_orientation << " and plug orientation: " << plug_orientation << std::endl;
                throw std::runtime_error("Rupert passage found");
            }
            if(plug_orientation_sample_inside_hole_orientation(config_.polyhedron, projected_hole, plug_orientation) &&
               !hole_orientation_close_to_plug_orientation(
                   config_.polyhedron,
                   Angle::theta<Interval>(hole_orientation),
                   Angle::phi<Interval>(hole_orientation),
                   Angle::alpha<Interval>(hole_orientation),
                   Angle::theta_mid<Interval>(plug_orientation),
                   Angle::phi_mid<Interval>(plug_orientation),
                   config_.epsilon
               )) {
                if(collect_unpruned_plug_orientations) {
                    prunable = false;
                    unpruned_plug_orientations.push_back(plug_orientation);
                    plug_orientations.ack();
                    continue;
                }
                return std::make_tuple(false, std::vector<Box2>(), std::vector<Box2>());
            }
            if(plug_orientation_outside_hole_orientation(config_.polyhedron, plug_orientation, projected_hole)) {
                pruned_plug_orientations.push_back(plug_orientation);
                plug_orientations.ack();
                continue;
            }
            if(plug_orientation.terminal() || Angle::angle_radius<Interval>(plug_orientation) < config_.plug_epsilon) {
                if(collect_unpruned_plug_orientations) {
                    prunable = false;
                    unpruned_plug_orientations.push_back(plug_orientation);
                    plug_orientations.ack();
                    continue;
                }
                return std::make_tuple(false, std::vector<Box2>(), std::vector<Box2>());
            }
            for(const Box2& rectangle_part: plug_orientation.parts()) {
                plug_orientations.add(rectangle_part);
            }
            plug_orientations.ack();
        }
        return std::make_tuple(prunable, pruned_plug_orientations, unpruned_plug_orientations);
    }

    void process_hole_orientation(const Box3& hole_orientation) {
        if(!(Angle::angle_radius<Interval>(hole_orientation) < Interval::pi() / Interval(2) * Interval(config_.resolution))) {
            std::cout << "Skippable: " << hole_orientation << std::endl;
            for(const Box3& hole_orientation_part: hole_orientation.parts()) {
                hole_orientations_.add(hole_orientation_part);
            }
            return;
        }
        const bool collect_unpruned_plug_orientations = hole_orientation.terminal() || Angle::angle_radius<Interval>(hole_orientation) < config_.hole_epsilon;
        const auto& [prunable, pruned_plug_orientations, unpruned_plug_orientations] = process_plug_orientations(hole_orientation, collect_unpruned_plug_orientations);
        if(prunable) {
            std::cout << "Prunable: " << hole_orientation << std::endl;
            pruned_hole_orientations_.add(CombinedOrientation(hole_orientation, pruned_plug_orientations));
            return;
        }
        if(collect_unpruned_plug_orientations) {
            std::cout << "Not prunable: " << hole_orientation << std::endl;
            unpruned_hole_orientations_.add(CombinedOrientation(hole_orientation, unpruned_plug_orientations));
            return;
        }
        for(const Box3& hole_orientation_part: hole_orientation.parts()) {
            hole_orientations_.add(hole_orientation_part);
        }
    }

    void processor_hole_orientations() {
        while(!interrupted_ && hole_orientations_.size() > 0) {
            const std::optional<Box3> optional_hole_orientation = hole_orientations_.fetch();
            if(optional_hole_orientation.has_value()) {
                process_hole_orientation(optional_hole_orientation.value());
                hole_orientations_.ack();
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
        hole_orientations_.add(Box3(std::array{Range(0, 0), Range(1, 0), Range(1, 0)}));
        Exporter::create_empty_working_directory(config_.working_directory());
        Exporter::export_polyhedron(config_.working_directory() / polyhedron_file_name, config_.polyhedron);
        std::ranges::generate_n(std::back_inserter(threads_), config_.threads, [this] {
            return std::thread([this] {
                processor_hole_orientations();
            });
        });
        for(std::thread& thread: threads_) {
            thread.join();
        }
        exporter_latch_.wait();
        Exporter::export_combined_orientations(config_.working_directory() / pruned_hole_orientations_file_name, pruned_hole_orientations_.flush());
        Exporter::export_combined_orientations(config_.working_directory() / unpruned_hole_orientations_file_name, unpruned_hole_orientations_.flush());
        mpfr_free_cache();
    }

    void interrupt() {
        interrupted_ = true;
    }
};
