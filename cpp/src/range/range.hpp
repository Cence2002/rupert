#pragma once

#include "range_type.hpp"
#include "interval/intervals.hpp"
#include <cstdint>
#include <bitset>

struct Range {
private:
    uint8_t depth_;
    uint16_t bits_;

public:
    explicit Range(const uint8_t depth, const uint16_t bits) : depth_(depth), bits_(bits) {
        if(depth_ >= 16) {
            throw std::runtime_error("depth >= 16");
        }
        if(bits_ >= 1 << depth_) {
            throw std::runtime_error("bits >= 2^depth");
        }
    }

    ~Range() = default;

    Range(const Range& range) = default;

    Range(Range&& range) = default;

    Range& operator=(const Range&) = delete;

    Range& operator=(Range&&) = delete;

    bool operator<(const Range& other) const {
        if(depth_ < other.depth_) {
            return true;
        }
        if(depth_ > other.depth_) {
            return false;
        }
        return bits_ < other.bits_;
    }

    uint8_t depth() const {
        return depth_;
    }

    uint16_t bits() const {
        return bits_;
    }

    std::vector<Range> parts() const {
        return {
            Range(static_cast<uint8_t>(depth_ + 1), static_cast<uint16_t>(bits_ << 1)),
            Range(static_cast<uint8_t>(depth_ + 1), static_cast<uint16_t>(bits_ << 1 | 1))
        };
    }

    template<IntervalType Interval>
    Interval interval() const {
        return Interval(bits_, static_cast<uint16_t>(bits_ + 1)) / Interval(static_cast<uint16_t>(1 << depth_));
    }

    template<IntervalType Interval>
    Interval angle_interval() const {
        return Interval(2) * Interval::pi() * interval<Interval>();
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Range& id) {
        return ostream << "<" << std::bitset<16>(id.bits_).to_string().substr(16 - id.depth_) << ">";
    }

    uint16_t pack() const {
        return static_cast<uint16_t>(1 << depth_) | bits_;
    }

    static Range unpack(const uint16_t packed) {
        if(packed == 0) {
            return Range(16, 0);
        }
        const uint8_t depth = static_cast<uint8_t>(31 - __builtin_clz(packed));
        const uint16_t bits = packed & static_cast<uint16_t>((1 << depth) - 1);
        return Range(depth, bits);
    }
};

static_assert(RangeType<Range>);
