// #include "test/test.hpp"
#include "pipeline/pipeline.hpp"
#include <csignal>

using I = BoostInterval;

std::optional<Pipeline<I>> pipeline;

void signal_handler(const int signal) {
    if(pipeline.has_value()) {
        switch(signal) {
            case SIGTERM: {
                std::cout << "Received SIGTERM" << std::endl;
                break;
            }
            case SIGINT: {
                std::cout << "Received SIGINT" << std::endl;
                break;
            }
            default: {
                std::cout << "Received signal " << signal << std::endl;
            }
        }
        std::cout << "Stopping pipeline" << std::endl;
        pipeline->stop();
    }
}

template<typename... Args>
void print(Args&&... args) {
    ((std::cout << args), ...);
    std::cout << std::endl;
}

int main() {
    // tests();

    const Polyhedron<I> hole = Archimedean::rhombicosidodecahedron<I>();
    const Polyhedron<I> plug = Archimedean::rhombicosidodecahedron<I>();

    const Config<I> config(
        "temp",
        hole,
        plug,
        I::pi() / I(180),
        10,
        1000,
        10000,
        2,
        2,
        true,
        "../../web/static",
        1000000,
        false
    );

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    pipeline.emplace(config);
    pipeline->init();
    pipeline->start();
    print("Pipeline stopped gracefully");

    return 0;
}
