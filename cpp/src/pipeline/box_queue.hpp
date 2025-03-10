#pragma once

#include "box.hpp"
#include <queue>
#include <mutex>
#include <optional>

struct Queue3 {
private:
    std::queue<Box> queue_;
    std::mutex mutex_;

public:
    explicit Queue3() : queue_(), mutex_() {
        // push(Box());

        //start from a small subset of the space
        push(Box(
            Id(0b0101, 4),
            Id(0b0011, 4),
            Id(0b1010, 4)
        ));

        // Terminal Box: T<000000> P<000011> A<000001>
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
};

struct BoxQueue {
private:
    std::queue<Box> queue_;
    std::mutex mutex_;

public:
    void push(const Box& box);

    std::optional<Box> pop();

    std::vector<Box> flush();
};
