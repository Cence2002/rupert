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
    const std::vector<Vector3<I>> polyhedron_vertices = Archimedean::rhombicosidodecahedron<I>();
    const std::vector<Matrix<I>> polyhedron_rotations = symmetries<I>(polyhedron_vertices, true);
    const std::vector<Matrix<I>> polyhedron_reflections = symmetries<I>(polyhedron_vertices, false);
    const Polyhedron<I> hole(polyhedron_vertices, polyhedron_rotations, polyhedron_reflections);
    const Polyhedron<I> polyhedron_normalized = hole.normalized();

    const Config<I> config{
        polyhedron_normalized,
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

    pipeline.emplace(config);
    pipeline->init();
    pipeline->start();
    print("Pipeline stopped gracefully");

    return 0;
}
