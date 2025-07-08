#include "global_solver/global_solver.hpp"
#include <csignal>

using I = BoostInterval;

std::optional<GlobalSolver<I>> global_solver;

void signal_handler(const int signal) {
    if(global_solver.has_value()) {
        switch(signal) {
            case SIGINT: {
                std::cout << "Received SIGINT" << std::endl;
                break;
            }
            case SIGTERM: {
                std::cout << "Received SIGTERM" << std::endl;
                break;
            }
            default: {
                std::cout << "Received signal " << signal << std::endl;
            }
        }
        std::cout << "Interrupting Global Solver..." << std::endl;
        global_solver->stop();
    }
}

template<typename... Args>
void print(Args&&... args) {
    ((std::cout << args), ...);
    std::cout << std::endl;
}

int main() {
    std::signal(SIGINT, signal_handler);

    const Config config(
        Polyhedron(Archimedean::rhombicosidodecahedron<I>()),
        I::pi() / I(180),
        2,
        2,
        1,
        "../../web/static",
        "temp",
        100,
        true,
        true
    );
    config.validate();

    global_solver.emplace(config);
    global_solver->init();
    global_solver->start();
    print("Global Solver terminated gracefully");

    return 0;
}
