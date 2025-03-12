#pragma once

#include "pipeline/box.hpp"
#include <queue>
#include <mutex>
#include <optional>

struct BoxQueue {
private:
    std::queue<Box> queue_;
    std::mutex mutex_;

public:
    explicit BoxQueue() : queue_(), mutex_() {}

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

    void push_all(const std::vector<Box>& boxes) {
        std::lock_guard<std::mutex> lock(mutex_);
        for(const Box& box: boxes) {
            queue_.push(box);
        }
    }

    std::vector<Box> pop_all() {
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
