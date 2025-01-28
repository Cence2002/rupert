#pragma once

#include <iostream>
#include <chrono>

inline void print() {
    std::cout << std::endl;
}

template<typename T, typename... Args>
void print(T t, Args... args) {
    std::cout << t;
    if constexpr(sizeof...(args) > 0) {
        std::cout << " ";
        print(args...);
    } else {
        std::cout << std::endl;
    }
}

inline std::chrono::time_point<std::chrono::high_resolution_clock> current_time() {
    return std::chrono::high_resolution_clock::now();
}

inline std::chrono::duration<double>::rep elapsed_time(const std::chrono::time_point<std::chrono::high_resolution_clock> &start) {
    return std::chrono::duration<double>(current_time() - start).count();
}

inline std::chrono::duration<long>::rep elapsed_microseconds(const std::chrono::time_point<std::chrono::high_resolution_clock> &start) {
    return std::chrono::duration_cast<std::chrono::microseconds>(current_time() - start).count();
}

inline std::chrono::duration<long>::rep elapsed_milliseconds(const std::chrono::time_point<std::chrono::high_resolution_clock> &start) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(current_time() - start).count();
}

inline std::chrono::duration<long>::rep elapsed_seconds(const std::chrono::time_point<std::chrono::high_resolution_clock> &start) {
    return std::chrono::duration_cast<std::chrono::seconds>(current_time() - start).count();
}
