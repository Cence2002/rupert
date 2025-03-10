#pragma once

#include "box.hpp"
#include <queue>
#include <mutex>
#include <optional>

struct BoxQueue {
private:
    std::queue<Box> queue_;
    std::mutex mutex_;

public:
    explicit BoxQueue() : queue_(), mutex_() {
        // push(Box());

        push(Box(
            Id(0b0101, 4),
            Id(0b0011, 4),
            Id(0b1010, 4)
        ));

        // push(Box(
        //     Id(0b000000, 6),
        //     Id(0b000011, 6),
        //     Id(0b000001, 6)
        // ));
    }

    void push(const Box& box) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(box);
    }

    std::optional<Box> pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if(queue_.empty()) {
            return std::nullopt;
        }
        const Box box = queue_.front();
        queue_.pop();
        return std::make_optional(box);
    }

    std::vector<Box> flush() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<Box> boxes;
        boxes.reserve(queue_.size());
        while(!queue_.empty()) {
            boxes.push_back(queue_.front());
            queue_.pop();
        }
        return boxes;
    }
};
