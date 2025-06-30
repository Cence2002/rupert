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
