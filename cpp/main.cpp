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
        global_solver->interrupt();
    }
}

void run_global_solver(const Config<I>& config) {
    config.validate();

    global_solver.emplace(config);
    global_solver->run();
    std::cout << "Global Solver terminated gracefully" << std::endl;
}

int main() {
    std::signal(SIGINT, signal_handler);

    run_global_solver(Config(
        Polyhedron(Platonic::cube<I>()),
        I::pi() / I(36),
        2,
        2,
        10,
        "../../web/static",
        "temp",
        100
    ));

    return 0;
}
