#pragma once

#include "interval/intervals.hpp"
#include <cstdint>
#include <boost/dynamic_bitset.hpp>

using Bitset = boost::dynamic_bitset<>;

constexpr uint8_t range_depth_limit = 30;

class Range {
    Bitset bits_;

public:
    explicit Range(): bits_(0) {}

    explicit Range(const Bitset& bits) : bits_(bits) {
        if(bits.size() > range_depth_limit) {
            throw std::runtime_error("Range depth limit exceeded");
        }
    }

    bool operator<(const Range& other) const { // ordered by importance
        if(bits_.size() < other.bits_.size()) {
            return false;
        }
        if(other.bits_.size() < bits_.size()) {
            return true;
        }
        return other.bits_ < bits_;
    }

    bool terminal() const {
        return bits_.size() == range_depth_limit;
    }

    std::pair<Range, Range> parts() const {
        Bitset min_part = bits_;
        Bitset max_part = bits_;
        min_part.push_back(false);
        max_part.push_back(true);
        return {Range(min_part), Range(max_part)};
    }

    template<IntervalType Interval>
    Interval interval() const {
        return Interval(static_cast<int>(bits_.to_ulong()), static_cast<int>(bits_.to_ulong() + 1)) / Interval(1 << bits_.size());
    }

    template<IntervalType Interval>
    Interval interval_min() const {
        return Interval(static_cast<int>(bits_.to_ulong())) / Interval(1 << bits_.size());
    }

    template<IntervalType Interval>
    Interval interval_max() const {
        return Interval(static_cast<int>(bits_.to_ulong() + 1)) / Interval(1 << bits_.size());
    }

    template<IntervalType Interval>
    Interval interval_mid() const {
        return Interval(static_cast<int>(bits_.to_ulong() << 1 | 1)) / Interval(2 << bits_.size());
    }

    template<IntervalType Interval>
    Interval interval_len() const {
        return Interval(1) / Interval(1 << bits_.size());
    }

    template<IntervalType Interval>
    Interval interval_rad() const {
        return Interval(1) / Interval(2 << bits_.size());
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Range& id) {
        std::string s;
        boost::to_string(id.bits_, s);
        return ostream << "<" << s << ">";
    }

    uint32_t pack() const {
        const uint8_t depth = static_cast<uint8_t>(bits_.size());
        const uint32_t bits = static_cast<uint32_t>(bits_.to_ulong());
        return (1 << depth) | bits;
    }

    static Range unpack(const uint32_t packed) {
        const uint8_t depth = static_cast<uint8_t>(31 - __builtin_clz(packed));
        const uint32_t bits = packed & static_cast<uint32_t>((1 << depth) - 1);
        return Range(Bitset(depth, bits));
    }
};
