#include "test/test.hpp"
#include "pipeline/pipeline.hpp"
#include <csignal>

using IntervalMode = FloatInterval;

std::optional<Pipeline<FloatInterval>> pipeline;

void signal_handler(int) {
    if(pipeline.has_value()) {
        std::cout << "Stopping pipeline" << std::endl;
        pipeline->stop();
    }
}

int main() {
    tests();

    using Interval = FloatInterval;

    const Polyhedron<Interval> hole = Polyhedron<Interval>::rhombicosidodecahedron();
    const Polyhedron<Interval> plug = Polyhedron<Interval>::rhombicosidodecahedron();

    const Config<Interval> config(
        "RID",
        hole,
        plug,
        1,
        64,
        10000,
        2,
        2,
        100000,
        "../../web/static"
    );

    std::signal(SIGINT, signal_handler);

    pipeline.emplace(config);
    pipeline->init();
    pipeline->start();
    print("Pipeline stopped gracefully");

    return 0;
}
