#pragma once

#include "queue/queue_type.hpp"
#include <queue>
#include <optional>

template<bool Priority, typename Task> requires (Priority ? PriorityTaskType<Task> : TaskType<Task>)
class BaseSerialQueue {
    std::conditional_t<Priority, std::priority_queue<Task>, std::queue<Task>> queue_{};
    size_t size_{0};

public:
    explicit BaseSerialQueue() = default;

    ~BaseSerialQueue() = default;

    BaseSerialQueue(const BaseSerialQueue& queue) = delete;

    BaseSerialQueue(BaseSerialQueue&& queue) = delete;

    BaseSerialQueue& operator=(const BaseSerialQueue&) = delete;

    BaseSerialQueue& operator=(BaseSerialQueue&&) = delete;

    size_t size() const {
        return size_;
    }

    void add(const Task& task) {
        queue_.push(task);
        size_++;
    }

    std::optional<Task> fetch() {
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

    void ack() {
        size_--;
    }

    std::vector<Task> flush() {
        std::vector<Task> tasks;
        while(!queue_.empty()) {
            if constexpr(Priority) {
                tasks.push_back(queue_.top());
                queue_.pop();
            } else {
                tasks.push_back(queue_.front());
                queue_.pop();
            }
        }
        size_ = 0;
        return tasks;
    }
};

template<TaskType Task>
using SerialQueue = BaseSerialQueue<false, Task>;

static_assert(QueueType<SerialQueue<int>, int>);

template<PriorityTaskType Task>
using SerialPriorityQueue = BaseSerialQueue<true, Task>;

static_assert(QueueType<SerialPriorityQueue<int>, int>);
