#pragma once

#include <vector>

template<typename Range>
concept RangeType =
    std::is_destructible_v<Range> &&
    std::is_copy_constructible_v<Range> &&
    std::is_move_constructible_v<Range> &&
    std::is_copy_assignable_v<Range> &&
    std::is_move_assignable_v<Range> &&

    !std::is_default_constructible_v<Range> &&

    requires(const Range& range, const Range& other_range) {
        { range.parts() } -> std::convertible_to<std::vector<Range>>;
        { range < other_range } -> std::convertible_to<bool>;
    };
