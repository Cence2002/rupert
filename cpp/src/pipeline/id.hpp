#pragma once

#include "interval/interval.hpp"
#include <cstdint>
#include <bitset>

struct Id {
private:
    uint64_t bits_;
    uint8_t depth_;

    static constexpr uint8_t overflow_depth = 32;

public:
    explicit Id() : bits_(0), depth_(0) {}

    explicit Id(const uint64_t bits, const uint8_t depth) : bits_(bits), depth_(depth) {}

    Id(const Id& other) = default;

    Id& operator=(const Id& other) = default;

    uint64_t bits() const {
        return bits_;
    }

    uint8_t depth() const {
        return depth_;
    }

    bool is_overflow() const {
        return depth_ >= overflow_depth;
    }

    double len() const {
        return std::pow(0.5, depth_);
    }

    Id min() const {
        if(is_overflow()) {
            throw std::runtime_error("Id overflow");
        }
        return Id(bits_ << 1, static_cast<uint8_t>(depth_ + 1));
    }

    Id max() const {
        if(is_overflow()) {
            throw std::runtime_error("Id overflow");
        }
        return Id((bits_ << 1) | static_cast<uint64_t>(1), static_cast<uint8_t>(depth_ + 1));
    }

    std::pair<Id, Id> split() const {
        if(is_overflow()) {
            throw std::runtime_error("Id overflow");
        }
        return std::make_pair(min(), max());
    }

    template<IntervalType Interval>
    Interval interval() const {
        if(is_overflow()) {
            throw std::runtime_error("Id overflow");
        }
        Interval interval = Interval::unit();
        for(uint8_t shift = depth_; shift > 0; shift--) {
            if(bits_ & (static_cast<uint64_t>(1) << (shift - 1))) {
                interval = Interval(interval.mid(), interval.max());
            } else {
                interval = Interval(interval.min(), interval.mid());
            }
        }
        return interval;
    }

    friend std::ostream& operator<<(std::ostream& os, const Id& id) {
        return os << "<" << std::bitset<64>(id.bits_).to_string().substr(64 - id.depth_) << ">";
    }
};
