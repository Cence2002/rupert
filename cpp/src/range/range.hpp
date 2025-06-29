#pragma once

#include "range_type.hpp"
#include "interval/interval.hpp"
#include <cstdint>
#include <bitset>

struct Range {
private:
    uint8_t depth_;
    uint16_t bits_;

public:
    explicit Range(const uint8_t depth, const uint16_t bits) : depth_(depth), bits_(bits) {
        if(!is_valid()) {
            throw std::runtime_error("Invalid Range");
        }
    }

    explicit Range() : Range(0, 0) {}

    ~Range() = default;

    Range(const Range& range) = default;

    Range(Range&& range) = default;

    Range& operator=(const Range&) = delete;

    Range& operator=(Range&&) = delete;

    uint8_t depth() const {
        return depth_;
    }

    uint16_t bits() const {
        return bits_;
    }

    bool is_valid() const {
        return depth_ < 16;
    }

    std::vector<Range> parts() const {
        if(!is_valid()) {
            throw std::runtime_error("parts() called on invalid Range");
        }
        return {
            Range(static_cast<uint8_t>(depth_ + 1), static_cast<uint16_t>(bits_ << 1)),
            Range(static_cast<uint8_t>(depth_ + 1), static_cast<uint16_t>((bits_ << 1) | 1))
        };
    }

    template<IntervalType Interval>
    Interval interval() const {
        if(!is_valid()) {
            throw std::runtime_error("interval() called on invalid Range");
        }
        return Interval(static_cast<uint16_t>(bits_), static_cast<uint16_t>(bits_ + 1)) / Interval(1 << depth_);
    }

    template<IntervalType Interval>
    Interval angle_interval() const {
        if(!is_valid()) {
            throw std::runtime_error("angle_interval() called on invalid Range");
        }
        return Interval(2) * Interval::pi() * interval<Interval>();
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Range& id) {
        return ostream << "<" << std::bitset<16>(id.bits_).to_string().substr(16 - id.depth_) << ">";
    }

    uint16_t pack() const {
        if(!is_valid()) {
            return 0;
        }
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
