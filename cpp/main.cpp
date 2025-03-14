#include "test/test.hpp"
#include "pipeline/pipeline.hpp"
#include <csignal>

using I = FloatInterval;

std::optional<Pipeline<FloatInterval>> pipeline;

void signal_handler(int) {
    if(pipeline.has_value()) {
        std::cout << "Stopping pipeline" << std::endl;
        pipeline->stop();
    }
}

int main() {
    tests();

    const Polyhedron<I> hole = Polyhedra::rhombicosidodecahedron<I>();
    const Polyhedron<I> plug = Polyhedra::rhombicosidodecahedron<I>();

    const Config<I> config(
        "RID",
        hole,
        plug,
        1,
        64,
        10000,
        2,
        2,
        true,
        "../../web/static",
        100000,
        true
    );

    std::signal(SIGINT, signal_handler);

    pipeline.emplace(config);
    pipeline->init();
    pipeline->start();
    print("Pipeline stopped gracefully");

    return 0;
}
