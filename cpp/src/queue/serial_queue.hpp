#pragma once

#include "queue/task_type.hpp"
#include <queue>
#include <optional>

template<bool Priority, typename Task> requires (Priority ? PriorityTaskType<Task> : TaskType<Task>)
class BaseSerialQueue {
    std::conditional_t<Priority, std::priority_queue<Task>, std::queue<Task>> queue_{};

public:
    explicit BaseSerialQueue() = default;

    size_t size() const {
        return queue_.size();
    }

    void push(const Task& task) {
        queue_.push(task);
    }

    std::optional<Task> pop() {
        if(queue_.empty()) {
            return std::nullopt;
        }
        if constexpr(Priority) {
            const Task task = queue_.top();
            queue_.pop();
            return std::make_optional(task);
        } else {
            const Task task = queue_.front();
            queue_.pop();
            return std::make_optional(task);
        }
    }
};

template<TaskType Task>
using SerialQueue = BaseSerialQueue<false, Task>;

template<PriorityTaskType Task>
using SerialPriorityQueue = BaseSerialQueue<true, Task>;
