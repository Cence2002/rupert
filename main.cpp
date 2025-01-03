#include "src/base.hpp"
#include "src/plot.hpp"
#include "src/polyhedron.hpp"
#include "src/boxes.hpp"
#include "src/test.hpp"

#include <iostream>

Plot plot_main(1000, 1000, 1);
std::vector<Vector3d> polyhedron = Polyhedron::rhombicosidodecahedron();

Plot plot_boxes(1000, 1000, 1);

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

    Intervals boxes(4, 4);
    const auto start = std::chrono::high_resolution_clock::now();
    for(int t = 0; t < 10000; t++) {
        if(boxes.empty()) {
            std::cout << std::format("Empty after {} iterations", t) << std::endl;
            break;
        }
        const Interval2 &box = boxes.front();
        bool any_outside = false;
        for(const Vector3d &v: polyhedron) {
            const auto &[x, y] = Vector3I(v.x, v.y, v.z).project(box.theta, box.phi);
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
            boxes.push_quad(box);
        }
        boxes.pop();
    }
    const auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
    std::cout << "Boxes: " << boxes.get_vector().size() << std::endl;
    for(const Interval2 &box: boxes.get_vector()) {
        plot_boxes.circle(box.center(), 0.1, box.color() / 2);
        plot_boxes.filled_box(box.normalized(), box.color());
        for(const Vector3d &vertex: polyhedron) {
            plot_main.polygon(box.boundary(vertex, 10, 10), box.color(), 2);
            plot_main.point(vertex.project(median(box.theta), median(box.phi)), box.color());
        }
    }
}

void exit() {
    plot_main.save("../outputs/main.png");
    plot_boxes.save("../outputs/boxes.png");
}

int main() {
    test();
    setup();
    draw();
    exit();
    return 0;
}
