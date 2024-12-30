#include "polyhedron.hpp"
#include "plot.hpp"
#include "boxes.hpp"
#include "test.hpp"
#include <iostream>

Plot plot_main(1000, 1000, 1);
std::vector<Vector3f> polyhedron = Polyhedron::rhombicosidodecahedron();

Plot plot_boxes(1000, 1000, 1);
Boxes boxes(1, 1);

cv::Scalar tr = Color::GREEN;
cv::Scalar tl = Color::RED;
cv::Scalar br = Color::YELLOW;
cv::Scalar bl = Color::BLUE;

void test() {
    test_combined_amplitude_phase();
    test_angle();

    std::cout << "All tests passed" << std::endl;
}

void setup() {
    plot_main.line(Vector2f(-10, 0), Vector2f(10, 0), Color::GRAY);
    plot_main.line(Vector2f(0, -10), Vector2f(0, 10), Color::GRAY);
    plot_main.circle(Vector2f(0, 0), 1, Color::GRAY);

    std::cout << "Vertices: " << polyhedron.size() << std::endl;
}

void draw() {
    Box hole_box(
        Interval(0, 0.01),
        Interval(0, 0.01)
    );
    Interval hole_alpha(0, 0.01);

    std::vector<Vector2f> hole_all;
    for(float theta = hole_box.theta_interval.min; theta <= hole_box.theta_interval.max; theta += 0.01) {
        for(float phi = hole_box.phi_interval.min; phi <= hole_box.phi_interval.max; phi += 0.01) {
            for(float alpha = hole_alpha.min; alpha <= hole_alpha.max; alpha += 0.01) {
                for(const Vector3f &vertex: polyhedron) {
                    hole_all.push_back(vertex.project(theta, phi).rotate(alpha));
                }
            }
        }
    }
    std::vector<Vector2f> hole = Vector2f::sort(Vector2f::hull(hole_all));
    plot_main.polygon(hole, Color::CYAN, 2);
    std::vector<float> hole_angles;
    for(const Vector2f &hole_point: hole) {
        hole_angles.push_back(hole_point.get_angle());
    }

    const auto start = std::chrono::high_resolution_clock::now();
    for(int t = 0; t < 1000; t++) {
        if(boxes.empty()) {
            break;
        }
        const Box box = boxes.pop();
        bool is_any_boundary_outside = false;
        for(const Vector3f &vertex: polyhedron) {
            bool is_any_vertex_inside = false;
            for(const Vector2f &boundary_point: Boxes::boundary(vertex, box, 0.01, 0.01)) {
                const int index = boundary_point.get_index(hole_angles);
                if(boundary_point.is_inside(hole[index], hole[(index + 1) % hole.size()])) {
                    is_any_vertex_inside = true;
                    break;
                }
            }
            if(!is_any_vertex_inside) {
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
    // for(const Box &box: boxes.get_boxes()) {
    //     for(const Vector3f &vertex: polyhedron) {
    //         // plot_main.points(Boxes::boundary(vertex, box, 0.01, 0.01), Color::GRAY);
    //     }
    // }
    for(const Box &box: boxes.get_boxes()) {
        plot_boxes.circle(box.center(), 0.1, box.color() / 4);
        plot_boxes.filled_box(box.normalized(), box.color());
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
