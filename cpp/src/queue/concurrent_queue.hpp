#pragma once

#include "queue/queue_type.hpp"
#include <queue>
#include <optional>
#include <mutex>
#include <vector>
#include <atomic>

template<bool Priority, typename Task> requires (Priority ? PriorityTaskType<Task> : TaskType<Task>)
class BaseConcurrentQueue {
    std::conditional_t<Priority, std::priority_queue<Task>, std::queue<Task>> queue_{};
    mutable std::mutex mutex_{};
    size_t size_{0};

public:
    explicit BaseConcurrentQueue() = default;

    ~BaseConcurrentQueue() = default;

    BaseConcurrentQueue(const BaseConcurrentQueue& queue) = delete;

    BaseConcurrentQueue(BaseConcurrentQueue&& queue) = delete;

    BaseConcurrentQueue& operator=(const BaseConcurrentQueue&) = delete;

    BaseConcurrentQueue& operator=(BaseConcurrentQueue&&) = delete;

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return size_;
    }

    void add(const Task& task) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(task);
        size_++;
    }

    std::optional<Task> fetch() {
        std::lock_guard<std::mutex> lock(mutex_);
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
        std::lock_guard<std::mutex> lock(mutex_);
        size_--;
    }

    std::vector<Task> flush() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<Task> tasks;
        tasks.reserve(queue_.size());
        while(!queue_.empty()) {
            if constexpr(Priority) {
                tasks.push_back(queue_.top());
                queue_.pop();
                size_--;
            } else {
                tasks.push_back(queue_.front());
                queue_.pop();
                size_--;
            }
        }
        return tasks;
    }
};

template<TaskType Task>
using ConcurrentQueue = BaseConcurrentQueue<false, Task>;

static_assert(QueueType<ConcurrentQueue<int>, int>);

template<PriorityTaskType Task>
using ConcurrentPriorityQueue = BaseConcurrentQueue<true, Task>;

static_assert(QueueType<ConcurrentPriorityQueue<int>, int>);
