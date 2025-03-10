#pragma once

#include "interval/interval.hpp"
#include <cstdint>
#include <bitset>

struct Id {
private:
    uint16_t bits_;
    uint8_t depth_;

public:
    explicit Id() : bits_(0), depth_(0) {}

    explicit Id(const uint16_t bits, const uint8_t depth) : bits_(bits), depth_(depth) {}

    Id(const Id& other) = default;

    Id& operator=(const Id& other) = default;

    Id(Id&& other) = default;

    Id& operator=(Id&& other) = default;

    uint64_t bits() const {
        return bits_;
    }

    uint8_t depth() const {
        return depth_;
    }

    bool invalid() const {
        return depth_ >= 16;
    }

    double size() const {
        return std::pow(0.5, depth_);
    }

    Id min_half() const {
        if(invalid()) {
            throw std::runtime_error("Invalid Id");
        }
        return Id(static_cast<uint16_t>(bits_ << 1), static_cast<uint8_t>(depth_ + 1));
    }

    Id max_half() const {
        if(invalid()) {
            throw std::runtime_error("Invalid Id");
        }
        return Id(static_cast<uint16_t>((bits_ << 1) | 1), static_cast<uint8_t>(depth_ + 1));
    }

    std::pair<Id, Id> subdivide() const {
        if(invalid()) {
            throw std::runtime_error("Invalid Id");
        }
        return std::make_pair(min_half(), max_half());
    }

    template<IntervalType Interval>
    Interval interval() const {
        if(invalid()) {
            throw std::runtime_error("Invalid Id");
        }
        return Interval(static_cast<uint16_t>(bits_), static_cast<uint16_t>(bits_ + 1)) / (static_cast<uint16_t>(1) << depth_);
    }

    friend std::ostream& operator<<(std::ostream& os, const Id& id) {
        return os << "<" << std::bitset<16>(id.bits_).to_string().substr(16 - id.depth_) << ">";
    }
};
