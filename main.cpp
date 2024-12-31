#include "src/base.hpp"
#include "src/plot.hpp"
#include "src/polyhedron.hpp"
#include "src/boxes.hpp"
#include "src/test.hpp"

#include <iostream>

Plot plot_main(1000, 1000, 1);
std::vector<Vector3> polyhedron = Polyhedron::rhombicosidodecahedron();

Plot plot_boxes(1000, 1000, 1);
Boxes boxes(10, 10);

void test() {
    test_combined_amplitude_phase();
    test_angle();
    test_intersections();

    std::cout << "All tests passed" << std::endl;
}

void setup() {
    plot_main.line(Vector2(-10, 0), Vector2(10, 0), Color::GRAY);
    plot_main.line(Vector2(0, -10), Vector2(0, 10), Color::GRAY);
    plot_main.circle(Vector2(0, 0), 1, Color::GRAY);

    std::cout << "Vertices: " << polyhedron.size() << std::endl;
}

void test_constant_phi() {
    constexpr Vector2 a(0.7, -0.7), b(0.4, 0.3);
    RandomNumberGenerator rng;
    for(int t = 0; t < 3000; t++) {
        Vector3 v(rng.uniform(-1, 1), rng.uniform(-1, 1), rng.uniform(-1, 1));
        v.normalize();
        const double theta_range = rng.uniform(0.1, 0.4);
        const double theta_min = rng.uniform(0, 2 * M_PI - theta_range);
        const double phi = rng.uniform(0, M_PI);
        Interval theta_interval(theta_min, theta_min + theta_range);

        if(Box::intersects_line_fixed_phi(v, a, b, theta_interval, phi)) {
            std::vector<Vector2> points;
            for(double theta = theta_min; theta <= theta_min + theta_range; theta += 0.001) {
                points.push_back(v.project(theta, phi));
            }
            plot_main.polyline(points, Color::GREEN, 2);
            plot_boxes.filled_box(Box(theta_interval, Interval(phi, phi)).normalized(), Color::GREEN);
        } else {
            std::vector<Vector2> points;
            for(double theta = theta_min; theta <= theta_min + theta_range; theta += 0.001) {
                points.push_back(v.project(theta, phi));
            }
            plot_main.polyline(points, Color::RED / 2);
            plot_boxes.filled_box(Box(theta_interval, Interval(phi, phi)).normalized(), Color::RED / 2);
        }
        plot_main.line(a, b, Color::RED);
    }
}

void test_constant_theta() {
    constexpr Vector2 a(0.7, -0.7), b(-0.4, -0.3);
    RandomNumberGenerator rng;
    for(int t = 0; t < 3000; t++) {
        Vector3 v(rng.uniform(-1, 1), rng.uniform(-1, 1), rng.uniform(-1, 1));
        v.normalize();
        const double phi_range = rng.uniform(0.1, 0.5);
        const double phi_min = rng.uniform(0, M_PI - phi_range);
        const double theta = rng.uniform(0, 2 * M_PI);
        Interval phi_interval(phi_min, phi_min + phi_range);

        if(Box::intersects_line_fixed_theta(v, a, b, theta, phi_interval)) {
            std::vector<Vector2> points;
            for(double phi = phi_min; phi <= phi_min + phi_range; phi += 0.001) {
                points.push_back(v.project(theta, phi));
            }
            plot_main.polyline(points, Color::GREEN, 2);
            plot_boxes.filled_box(Box(Interval(theta, theta), phi_interval).normalized(), Color::GREEN);
        } else {
            std::vector<Vector2> points;
            for(double phi = phi_min; phi <= phi_min + phi_range; phi += 0.001) {
                points.push_back(v.project(theta, phi));
            }
            plot_main.polyline(points, Color::RED / 2);
            plot_boxes.filled_box(Box(Interval(theta, theta), phi_interval).normalized(), Color::RED / 2);
        }
        plot_main.line(a, b, Color::RED);
    }
}

void draw() {
    constexpr Box hole_box(
        Interval(0, 0.01),
        Interval(0, 0.01)
    );
    constexpr Interval hole_alpha(0, 0.01);

    std::vector<Vector2> hole_all;
    for(double alpha = hole_alpha.min; alpha <= hole_alpha.max; alpha += 0.01) {
        for(double theta = hole_box.theta_interval.min; theta <= hole_box.theta_interval.max; theta += 0.01) {
            for(double phi = hole_box.phi_interval.min; phi <= hole_box.phi_interval.max; phi += 0.01) {
                for(const Vector3 &vertex: polyhedron) {
                    hole_all.push_back(vertex.project(theta, phi).rotate(alpha));
                }
            }
        }
    }
    std::vector<Vector2> hole = Vector2::sort(Vector2::convex_hull(hole_all));
    plot_main.polygon(hole, Color::CYAN, 2);
    std::vector<double> hole_angles;
    for(const Vector2 &hole_point: hole) {
        hole_angles.push_back(hole_point.get_angle());
    }

    const auto start = std::chrono::high_resolution_clock::now();
    for(int t = 0; t < 500; t++) {
        if(boxes.empty()) {
            break;
        }
        const Box box = boxes.pop();
        bool is_any_boundary_outside = false;
        for(const Vector3 &vertex: polyhedron) {
            if(!box.intersects_polygon(vertex, hole)) {
                is_any_boundary_outside = true;
                break;
            }
        }
        if(!is_any_boundary_outside) {
            boxes.push_quadrants(box);
        }
    }
    const auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
    std::cout << "Boxes: " << boxes.get_boxes().size() << std::endl;
    for(const Box &box: boxes.get_boxes()) {
        plot_boxes.circle(box.center(), 0.1, box.color() / 2);
        plot_boxes.filled_box(box.normalized(), box.color());
        for(const Vector3 &vertex: polyhedron) {
            plot_main.polygon(box.boundary(vertex, 0.1, 0.1), box.color());
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
