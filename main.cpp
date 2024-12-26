#include <iostream>
#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
#include "plot.hpp"
#include "math.hpp"


Plot plot(600, 400, 1.5f);

void setup() {
    plot.line(Eigen::Vector2f(-10, 0), Eigen::Vector2f(10, 0), Color::GRAY);
    plot.line(Eigen::Vector2f(0, -10), Eigen::Vector2f(0, 10), Color::GRAY);
    plot.circle(Eigen::Vector2f(0, 0), 1, Color::GRAY);
}

void draw() {
    for(float theta = 0.3; theta < 0.6; theta += 0.002) {
        for(float phi = 0.3; phi < 0.4; phi += 0.002) {
            Eigen::Vector2f angles(theta, phi);
            Eigen::Matrix<float, 2, 3> axes = angles_to_axes(angles);
            Eigen::Matrix<float, 3, 8> vertices;
            vertices <<
                    +1, +1, +1, +1, -1, -1, -1, -1,
                    +1, +1, -1, -1, +1, +1, -1, -1,
                    +1, -1, +1, -1, +1, -1, +1, -1;
            vertices /= sqrt(3.0f);

            Eigen::Matrix<float, 2, 8> projected_vertices = axes * vertices;

            for(int i = 0; i < projected_vertices.cols(); i++) {
                plot.point(projected_vertices.col(i), Color::GREEN, 1);
            }
        }
    }
}

void exit() {
    plot.save("../outputs/image.png");
}

int main() {
    setup();
    draw();
    exit();
    return 0;
}
