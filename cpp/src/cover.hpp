#pragma once

#include <bitset>
#include <boost/dynamic_bitset.hpp>
#include <queue>
#include "interval/interval.hpp"

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

struct Box2 {
private:
    Id theta_id_;
    Id phi_id_;

public:
    explicit Box2() : theta_id_(), phi_id_() {}

    explicit Box2(const Id& theta_id, const Id& phi_id) : theta_id_(theta_id), phi_id_(phi_id) {}

    Id theta_id() const {
        return theta_id_;
    }

    Id phi_id() const {
        return phi_id_;
    }

    bool is_overflow() const {
        return theta_id_.is_overflow() || phi_id_.is_overflow();
    }

    std::array<Box2, 4> split() const {
        const auto [theta_id_min, theta_id_max] = theta_id_.split();
        const auto [phi_id_min, phi_id_max] = phi_id_.split();
        return std::array<Box2, 4>{
            Box2(theta_id_min, phi_id_min),
            Box2(theta_id_min, phi_id_max),
            Box2(theta_id_max, phi_id_min),
            Box2(theta_id_max, phi_id_max)
        };
    }

    double size() const {
        return theta_id_.len() * phi_id_.len();
    }

    template<IntervalType Interval>
    Interval theta() const {
        return theta_id_.interval<Interval>() * 2 * Interval::pi();
    }

    template<IntervalType Interval>
    Interval phi() const {
        return phi_id_.interval<Interval>() * Interval::pi();
    }

    friend std::ostream& operator<<(std::ostream& os, const Box2& box2) {
        return os << "T" << box2.theta_id_ << " P" << box2.phi_id_;
    }
};

struct Box3 {
private:
    Id theta_id_;
    Id phi_id_;
    Id alpha_id_;

public:
    explicit Box3() : theta_id_(), phi_id_(), alpha_id_() {}

    explicit Box3(const Id& theta_id, const Id& phi_id, const Id& alpha_id) : theta_id_(theta_id), phi_id_(phi_id), alpha_id_(alpha_id) {}

    Id theta_id() const {
        return theta_id_;
    }

    Id phi_id() const {
        return phi_id_;
    }

    Id alpha_id() const {
        return alpha_id_;
    }

    bool is_overflow() const {
        return theta_id_.is_overflow() || phi_id_.is_overflow() || alpha_id_.is_overflow();
    }

    std::array<Box3, 8> split() const {
        const auto [theta_id_min, theta_id_max] = theta_id_.split();
        const auto [phi_id_min, phi_id_max] = phi_id_.split();
        const auto [alpha_id_min, alpha_id_max] = alpha_id_.split();
        return std::array<Box3, 8>{
            Box3(theta_id_min, phi_id_min, alpha_id_min),
            Box3(theta_id_min, phi_id_min, alpha_id_max),
            Box3(theta_id_min, phi_id_max, alpha_id_min),
            Box3(theta_id_min, phi_id_max, alpha_id_max),
            Box3(theta_id_max, phi_id_min, alpha_id_min),
            Box3(theta_id_max, phi_id_min, alpha_id_max),
            Box3(theta_id_max, phi_id_max, alpha_id_min),
            Box3(theta_id_max, phi_id_max, alpha_id_max)
        };
    }

    double size() const {
        return theta_id_.len() * phi_id_.len() * alpha_id_.len();
    }

    template<IntervalType Interval>
    Interval theta() const {
        return theta_id_.interval<Interval>() * 2 * Interval::pi();
    }

    template<IntervalType Interval>
    Interval phi() const {
        return phi_id_.interval<Interval>() * Interval::pi();
    }

    template<IntervalType Interval>
    Interval alpha() const {
        return alpha_id_.interval<Interval>() * 2 * Interval::pi();
    }

    friend std::ostream& operator<<(std::ostream& os, const Box3& box3) {
        return os << "T" << box3.theta_id_ << " P" << box3.phi_id_ << " A" << box3.alpha_id_;
    }
};

struct Queue2 {
private:
    std::queue<Box2> queue_;

public:
    explicit Queue2() : queue_() {
        push(Box2());
    }

    bool empty() const {
        return queue_.empty();
    }

    void push(const Box2& box2) {
        queue_.push(box2);
    }

    std::optional<Box2> pop() {
        if(empty()) {
            return std::nullopt;
        }
        const Box2 box2 = queue_.front();
        queue_.pop();
        return std::make_optional(box2);
    }
};

struct Queue3 {
private:
    std::queue<Box3> queue_;
    std::mutex mutex_;

public:
    explicit Queue3() : queue_(), mutex_() {
        // push(Box3());

        //start from a small subset of the space
        push(Box3(
            Id(0b0101, 4),
            Id(0b0011, 4),
            Id(0b1010, 4)
        ));

        // Terminal Box: T<000000> P<000011> A<000001>
        // push(Box3(
        //     Id(0b000000, 6),
        //     Id(0b000011, 6),
        //     Id(0b000001, 6)
        // ));
    }

    void push(const Box3& box) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(box);
    }

    std::optional<Box3> pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if(queue_.empty()) {
            return std::nullopt;
        }
        const Box3 box = queue_.front();
        queue_.pop();
        return std::make_optional(box);
    }
};
