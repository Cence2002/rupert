#pragma once

#include "queue/task_type.hpp"
#include <queue>
#include <optional>
#include <mutex>

template<TaskType Task>
struct ConcurrentQueue {
private:
    std::queue<Task> queue_;
    std::mutex mutex_;

public:
    explicit ConcurrentQueue() : queue_(), mutex_() {}

    size_t size() {
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
        const Task task = queue_.front();
        queue_.pop();
        return std::make_optional(task);
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
            tasks.push_back(queue_.front());
            queue_.pop();
        }
        return tasks;
    }
};
