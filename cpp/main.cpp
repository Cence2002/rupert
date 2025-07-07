#include "global_solver/global_solver.hpp"
#include <csignal>

using I = BoostInterval;

std::optional<GlobalSolver<I>> pipeline;

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
    const Polyhedron<I> polyhedron(Archimedean::rhombicosidodecahedron<I>());

    const Config<I> config{
        polyhedron,
        I::pi() / I(180),
        1,
        10000,
        2,
        2,
        1,
        "../../web/static",
        "temp",
        100,
        true,
        true
    };
    config.validate();

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    return 0;

    pipeline.emplace(config);
    pipeline->init();
    pipeline->start();
    print("Pipeline stopped gracefully");

    return 0;
}
