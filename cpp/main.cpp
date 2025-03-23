#include "test/test.hpp"
#include "pipeline/pipeline.hpp"
#include <csignal>

using I = FloatInterval;

std::optional<Pipeline<FloatInterval>> pipeline;

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

int main() {
    tests();

    const Polyhedron<I> plug = Archimedean::rhombicosidodecahedron<I>().normalise();
    const Polyhedron<I> hole = Archimedean::rhombicosidodecahedron<I>().normalise();

    const Config<I> config(
        "RID",
        plug,
        hole,
        1,
        1000000,
        10000,
        2,
        2,
        true,
        "../../web/static",
        100000,
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
