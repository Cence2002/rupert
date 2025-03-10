#pragma once

#include "box.hpp"
#include "rectangle.hpp"

struct TerminalBox {
private:
    Box box_;
    std::vector<Rectangle> rectangles_;

public:
    size_t size() const;

    std::vector<uint8_t> to_bytes() const;

    TerminalBox from_bytes(const std::vector<uint8_t>& bytes);
};
