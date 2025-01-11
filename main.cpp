#include "src/vector.hpp"
#include "src/interval.hpp"
#include "src/queue.hpp"
#include "src/plot.hpp"
#include "src/polyhedron.hpp"
#include "src/test.hpp"
#include <iostream>

int main() {
    std::vector<Vector3d> polyhedron = Polyhedron::rhombicosidodecahedron();

    Queue3 queue3(10, 10, 10);
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> threads;
    const int thread_count = 10;
    for(int i = 0; i < thread_count; i++) {
        threads.emplace_back([&] {
            for(int j = 0; j < 20 / thread_count; j++) {
                queue3.process(polyhedron, 5, 20000, 100);
            }
        });
    }
    for(auto &thread: threads) {
        thread.join();
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << "s" << std::endl;
    // Takes approximately 10s

    return 0;
}
