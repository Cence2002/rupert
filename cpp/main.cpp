#include "geometry/geometry.hpp"
#include "pipeline/pipeline.hpp"
#include "test/test.hpp"

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

    Pipeline<Interval> pipeline(config);
    pipeline.init();
    pipeline.start();

    return 0;
}
