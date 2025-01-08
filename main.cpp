#include "src/vector.hpp"
#include "src/interval.hpp"
#include "src/queue.hpp"
#include "src/plot.hpp"
#include "src/polyhedron.hpp"
#include "src/test.hpp"
#include <iostream>

std::vector<Vector3d> polyhedron = Polyhedron::rhombicosidodecahedron();

Plot plot_main("main", 1000, 1000, {-1.5, 1.5}, {-1.5, 1.5});
Plot plot_boxes("boxes", 1000, 1000, {0, 1}, {0, 1});

std::vector<Vector2d> hole;
Queue2 boxes;

void test() {
    test_angle();

    std::cout << "All tests passed" << std::endl;
}

void setup() {
    plot_main.clear();
    plot_boxes.clear();
    plot_main.move({100, 100});
    plot_boxes.move({1150, 100});
    plot_boxes.show();
    plot_main.show();

    Interval hole_alpha(0);
    Interval hole_theta(0);
    Interval hole_phi(-0.01, 0.01);
    std::vector<Vector2d> hole_all;
    for(const Interval &alpha: divide(hole_alpha, 10)) {
        for(const Interval &theta: divide(hole_theta, 10)) {
            for(const Interval &phi: divide(hole_phi, 10)) {
                for(const Vector3d &vertex: polyhedron) {
                    const Vector2I v_project = Vector3I(vertex.x, vertex.y, vertex.z).project(theta, phi);
                    const auto [x, y] = v_project.rotate(alpha);
                    hole_all.emplace_back(lower(x), lower(y));
                    hole_all.emplace_back(lower(x), upper(y));
                    hole_all.emplace_back(upper(x), lower(y));
                    hole_all.emplace_back(upper(x), upper(y));
                }
            }
        }
    }
    hole = convex_hull(hole_all);
    std::ranges::sort(hole, [](const Vector2d &v0, const Vector2d &v1) {
        return v0.angle() < v1.angle();
    });
}

void process() {
    if(boxes.empty()) {
        return;
    }
    const Box2 box = boxes.fetch();
    for(const Vector3d &vertex: polyhedron) {
        if(!box_intersects_polygon(vertex, box, hole)) {
            return;
        }
    }
    boxes.push_sub_boxes(box);
}

void plot() {
    plot_main.clear();
    plot_boxes.clear();

    plot_main.line(Vector2d(-10, 0), Vector2d(10, 0), Colors::GRAY);
    plot_main.line(Vector2d(0, -10), Vector2d(0, 10), Colors::GRAY);
    plot_main.circle(Vector2d(0, 0), 1, Colors::GRAY);
    plot_main.polygon(hole, Colors::CYAN);

    for(const Box2 &box: boxes.vector()) {
        if(std::max(width(box.theta), width(box.phi)) < 0.01) {
            plot_boxes.circle(box.normalized().center(), 0.1, gradient(box) / 2);
        }
    }
    for(const Box2 &box: boxes.vector()) {
        plot_boxes.filled_box(box.normalized(), gradient(box) / 2);
        plot_boxes.box(box.normalized(), gradient(box));
    }
    plot_boxes.show();
    plot_main.show();
}

void draw() {
    while(Plot::step(1)) {
        for(int t = 0; t < 100; t++) {
            process();
        }
        plot();
        std::cout << boxes.size() << std::endl;
        if(boxes.empty()) {
            break;
        }
    }
}

void exit() {
    plot_main.save("../outputs/main.png");
    plot_boxes.save("../outputs/boxes.png");
    cv::destroyAllWindows();
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
