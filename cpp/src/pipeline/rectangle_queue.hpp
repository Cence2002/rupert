#pragma once

#include "rectangle.hpp"
#include <queue>
#include <optional>

struct Queue2 {
private:
    std::queue<Rectangle> queue_;

public:
    explicit Queue2() : queue_() {
        push(Rectangle());
    }

    bool empty() const {
        return queue_.empty();
    }

    void push(const Rectangle& rectangle) {
        queue_.push(rectangle);
    }

    std::optional<Rectangle> pop() {
        if(empty()) {
            return std::nullopt;
        }
        const Rectangle rectangle = queue_.front();
        queue_.pop();
        return std::make_optional(rectangle);
    }
};

struct RectangleQueue {
private:
    std::queue<Rectangle> queue_;

public:
    bool empty() const;

    void push(const Rectangle& rectangle);

    std::optional<Rectangle> pop();
};
