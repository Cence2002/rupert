#pragma once

#include "range/range3.hpp"
#include "range/range2.hpp"
#include <vector>

struct TerminalBox {
private:
    Range3 range3_;
    std::vector<Range2> range2s_;

public:
    explicit TerminalBox(const Range3& range3, const std::vector<Range2>& range2s) : range3_(range3), range2s_(range2s) {}

    const Range3& range3() const {
        return range3_;
    }

    const std::vector<Range2>& range2s() const {
        return range2s_;
    }

    bool is_valid() const {
        return range3_.is_valid();
    }
};
