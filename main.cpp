#include "plot.hpp"
#include "math.hpp"
#include "test.hpp"
#include "cover.hpp"
#include <iostream>


Plot plot_main(1000, 1000, 1);
std::vector<Eigen::Vector3f> polyhedron = {
            {+1, +1, +1},
            {+1, +1, -1},
            {+1, -1, +1},
            {+1, -1, -1},
            {-1, +1, +1},
            {-1, +1, -1},
            {-1, -1, +1},
            {-1, -1, -1}
        };

Plot plot_cover(1000, 1000, 1);
Cover cover(10, 10);

cv::Scalar tr = Color::GREEN;
cv::Scalar tl = Color::RED;
cv::Scalar br = Color::YELLOW;
cv::Scalar bl = Color::BLUE;

void test() {
    test_combined_amplitude_phase();
    test_angle();
}

void setup() {
    plot_main.line(Eigen::Vector2f(-10, 0), Eigen::Vector2f(10, 0), Color::GRAY);
    plot_main.line(Eigen::Vector2f(0, -10), Eigen::Vector2f(0, 10), Color::GRAY);
    plot_main.circle(Eigen::Vector2f(0, 0), 1, Color::GRAY);

    for(Eigen::Vector3f &vertex: polyhedron) {
        vertex /= vertex.norm();
    }
}

void draw() {
    Box hole_box(
        Interval(0.4, 0.5),
        Interval(0.6, 0.7)
    );
    Interval hole_alpha(0.1, 0.2);
    std::vector<Eigen::Vector2f> hole_all;
    for(const Eigen::Vector3f &vertex: polyhedron) {
        for(float theta = hole_box.theta_interval.min; theta < hole_box.theta_interval.max; theta += 0.01) {
            for(float phi = hole_box.phi_interval.min; phi < hole_box.phi_interval.max; phi += 0.01) {
                for(float alpha = hole_alpha.min; alpha < hole_alpha.max; alpha += 0.01) {
                    hole_all.push_back(rotate_point(project_point(vertex, theta, phi), alpha));
                }
            }
        }
    }
    std::vector<Eigen::Vector2f> hole = sort_by_angle(convex_hull(hole_all));
    plot_main.polygon(hole, Color::CYAN, 2);
    std::vector<float> hole_angles;
    for(const Eigen::Vector2f &hole_point: hole) {
        hole_angles.push_back(get_angle(hole_point));
    }

    for(int t = 0; t < 100000; t++) {
        Box box = cover.pop();
        std::vector<std::vector<Eigen::Vector2f>> boundaries;
        for(const Eigen::Vector3f &vertex: polyhedron) {
            boundaries.push_back(boundary(vertex, box, 0.01, 0.01));
        }
        bool is_any_outside = false;
        for(const std::vector<Eigen::Vector2f> &boundary_points: boundaries) {
            bool is_all_inside = false;
            for(const Eigen::Vector2f &boundary_point: boundary_points) {
                float angle = get_angle(boundary_point);
                int index = get_index(hole_angles, angle);
                if(is_inside(boundary_point, hole[index], hole[(index + 1) % hole.size()])) {
                    is_all_inside = true;
                    break;
                }
            }
            if(!is_all_inside) {
                is_any_outside = true;
                break;
            }
        }
        if(!is_any_outside) {
            cover.push_quadrants(box);
        }
    }
    cover.plot_boxes(plot_cover, 0);
    for(const Box &box: cover.get_boxes()) {
        for(const Eigen::Vector3f &vertex: polyhedron) {
            std::vector<Eigen::Vector2f> boundary_points = boundary(vertex, box, 0.01, 0.01);
            plot_main.polygon(boundary_points, Color::GREEN);
        }
    }
}

void exit() {
    plot_main.save("../outputs/main.png");
    plot_cover.save("../outputs/cover.png");
}

int main() {
    test();
    setup();
    draw();
    exit();
    return 0;
}
