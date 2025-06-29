#pragma once

#include <queue>
#include <optional>
#include <mutex>

template<typename Task>
concept TaskType =
    std::copy_constructible<Task> &&
    std::destructible<Task> &&
    std::constructible_from<std::optional<Task>, Task> &&
    requires(std::queue<Task>& queue, const Task& task) {
        { queue.push(task) } -> std::same_as<void>;
        { queue.front() } -> std::convertible_to<Task>;
    };

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
