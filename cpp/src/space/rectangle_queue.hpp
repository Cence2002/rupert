#pragma once

#include "space/rectangle.hpp"
#include <queue>
#include <optional>

struct RectangleQueue {
private:
    std::queue<Rectangle> queue_;

public:
    explicit RectangleQueue() : queue_() {
        push(Rectangle());
    }

    void push(const Rectangle& rectangle) {
        queue_.push(rectangle);
    }

    std::optional<Rectangle> pop() {
        if(queue_.empty()) {
            return std::nullopt;
        }
        const Rectangle rectangle = queue_.front();
        queue_.pop();
        return std::make_optional(rectangle);
    }
};
