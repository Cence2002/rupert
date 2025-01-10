#include "src/vector.hpp"
#include "src/interval.hpp"
#include "src/queue.hpp"
#include "src/plot.hpp"
#include "src/polyhedron.hpp"
#include "src/test.hpp"
#include <iostream>

std::vector<Vector3d> polyhedron = Polyhedron::rhombicosidodecahedron();

// Plot plot_main("main", 1000, 1000, {-1.5, 1.5}, {-1.5, 1.5});
// Plot plot_queue("queue", 1000, 1000, {0, 1}, {0, 1});

Queue3 queue3;

void test() {
    test_angle();

    std::cout << "All tests passed" << std::endl;
}

void setup() {
    // plot_main.clear();
    // plot_queue.clear();
    // plot_main.move({100, 100});
    // plot_queue.move({1150, 100});
    // plot_queue.show();
    // plot_main.show();
}

void draw() {
    while(!queue3.empty()) {
        queue3.process(polyhedron, 5, 20000, 20, 20);
    }
}

void exit() {
    // plot_main.save("../outputs/main.png");
    // plot_queue.save("../outputs/queue.png");
    // cv::destroyAllWindows();
}

int main() {
    test();
    setup();
    draw();
    exit();
    return 0;
}

// #include <opencv2/viz.hpp>
// #include <opencv2/viz/widgets.hpp>
// #include <opencv2/core.hpp>"
//
// int main() {
//     cv::viz::Viz3d window("3D Scene");
//     window.showWidget("Line", cv::viz::WLine(cv::Point3d(0, 0, 0), cv::Point3d(1, 1, 1), cv::viz::Colors::red()));
//     window.showWidget("CoordinateSystem", cv::viz::WCoordinateSystem(1.0));
//     window.spin();
//     return 0;
// }
