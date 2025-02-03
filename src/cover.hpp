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

    template<IntervalType I>
    I to_interval() const {
        I interval = I::unit();
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
        return os << "<" << std::bitset<64>(id.bits_).to_string().substr(64u - id.depth_) << ">";
    }
};

struct Box2 {
private:
    Id theta_id_;
    Id phi_id_;

public:
    explicit Box2() : theta_id_(), phi_id_() {}

    explicit Box2(const Id& theta_id, const Id& phi_id) : theta_id_(theta_id), phi_id_(phi_id) {}

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

    template<IntervalType I>
    I theta() const {
        return theta_id_.to_interval<I>();
    }

    template<IntervalType I>
    I phi() const {
        return phi_id_.to_interval<I>();
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

    template<IntervalType I>
    I theta() const {
        return theta_id_.to_interval<I>();
    }

    template<IntervalType I>
    I phi() const {
        return phi_id_.to_interval<I>();
    }

    template<IntervalType I>
    I alpha() const {
        return alpha_id_.to_interval<I>();
    }

    friend std::ostream& operator<<(std::ostream& os, const Box3& box3) {
        return os << "T" << box3.theta_id_ << " P" << box3.phi_id_ << " A" << box3.alpha_id_;
    }
};

struct Queue2 {
private:
    std::queue<Box2> queue{};

public:
    explicit Queue2() {
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
    std::queue<Box3> queue{};
    std::mutex mutex{};

public:
    explicit Queue3() {
        push(Box3(
            Id(0b101u, 3),
            Id(0b101u, 3),
            Id(0b101u, 3)
        ));
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
