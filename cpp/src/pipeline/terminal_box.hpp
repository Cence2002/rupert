#pragma once

#include "pipeline/box.hpp"
#include "pipeline/rectangle.hpp"
#include <vector>

struct TerminalBox {
private:
    Box box_;
    std::vector<Rectangle> rectangles_;

public:
    TerminalBox(const Box& box, const std::vector<Rectangle>& rectangles) : box_(box), rectangles_(rectangles) {}

    static TerminalBox invalid() {
        return TerminalBox(Box::invalid(), {});
    }

    bool is_invalid() const {
        return box_.is_invalid();
    }

    size_t size() const {
        return box_.size() + rectangles_.size() * rectangles_[0].size();
    }

    void to_stream(std::ostream& os) const {
        box_.to_stream(os);
        for(const Rectangle& rectangle: rectangles_) {
            rectangle.to_stream(os);
        }
    }

    static TerminalBox from_bytes(std::istream& is) {
        const Box box = Box::from_bytes(is);
        if(box.is_invalid()) {
            return invalid();
        }
        std::vector<Rectangle> rectangles;
        while(true) {
            const Rectangle rectangle = Rectangle::from_bytes(is);
            if(rectangle.is_invalid()) {
                break;
            }
            rectangles.push_back(rectangle);
        }
        return TerminalBox(box, rectangles);
    }
};
