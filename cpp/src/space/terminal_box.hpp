#pragma once

#include "space/box.hpp"
#include "space/rectangle.hpp"
#include <vector>

struct TerminalBox {
private:
    Box box_;
    std::vector<Rectangle> rectangles_;

public:
    explicit TerminalBox(const Box& box, const std::vector<Rectangle>& rectangles) : box_(box), rectangles_(rectangles) {}

    const Box& box() const {
        return box_;
    }

    const std::vector<Rectangle>& rectangles() const {
        return rectangles_;
    }

    static TerminalBox invalid() {
        return TerminalBox(Box::invalid(), {});
    }

    bool is_invalid() const {
        return box_.is_invalid();
    }

    size_t size() const {
        return box_.size() + rectangles_.size() * rectangles_[0].size();
    }
};
