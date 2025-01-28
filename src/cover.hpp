#pragma once

#include <bitset>
#include <boost/dynamic_bitset.hpp>
#include <queue>
#include "interval.hpp"

struct Id {
private:
    uint64_t bits{};
    uint8_t depth{};

public:
    explicit Id() = default;

    explicit Id(const uint64_t bits, const uint8_t depth) : bits(bits), depth(depth) {}

    Id(const Id &other) = default;

    Id &operator=(const Id &other) = default;

    double len() const {
        return 1.0 / static_cast<double>(1ull << depth);
    }

    Id left() const {
        if(depth == 64) {
            throw std::runtime_error("Id overflow");
        }
        return Id(bits << 1, depth + 1);
    }

    Id right() const {
        if(depth == 64) {
            throw std::runtime_error("Id overflow");
        }
        return Id((bits << 1) | 1ull, depth + 1);
    }

    std::pair<Id, Id> split() const {
        if(depth == 64) {
            throw std::runtime_error("Id overflow");
        }
        return std::make_pair(left(), right());
    }

    template<IntervalType I>
    I to_interval() const {
        I interval = I::unit();
        for(uint8_t i = 0; i < depth; i++) {
            if(bits & (1ull << (depth - 1 - i))) {
                interval = (interval - interval.max()) / 2 + interval.max();
            } else {
                interval = (interval - interval.min()) / 2 + interval.min();
            }
        }
        return interval;
    }

    friend std::ostream &operator<<(std::ostream &os, const Id &id) {
        return os << "<" << std::bitset<64>(id.bits).to_string().substr(64u - id.depth) << ">";
    }
};

struct Box2 {
    Id theta_id;
    Id phi_id;

    explicit Box2() = default;

    explicit Box2(const Id &theta_id, const Id &phi_id) : theta_id(theta_id), phi_id(phi_id) {}

    std::array<Box2, 4> split() const {
        const auto [theta_id_left, theta_id_right] = theta_id.split();
        const auto [phi_id_left, phi_id_right] = phi_id.split();
        return std::array<Box2, 4>{
                    Box2(theta_id_left, phi_id_left),
                    Box2(theta_id_left, phi_id_right),
                    Box2(theta_id_right, phi_id_left),
                    Box2(theta_id_right, phi_id_right)
                };
    }

    double size() const {
        return theta_id.len() * phi_id.len();
    }

    template<IntervalType I>
    I theta() const {
        return theta_id.to_interval<I>();
    }

    template<IntervalType I>
    I phi() const {
        return phi_id.to_interval<I>();
    }

    friend std::ostream &operator<<(std::ostream &os, const Box2 &box2) {
        return os << "T" << box2.theta_id << " P" << box2.phi_id;
    }
};

struct Box3 {
    Id theta_id;
    Id phi_id;
    Id alpha_id;

    explicit Box3() = default;

    explicit Box3(const Id &theta_id, const Id &phi_id, const Id &alpha_id) : theta_id(theta_id), phi_id(phi_id), alpha_id(alpha_id) {}

    std::array<Box3, 8> split() const {
        const auto [theta_id_left, theta_id_right] = theta_id.split();
        const auto [phi_id_left, phi_id_right] = phi_id.split();
        const auto [alpha_id_left, alpha_id_right] = alpha_id.split();
        return std::array<Box3, 8>{
                    Box3(theta_id_left, phi_id_left, alpha_id_left),
                    Box3(theta_id_left, phi_id_left, alpha_id_right),
                    Box3(theta_id_left, phi_id_right, alpha_id_left),
                    Box3(theta_id_left, phi_id_right, alpha_id_right),
                    Box3(theta_id_right, phi_id_left, alpha_id_left),
                    Box3(theta_id_right, phi_id_left, alpha_id_right),
                    Box3(theta_id_right, phi_id_right, alpha_id_left),
                    Box3(theta_id_right, phi_id_right, alpha_id_right)
                };
    }

    double size() const {
        return theta_id.len() * phi_id.len() * alpha_id.len();
    }

    template<IntervalType I>
    I theta() const {
        return theta_id.to_interval<I>();
    }

    template<IntervalType I>
    I phi() const {
        return phi_id.to_interval<I>();
    }

    template<IntervalType I>
    I alpha() const {
        return alpha_id.to_interval<I>();
    }

    friend std::ostream &operator<<(std::ostream &os, const Box3 &box3) {
        return os << "T" << box3.theta_id << " P" << box3.phi_id << " A" << box3.alpha_id;
    }
};

struct Queue2 {
private:
    std::queue<Box2> queue;

public:
    explicit Queue2() {
        push(Box2());
    }

    bool empty() const {
        return queue.empty();
    }

    void push(const Box2 &box2) {
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
    explicit Queue3() {
        push(Box3(
            Id(0b101u, 3),
            Id(0b101u, 3),
            Id(0b101u, 3)
        ));
    }

    void push(const Box3 &box) {
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
