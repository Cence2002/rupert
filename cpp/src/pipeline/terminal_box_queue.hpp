#pragma once

#include "terminal_box.hpp"
#include <queue>
#include <mutex>

struct TerminalBoxQueue {
private:
    std::queue<TerminalBox> queue_;
    std::mutex mutex_;
    size_t size_;

public:
    size_t size() const;

    void push(const TerminalBox& terminal_box);

    std::vector<TerminalBox> flush();
};