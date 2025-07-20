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

    requires(std::priority_queue<Task>& queue, const Task& task) {
        { queue.push(task) } -> std::same_as<void>;
        { queue.top() } -> std::convertible_to<Task>;
        { queue.pop() } -> std::same_as<void>;
    };

template<typename Queue, typename Task>
concept QueueType =
    std::is_default_constructible_v<Queue> &&
    std::is_destructible_v<Queue> &&

    !std::is_copy_constructible_v<Queue> &&
    !std::is_move_constructible_v<Queue> &&
    !std::is_copy_assignable_v<Queue> &&
    !std::is_move_assignable_v<Queue> &&

    requires(Queue& queue, const Task& task) {
        { queue.size() } -> std::same_as<size_t>;
        { queue.add(task) } -> std::same_as<void>;
        { queue.fetch() } -> std::convertible_to<std::optional<Task>>;
        { queue.ack() } -> std::same_as<void>;
    };
