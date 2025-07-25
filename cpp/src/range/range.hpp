#pragma once

#include "range_type.hpp"
#include "interval/intervals.hpp"
#include <cstdint>
#include <bitset>

class Range {
    uint8_t depth_;
    uint32_t bits_;

public:
    explicit Range() = default;

    explicit Range(const uint8_t depth, const uint32_t bits) : depth_(depth), bits_(bits) {
        if(depth_ >= 30) {
            throw std::runtime_error("depth >= 30");
        }
        if(bits_ >= 1u << depth_) {
            throw std::runtime_error("bits >= 2^depth");
        }
    }

    bool operator<(const Range& other) const {
        if(depth_ < other.depth_) {
            return false;
        }
        if(other.depth_ < depth_) {
            return true;
        }
        return bits_ < other.bits_;
    }

    uint8_t depth() const {
        return depth_;
    }

    uint32_t bits() const {
        return bits_;
    }

    bool terminal() const {
        return depth_ >= 15;
    }

    std::pair<Range, Range> parts() const {
        return {
            Range(static_cast<uint8_t>(depth_ + 1), bits_ << 1),
            Range(static_cast<uint8_t>(depth_ + 1), bits_ << 1 | 1)
        };
    }

    template<IntervalType Interval>
    Interval interval() const {
        return Interval(static_cast<int>(bits_), static_cast<int>(bits_ + 1)) / Interval(1 << depth_);
    }

    template<IntervalType Interval>
    Interval interval_mid() const {
        return Interval(static_cast<int>(bits_ << 1 | 1)) / Interval(1 << (depth_ + 1));
    }

    template<IntervalType Interval>
    Interval angle() const {
        return Interval(2) * Interval::pi() * interval<Interval>();
    }

    template<IntervalType Interval>
    Interval angle_mid() const {
        return Interval(2) * Interval::pi() * interval_mid<Interval>();
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Range& id) {
        return ostream << "<" << std::bitset<16>(id.bits_).to_string().substr(16 - id.depth_) << ">";
    }

    uint32_t pack() const {
        return (1 << depth_) | bits_;
    }

    static Range unpack(const uint32_t packed) {
        if(packed == 0) {
            return Range(16, 0);
        }
        const uint8_t depth = static_cast<uint8_t>(31 - __builtin_clz(packed));
        const uint32_t bits = packed & static_cast<uint32_t>((1 << depth) - 1);
        return Range(depth, bits);
    }
};

// static_assert(RangeType<Range>);
