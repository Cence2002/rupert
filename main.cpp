#include "src/vector.hpp"
#include "src/interval.hpp"
#include "src/plot.hpp"
#include "src/polyhedron.hpp"
#include "src/queue.hpp"
#include "src/test.hpp"

#include <iostream>

std::vector<Vector3d> polyhedron = Polyhedron::rhombicosidodecahedron();

Plot plot_main("main", 1000);
Plot plot_boxes("boxes", 1000);

void test() {
    test_angle();
    test_intersections();

    std::cout << "All tests passed" << std::endl;
}

void setup() {
    plot_main.line(Vector2d(-10, 0), Vector2d(10, 0), Color::GRAY);
    plot_main.line(Vector2d(0, -10), Vector2d(0, 10), Color::GRAY);
    plot_main.circle(Vector2d(0, 0), 1, Color::GRAY);

    std::cout << "Vertices: " << polyhedron.size() << std::endl;
}

void draw() {
    // Interval hole_alpha(1.19, 1.2);
    // Interval hole_theta(0.49, 0.5);
    // Interval hole_phi(0.59, 0.6);
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
    std::vector<Vector2d> hole = Vector2d::convex_hull(hole_all);
    std::ranges::sort(hole, [](const Vector2d &v0, const Vector2d &v1) {
        return v0.get_angle() < v1.get_angle();
    });
    std::vector<double> hole_angles;
    for(const Vector2d &hole_point: hole) {
        hole_angles.push_back(hole_point.get_angle());
    }
    plot_main.polygon(hole, Color::CYAN, 2);

    Queue2 boxes(4, 4);
    while(Plot::step(20)) {
        for(int t = 0; t < 20; t++) {
            if(boxes.empty()) {
                std::cout << std::format("Empty after {} iterations", t) << std::endl;
                break;
            }
            const Interval2 &box = boxes.fetch();
            bool any_outside = false;
            for(const Vector3d &v: polyhedron) {
                const auto &[x, y] = Vector3I(v.x, v.y, v.z).project(box.interval_0, box.interval_1);
                const bool intersects = (in(0.0, x) && in(0.0, y)) |
                                        Vector2(lower(x), lower(y)).is_inside_polygon(hole, hole_angles) |
                                        Vector2(lower(x), upper(y)).is_inside_polygon(hole, hole_angles) |
                                        Vector2(upper(x), lower(y)).is_inside_polygon(hole, hole_angles) |
                                        Vector2(upper(x), upper(y)).is_inside_polygon(hole, hole_angles);
                // const bool intersects = box.intersects_polygon(v, hole, hole_angles);
                if(!intersects) {
                    any_outside = true;
                    break;
                }
            }
            if(!any_outside) {
                boxes.push_parts(box);
            }
        }
        plot_boxes.clear();
        for(const Interval2 &box: boxes.get_vector()) {
            // plot_boxes.circle((box.center() - Vector2d(M_PI, M_PI / 2)) / Vector2d(M_PI, M_PI_2), 0.1, Color::gradient2(box.center()) / 2);
            plot_boxes.filled_interval2({
                                            map(box.interval_0, {0, 2 * M_PI}, {-1, 1}),
                                            map(box.interval_1, {0, M_PI}, {-1, 1})
                                        }, Color::gradient2(map(box.center().x, {0, 2 * M_PI}, {0, 1}), map(box.center().y, {0, M_PI}, {0, 1})) / 2);
            plot_boxes.interval2({
                                     map(box.interval_0, {0, 2 * M_PI}, {-1, 1}),
                                     map(box.interval_1, {0, M_PI}, {-1, 1})
                                 }, Color::WHITE);
            // for(const Vector3d &vertex: polyhedron) {
            //     plot_main.filled_polygon(box.boundary(vertex, 10, 10), Color::gradient2(box.center()) / 2);
            //     plot_main.polygon(box.boundary(vertex, 10, 10), Color::gradient2(box.center()), 2);
            //     // plot_main.point(vertex.project(median(box.interval_0), median(box.interval_1)), Color::gradient2(box.center()), 2);
            // }
        }
        plot_boxes.point(plot_boxes.get_mouse(), Color::MAGENTA / 2, 10);
        plot_boxes.show();

        if(boxes.empty()) {
            break;
        }
    }
    std::cout << "Boxes: " << boxes.get_vector().size() << std::endl;
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
