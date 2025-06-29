// #include "test/test.hpp"
#include "global_solver/pipeline.hpp"
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

    const std::vector<Vector3<I>> hole_vertices = Archimedean::rhombicosidodecahedron<I>();
    const std::vector<Vector3<I>> plug_vertices = Archimedean::rhombicosidodecahedron<I>();
    const std::vector<Matrix<I>> hole_rotations = symmetries<I>(hole_vertices, true);
    const std::vector<Matrix<I>> hole_reflections = symmetries<I>(hole_vertices, false);
    const std::vector<Matrix<I>> plug_rotations = symmetries<I>(plug_vertices, true);
    const std::vector<Matrix<I>> plug_reflections = symmetries<I>(plug_vertices, false);
    const Polyhedron<I> hole(hole_vertices, hole_rotations, hole_reflections);
    const Polyhedron<I> plug(plug_vertices, plug_rotations, plug_reflections);
    const Polyhedron<I> hole_normalized = hole.normalized();
    const Polyhedron<I> plug_normalized = plug.normalized();

    const Config<I> config(
        "temp",
        hole_normalized,
        plug_normalized,
        I::pi() / I(180),
        10,
        1,
        10000,
        2,
        2,
        true,
        "../../web/static",
        100,
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
