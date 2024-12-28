#include <iostream>
#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
#include "plot.hpp"
#include "math.hpp"
#include "test.hpp"


Plot plot(4000, 4000, 2);

void test() {
    test_combined_amplitude_phase();
}

void setup() {
    plot.line(Eigen::Vector2f(-10, 0), Eigen::Vector2f(10, 0), Color::GRAY);
    plot.line(Eigen::Vector2f(0, -10), Eigen::Vector2f(0, 10), Color::GRAY);
    // plot.circle(Eigen::Vector2f(0, 0), 1, Color::GRAY);
}

void draw() {
    // float hole_theta = 0.6;
    // float hole_phi = 0.8;
    // Eigen::Matrix<float, 2, 3> hole_axes = angles_to_axes(Eigen::Vector2f(hole_theta, hole_phi));
    // std::vector<Eigen::Vector2f> hole_vertices;
    // for(Eigen::Vector3f &vertex: polyhedron_vertices) {
    //     hole_vertices.push_back(hole_axes * vertex);
    // }
    // std::vector<Eigen::Vector2f> hole_hull = convex_hull(hole_vertices);
    // plot.polygon(hole_hull, Color::RED);

    // float theta_min = 0.3;
    // float theta_max = 0.5;
    // float phi_min = 0.5;
    // float phi_max = 0.6;
    // float theta_min = 0.3;
    // float theta_max = 1.3;
    // float phi_min = 0.5;
    // float phi_max = 0.8;
    // for(float theta = theta_min; theta < theta_max; theta += 0.001) {
    //     for(float phi = phi_min; phi < phi_max; phi += 0.001) {
    //         Eigen::Matrix<float, 2, 3> axes = angles_to_axes(Eigen::Vector2f(theta, phi));
    //         std::vector<Eigen::Vector2f> vertices;
    //         for(Eigen::Vector3f &vertex: polyhedron_vertices) {
    //             vertices.push_back(axes * vertex);
    //         }
    //         for(int i = 0; i < vertices.size(); i++) {
    //             plot.point(vertices[i], Color::GREEN, 1);
    //         }
    //     }
    // }
    //generate a random theta min and phi min, ranges, and a vertekx to be rotated
    // for(int t = 0; t < 150; t++) {
    //     float theta_min = (rand() % 1000) / 1000.0 * (M_PI - 0.5) * 2;
    //     float theta_range = (rand() % 1000) / 1000.0 * 0.5;
    //     float phi_min = (rand() % 1000) / 1000.0 * M_PI;
    //     float phi_range = (rand() % 1000) / 1000.0 * 0.1;
    //     Eigen::Vector3f vertex;
    //     vertex <<
    //             (rand() % 1000 - 500) / 500.0,
    //             (rand() % 1000 - 500) / 500.0,
    //             (rand() % 1000 - 500) / 500.0;
    //     vertex /= vertex.norm();
    //     cv::Scalar color(rand() % 256, rand() % 256, rand() % 256);
    //     for(float theta = theta_min; theta < theta_min + theta_range; theta += 0.0002) {
    //         for(float phi = phi_min; phi < phi_min + phi_range; phi += 0.0002) {
    //             Eigen::Matrix<float, 2, 3> axes = angles_to_axes(Eigen::Vector2f(theta, phi));
    //             Eigen::Vector2f projected_vertex = axes * vertex;
    //             plot.point(projected_vertex, color);
    //         }
    //     }
    // }

    float theta_min = 0.3;
    float theta_max = 1.4;
    float phi_min = 0.5;
    float phi_max = 0.9;
    Eigen::Vector3f vertex;
    vertex << 1, -1, 1;
    vertex /= vertex.norm();
    for(float theta = theta_min; theta < theta_max; theta += 0.001) {
        for(float phi = phi_min; phi < phi_max; phi += 0.001) {
            Eigen::Matrix<float, 2, 3> axes = angles_to_axes(Eigen::Vector2f(theta, phi));
            Eigen::Vector2f projected_vertex = axes * vertex;
            plot.point(projected_vertex, Color::BLUE);
        }
    }

    std::vector<Eigen::Vector2f> boundary_points = boundary(vertex, {theta_min, theta_max}, {phi_min, phi_max}, 0.01, 0.01);
    plot.polygon(boundary_points, Color::RED, 2);
    plot.points(boundary_points, Color::GREEN, 2);


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
    for(Eigen::Vector3f &vertex: polyhedron) {
        vertex /= std::sqrt(3.0f);
    }
}

void exit() {
    plot.save("../outputs/image.png");
}

int main() {
    test();
    setup();
    draw();
    exit();
    return 0;
}
