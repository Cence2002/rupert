#pragma once

template<typename Range>
concept RangeType = requires(const Range& range) {
    { range.is_valid() } -> std::convertible_to<bool>;
    { range.parts() } -> std::convertible_to<std::vector<Range>>;
};
