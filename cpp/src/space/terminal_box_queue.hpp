#pragma once

#include "space/terminal_box.hpp"
#include <queue>
#include <mutex>

struct TerminalBoxQueue {
private:
    std::queue<TerminalBox> queue_;
    std::mutex mutex_;
    size_t size_;

public:
    explicit TerminalBoxQueue() : queue_(), mutex_(), size_(0) {}

    size_t size() const {
        return size_;
    }

    void push(const TerminalBox& terminal_box) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(terminal_box);
        size_ += terminal_box.size();
    }

    std::vector<TerminalBox> flush() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<TerminalBox> terminal_boxes;
        terminal_boxes.reserve(queue_.size());
        while(!queue_.empty()) {
            terminal_boxes.push_back(queue_.front());
            queue_.pop();
        }
        size_ = 0;
        return terminal_boxes;
    }
};
