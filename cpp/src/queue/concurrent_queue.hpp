#pragma once

#include "queue/task_type.hpp"
#include <queue>
#include <optional>
#include <mutex>
#include <vector>

template<bool Priority, typename Task> requires (Priority ? PriorityTaskType<Task> : TaskType<Task>)
struct BaseConcurrentQueue {
private:
    std::conditional_t<Priority, std::priority_queue<Task>, std::queue<Task>> queue_{};
    mutable std::mutex mutex_{};

public:
    explicit BaseConcurrentQueue() = default;

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    void push(const Task& task) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(task);
    }

    std::optional<Task> pop() {
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

    void push_all(const std::vector<Task>& tasks) {
        std::lock_guard<std::mutex> lock(mutex_);
        for(const Task& task: tasks) {
            queue_.push(task);
        }
    }

    std::vector<Task> pop_all() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<Task> tasks;
        tasks.reserve(queue_.size());
        while(!queue_.empty()) {
            if constexpr(Priority) {
                tasks.push_back(queue_.top());
                queue_.pop();
            } else {
                tasks.push_back(queue_.front());
                queue_.pop();
            }
        }
        return tasks;
    }
};

template<TaskType Task>
using ConcurrentQueue = BaseConcurrentQueue<false, Task>;

template<PriorityTaskType Task>
using ConcurrentPriorityQueue = BaseConcurrentQueue<true, Task>;
