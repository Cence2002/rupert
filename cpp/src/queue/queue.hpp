#pragma once

#include "queue/task_type.hpp"
#include <queue>
#include <optional>

template<TaskType Task>
struct Queue {
private:
    std::queue<Task> queue_;

public:
    explicit Queue() : queue_() {}

    size_t size() {
        return queue_.size();
    }

    void push(const Task& task) {
        queue_.push(task);
    }

    std::optional<Task> pop() {
        if(queue_.empty()) {
            return std::nullopt;
        }
        const Task task = queue_.front();
        queue_.pop();
        return std::make_optional(task);
    }
};
