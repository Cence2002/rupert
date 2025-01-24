#pragma once

#include <boost/dynamic_bitset.hpp>
#include <queue>

class Id {
    boost::dynamic_bitset<> bits;

public:
    explicit Id() : bits() {}

    explicit Id(const boost::dynamic_bitset<> &bits) : bits(bits) {}

    Id operator+(const bool bit) const {
        boost::dynamic_bitset<> new_bits = bits;
        new_bits.push_back(bit);
        return Id(new_bits);
    }

    friend std::ostream &operator<<(std::ostream &os, const Id &id) {
        if(id.bits.empty()) {
            return os << "-";
        }
        for(size_t i = id.bits.size(); i > 0; i--) {
            os << (id.bits[i - 1] ? "1" : "0");
        }
        return os;
    }
};

class Range {
public:
    Id id;
    double min;
    double max;

    explicit Range(const Id &id, const double min, const double max) : id(id), min(min), max(max) {}

    double mid() const {
        return (min + max) / 2;
    }

    std::array<Range, 2> split() const {
        const double mid = (min + max) / 2;
        return std::array<Range, 2>{
                    Range(id + false, min, mid),
                    Range(id + true, mid, max)
                };
    }

    double len() const {
        return max - min;
    }

    friend std::ostream &operator<<(std::ostream &os, const Range &range) {
        return os <<
               "(" << range.id << ")" <<
               " => " <<
               "[" << range.min << ", " << range.max << "]";
    }
};

class Box2 {
public:
    Range theta;
    Range phi;

    explicit Box2(const Range &theta, const Range &phi) : theta(theta), phi(phi) {}

    static Box2 init() {
        return Box2(
            Range(Id(), 0, pi_twice_upper),
            Range(Id(), 0, pi_upper)
        );
    }

    std::array<Box2, 4> split() const {
        const auto [theta0, theta1] = theta.split();
        const auto [phi0, phi1] = phi.split();
        return std::array<Box2, 4>{
                    Box2(theta0, phi0),
                    Box2(theta0, phi1),
                    Box2(theta1, phi0),
                    Box2(theta1, phi1)
                };
    }

    double size() const {
        return theta.len() * phi.len();
    }

    friend std::ostream &operator<<(std::ostream &os, const Box2 &box2) {
        return os << "{" << std::endl <<
               "  " << box2.theta << std::endl <<
               "  " << box2.phi << std::endl <<
               "}";
    }
};

class Box3 {
public:
    Range theta;
    Range phi;
    Range alpha;

    explicit Box3(const Range &theta, const Range &phi, const Range &alpha) : theta(theta), phi(phi), alpha(alpha) {}

    static Box3 init() {
        return Box3(
            Range(Id(), 0, pi_twice_upper),
            Range(Id(), 0, pi_upper),
            Range(Id(), 0, pi_twice_upper)
        );
    }

    std::array<Box3, 8> split() const {
        const auto [theta0, theta1] = theta.split();
        const auto [phi0, phi1] = phi.split();
        const auto [alpha0, alpha1] = alpha.split();
        return std::array<Box3, 8>{
                    Box3(theta0, phi0, alpha0),
                    Box3(theta0, phi0, alpha1),
                    Box3(theta0, phi1, alpha0),
                    Box3(theta0, phi1, alpha1),
                    Box3(theta1, phi0, alpha0),
                    Box3(theta1, phi0, alpha1),
                    Box3(theta1, phi1, alpha0),
                    Box3(theta1, phi1, alpha1)
                };
    }

    double size() const {
        return theta.len() * phi.len() * alpha.len();
    }

    friend std::ostream &operator<<(std::ostream &os, const Box3 &box3) {
        return os << "{" << std::endl <<
               "  " << box3.theta << std::endl <<
               "  " << box3.phi << std::endl <<
               "  " << box3.alpha << std::endl <<
               "}";
    }
};

class Queue2 {
    std::queue<Box2> queue;

public:
    explicit Queue2() {
        push(Box2::init());
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

class Queue3 {
    std::queue<Box3> queue;
    std::mutex mutex;

public:
    explicit Queue3() {
        push(Box3::init());
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
