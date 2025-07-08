#pragma once

#include <queue>
#include <optional>

template<typename Task>
concept TaskType =
    std::copy_constructible<Task> &&
    std::destructible<Task> &&
    std::constructible_from<std::optional<Task>, Task> &&

    requires(std::queue<Task>& queue, const Task& task) {
        { queue.push(task) } -> std::same_as<void>;
        { queue.front() } -> std::convertible_to<Task>;
        { queue.pop() } -> std::same_as<void>;
    };

template<typename Task>
concept PriorityTaskType =
    TaskType<Task> &&

    requires(std::priority_queue<Task>& queue, const Task& task, const Task& other_task) {
        { queue.push(task) } -> std::same_as<void>;
        { queue.top() } -> std::convertible_to<Task>;
        { queue.pop() } -> std::same_as<void>;
        { std::less<Task>{}(task, other_task) } -> std::convertible_to<bool>;
    };
