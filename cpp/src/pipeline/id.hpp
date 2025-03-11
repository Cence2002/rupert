#pragma once

#include "interval/interval.hpp"
#include <cstdint>
#include <bitset>
#include <cstring>

struct Id {
private:
    uint8_t depth_;
    uint16_t bits_;

public:
    explicit Id() : depth_(0), bits_(0) {}

    explicit Id(const uint8_t depth, const uint16_t bits) : depth_(depth), bits_(bits) {}

    Id(const Id& other) = default;

    Id& operator=(const Id& other) = default;

    Id(Id&& other) = default;

    Id& operator=(Id&& other) = default;

    uint8_t depth() const {
        return depth_;
    }

    uint64_t bits() const {
        return bits_;
    }

    static Id invalid() {
        return Id(16, 0);
    }

    bool is_invalid() const {
        return depth_ >= 16;
    }

    static size_t size() {
        return sizeof(uint16_t);
    }

    Id min_half() const {
        if(is_invalid()) {
            throw std::runtime_error("Invalid Id");
        }
        return Id(static_cast<uint8_t>(depth_ + 1), static_cast<uint16_t>(bits_ << 1));
    }

    Id max_half() const {
        if(is_invalid()) {
            throw std::runtime_error("Invalid Id");
        }
        return Id(static_cast<uint8_t>(depth_ + 1), static_cast<uint16_t>((bits_ << 1) | 1));
    }

    std::pair<Id, Id> subdivide() const {
        if(is_invalid()) {
            throw std::runtime_error("Invalid Id");
        }
        return std::make_pair(min_half(), max_half());
    }

    template<IntervalType Interval>
    Interval interval() const {
        if(is_invalid()) {
            throw std::runtime_error("Invalid Id");
        }
        return Interval(static_cast<uint16_t>(bits_), static_cast<uint16_t>(bits_ + 1)) / (1 << depth_);
    }

    friend std::ostream& operator<<(std::ostream& os, const Id& id) {
        return os << "<" << std::bitset<16>(id.bits_).to_string().substr(16 - id.depth_) << ">";
    }

    uint16_t pack() const {
        if(is_invalid()) {
            return 0;
        }
        return static_cast<uint16_t>(1 << depth_) | bits_;
    }

    static Id unpack(const uint16_t packed) {
        if(packed == 0) {
            return Id(16, 0);
        }
        const uint8_t depth = static_cast<uint8_t>(31 - __builtin_clz(packed));
        const uint16_t bits = packed & static_cast<uint16_t>((1 << depth) - 1);
        return Id(depth, bits);
    }

    void to_stream(std::ostream& os) const {
        const uint16_t packed = pack();
        os.write(reinterpret_cast<const char*>(&packed), sizeof(packed));
    }

    static Id from_stream(std::istream& is) {
        uint16_t packed;
        is.read(reinterpret_cast<char*>(&packed), sizeof(packed));
        if(is.gcount() != sizeof(packed)) {
            throw std::runtime_error("Failed to read Id from stream");
        }
        return unpack(packed);
    }
};
