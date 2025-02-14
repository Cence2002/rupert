#pragma once

#include <bitset>
#include <boost/dynamic_bitset.hpp>
#include <queue>
#include "interval/interval.hpp"

struct Id {
private:
    uint64_t bits_;
    uint8_t depth_;

public:
    explicit Id() : bits_(0), depth_(0) {}

    explicit Id(const uint64_t& bits, const uint8_t& depth) : bits_(bits), depth_(depth) {}

    Id(const Id& other) = default;

    Id& operator=(const Id& other) = default;

    uint64_t bits() const {
        return bits_;
    }

    uint8_t depth() const {
        return depth_;
    }

    double len() const {
        return 1.0 / static_cast<double>(1ull << depth_);
    }

    Id min() const {
        if(depth_ == 64) {
            throw std::runtime_error("Id overflow");
        }
        return Id(bits_ << 1, depth_ + 1);
    }

    Id max() const {
        if(depth_ == 64) {
            throw std::runtime_error("Id overflow");
        }
        return Id((bits_ << 1) | 1ull, depth_ + 1);
    }

    std::pair<Id, Id> split() const {
        if(depth_ == 64) {
            throw std::runtime_error("Id overflow");
        }
        return std::make_pair(min(), max());
    }

    template<IntervalType Interval>
    Interval to_interval() const {
        Interval interval = Interval::unit();
        for(uint8_t i = 0; i < depth_; i++) {
            if(bits_ & (1ull << (depth_ - 1 - i))) {
                interval = (interval - interval.max()) / 2 + interval.max();
            } else {
                interval = (interval - interval.min()) / 2 + interval.min();
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
    Id phi_id_;
    Id theta_id_;

public:
    explicit Box2() : phi_id_(), theta_id_() {}

    explicit Box2(const Id& phi_id, const Id& theta_id) : phi_id_(phi_id), theta_id_(theta_id) {}

    Id phi_id() const {
        return phi_id_;
    }

    Id theta_id() const {
        return theta_id_;
    }

    std::array<Box2, 4> split() const {
        const auto [phi_id_min, phi_id_max] = phi_id_.split();
        const auto [theta_id_min, theta_id_max] = theta_id_.split();
        return std::array<Box2, 4>{
                Box2(phi_id_min, theta_id_min),
                Box2(phi_id_min, theta_id_max),
                Box2(phi_id_max, theta_id_min),
                Box2(phi_id_max, theta_id_max)
            };
    }

    double size() const {
        return phi_id_.len() * theta_id_.len();
    }

    template<IntervalType Interval>
    Interval phi() const {
        return phi_id_.to_interval<Interval>();
    }

    template<IntervalType Interval>
    Interval theta() const {
        return theta_id_.to_interval<Interval>();
    }

    friend std::ostream& operator<<(std::ostream& os, const Box2& box2) {
        return os << "P" << box2.phi_id_ << " T" << box2.theta_id_;
    }
};

struct Box3 {
private:
    Id phi_id_;
    Id theta_id_;
    Id alpha_id_;

public:
    explicit Box3() : phi_id_(), theta_id_(), alpha_id_() {}

    explicit Box3(const Id& phi_id, const Id& theta_id, const Id& alpha_id) : phi_id_(phi_id), theta_id_(theta_id), alpha_id_(alpha_id) {}

    Id phi_id() const {
        return phi_id_;
    }

    Id theta_id() const {
        return theta_id_;
    }

    Id alpha_id() const {
        return alpha_id_;
    }

    std::array<Box3, 8> split() const {
        const auto [phi_id_min, phi_id_max] = phi_id_.split();
        const auto [theta_id_min, theta_id_max] = theta_id_.split();
        const auto [alpha_id_min, alpha_id_max] = alpha_id_.split();
        return std::array<Box3, 8>{
                Box3(phi_id_min, theta_id_min, alpha_id_min),
                Box3(phi_id_min, theta_id_min, alpha_id_max),
                Box3(phi_id_min, theta_id_max, alpha_id_min),
                Box3(phi_id_min, theta_id_max, alpha_id_max),
                Box3(phi_id_max, theta_id_min, alpha_id_min),
                Box3(phi_id_max, theta_id_min, alpha_id_max),
                Box3(phi_id_max, theta_id_max, alpha_id_min),
                Box3(phi_id_max, theta_id_max, alpha_id_max)
            };
    }

    double size() const {
        return phi_id_.len() * theta_id_.len() * alpha_id_.len();
    }

    template<IntervalType Interval>
    Interval phi() const {
        return phi_id_.to_interval<Interval>();
    }

    template<IntervalType Interval>
    Interval theta() const {
        return theta_id_.to_interval<Interval>();
    }

    template<IntervalType Interval>
    Interval alpha() const {
        return alpha_id_.to_interval<Interval>();
    }

    friend std::ostream& operator<<(std::ostream& os, const Box3& box3) {
        return os << "P" << box3.phi_id_ << " T" << box3.theta_id_ << " A" << box3.alpha_id_;
    }
};

struct Queue2 {
private:
    std::queue<Box2> queue;

public:
    explicit Queue2() : queue() {
        push(Box2());
    }

    bool empty() const {
        return queue.empty();
    }

    void push(const Box2& box2) {
        queue.push(box2);
    }

    std::optional<Box2> pop() {
        if(empty()) {
            return std::nullopt;
        }
        const Box2 box2 = queue.front();
        queue.pop();
        return std::make_optional(box2);
    }
};

struct Queue3 {
private:
    std::queue<Box3> queue;
    std::mutex mutex;

public:
    explicit Queue3() : queue(), mutex() {
        // push(Box3(
        //     Id(0b0, 5),
        //     Id(0b0100, 4),
        //     Id(0b0, 5)
        // ));
        push(Box3());
    }

    void push(const Box3& box) {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(box);
    }

    std::optional<Box3> pop() {
        std::lock_guard<std::mutex> lock(mutex);
        if(queue.empty()) {
            return std::nullopt;
        }
        const Box3 box = queue.front();
        queue.pop();
        return std::make_optional(box);
    }
};
