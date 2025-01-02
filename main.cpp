#include "src/base.hpp"
#include "src/plot.hpp"
#include "src/polyhedron.hpp"
#include "src/boxes.hpp"
#include "src/test.hpp"

#include <iostream>

Plot plot_main(1000, 1000, 1);
std::vector<Vector3> polyhedron = Polyhedron::rhombicosidodecahedron();

Plot plot_boxes(1000, 1000, 1);
Boxes boxes(4, 4);

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

void test_interval_hole() {
    namespace il = boost::numeric::interval_lib;
    using I = boost::numeric::interval<
        double,
        boost::numeric::interval_lib::policies<
            boost::numeric::interval_lib::save_state<
                boost::numeric::interval_lib::rounded_transc_std<double>
            >,
            boost::numeric::interval_lib::checking_base<double>
        >
    >;
    Vector3 v(0.4, 5, -1);
    v.normalize();
    constexpr Box hole_box(
        Interval(0.1, 0.5),
        Interval(1.3, 1.4)
    );
    constexpr Interval hole_alpha(0.2, 0.25);
    std::vector<Vector2> hole_all;
    for(double alpha = hole_alpha.min; alpha <= hole_alpha.max; alpha += hole_alpha.length() / 100) {
        for(double theta = hole_box.theta_interval.min; theta <= hole_box.theta_interval.max; theta += hole_box.theta_interval.length() / 100) {
            for(double phi = hole_box.phi_interval.min; phi <= hole_box.phi_interval.max; phi += hole_box.phi_interval.length() / 100) {
                hole_all.push_back(v.project(theta, phi).rotate(alpha));
            }
        }
    }
    for(int alpha_i = 0; alpha_i < 100; alpha_i++) {
        const I alpha(hole_alpha.min + alpha_i * hole_alpha.length() / 100, hole_alpha.min + (alpha_i + 1) * hole_alpha.length() / 100);
        for(int theta_i = 0; theta_i < 100; theta_i++) {
            const I theta(hole_box.theta_interval.min + theta_i * hole_box.theta_interval.length() / 100, hole_box.theta_interval.min + (theta_i + 1) * hole_box.theta_interval.length() / 100);
            for(int phi_i = 0; phi_i < 100; phi_i++) {
                const I phi(hole_box.phi_interval.min + phi_i * hole_box.phi_interval.length() / 100, hole_box.phi_interval.min + (phi_i + 1) * hole_box.phi_interval.length() / 100);

                const I sin_theta = sin(theta);
                const I cos_theta = cos(theta);
                const I projection_x = -v.x * sin_theta + v.y * cos_theta;
                const I projection_y = (v.x * cos_theta + v.y * sin_theta) * cos(phi) - v.z * sin(phi);
                const I sin_alpha = sin(alpha);
                const I cos_alpha = cos(alpha);
                const I x = projection_x * cos_alpha - projection_y * sin_alpha;
                const I y = projection_x * sin_alpha + projection_y * cos_alpha;
                const Box box{
                            Interval(x.lower(), x.upper()),
                            Interval(y.lower(), y.upper())
                        };
                plot_main.box(box, Color::RED);
            }
        }
    }
    plot_main.points(hole_all, Color::CYAN);
}

void test_3_2() {
    for(const I &hole_alpha: divide(I(1.16, 1.2), 1)) {
        for(const I &hole_theta: divide(I(0.46, 0.5), 1)) {
            for(const I &hole_phi: divide(I(0.56, 0.6), 1)) {
                std::vector<Vector2> hole_all;
                for(const I &alpha: divide(hole_alpha, 10)) {
                    const I sin_alpha = boost::numeric::sin(alpha);
                    const I cos_alpha = boost::numeric::cos(alpha);
                    for(const I &theta: divide(hole_theta, 10)) {
                        const I sin_theta = boost::numeric::sin(theta);
                        const I cos_theta = boost::numeric::cos(theta);
                        for(const I &phi: divide(hole_phi, 10)) {
                            const I sin_phi = boost::numeric::sin(phi);
                            const I cos_phi = boost::numeric::cos(phi);
                            for(const Vector3 &vertex: polyhedron) {
                                const I x = -vertex.x * sin_theta + vertex.y * cos_theta;
                                const I y = (vertex.x * cos_theta + vertex.y * sin_theta) * cos_phi - vertex.z * sin_phi;
                                const I x_rotated = x * cos_alpha - y * sin_alpha;
                                const I y_rotated = x * sin_alpha + y * cos_alpha;
                                hole_all.push_back(Vector2(x_rotated.lower(), y_rotated.lower()));
                                hole_all.push_back(Vector2(x_rotated.lower(), y_rotated.upper()));
                                hole_all.push_back(Vector2(x_rotated.upper(), y_rotated.lower()));
                                hole_all.push_back(Vector2(x_rotated.upper(), y_rotated.upper()));
                            }
                        }
                    }
                }
                std::vector<Vector2> hole = Vector2::convex_hull(hole_all);
                std::ranges::sort(hole, [](const Vector2 &v0, const Vector2 &v1) {
                    return v0.get_angle() < v1.get_angle();
                });
                std::vector<double> hole_angles;
                for(const Vector2 &hole_point: hole) {
                    hole_angles.push_back(hole_point.get_angle());
                }
                plot_main.polygon(hole, Color::CYAN, 2);

                std::queue<std::tuple<I, I>> boxes({
                        {I(0, 2 * M_PI), I(0, M_PI)}
                    });
                for(int t = 0; t < 10000; t++) {
                    if(boxes.empty()) {
                        std::cout << std::format("t={}", t) << std::endl;
                        break;
                    }
                    const auto [theta, phi] = boxes.front();
                    boxes.pop();
                    // const I sin_theta = boost::numeric::sin(theta);
                    // const I cos_theta = boost::numeric::cos(theta);
                    // const I sin_phi = boost::numeric::sin(phi);
                    // const I cos_phi = boost::numeric::cos(phi);
                    Box box{
                                Interval(theta.lower(), theta.upper()),
                                Interval(phi.lower(), phi.upper())
                            };
                    bool any_outside = false;
                    for(const Vector3 &v: polyhedron) {
                        // const I x = -v.x * sin_theta + v.y * cos_theta;
                        // const I y = (v.x * cos_theta + v.y * sin_theta) * cos_phi - v.z * sin_phi;
                        // const bool intersects = (x.lower() < 0 && 0 < x.upper() && y.lower() < 0 && 0 < y.upper()) |
                        //                         Vector2(x.lower(), y.lower()).is_inside_polygon(hole, hole_angles) |
                        //                         Vector2(x.lower(), y.upper()).is_inside_polygon(hole, hole_angles) |
                        //                         Vector2(x.upper(), y.lower()).is_inside_polygon(hole, hole_angles) |
                        //                         Vector2(x.upper(), y.upper()).is_inside_polygon(hole, hole_angles);
                        bool intersects = box.intersects_polygon(v, hole, hole_angles);
                        if(!intersects) {
                            any_outside = true;
                            break;
                        }
                    }
                    if(!any_outside) {
                        const double theta_mid = (theta.lower() + theta.upper()) / 2;
                        const double phi_mid = (phi.lower() + phi.upper()) / 2;
                        boxes.push({{theta.lower(), theta_mid}, {phi.lower(), phi_mid}});
                        boxes.push({{theta.lower(), theta_mid}, {phi_mid, phi.upper()}});
                        boxes.push({{theta_mid, theta.upper()}, {phi.lower(), phi_mid}});
                        boxes.push({{theta_mid, theta.upper()}, {phi_mid, phi.upper()}});
                    }
                }
                // std::cout << "Boxes: " << boxes.size() << std::endl;
                // while(!boxes.empty()) {
                //     const auto [theta, phi] = boxes.front();
                //     boxes.pop();
                //     Box box{
                //                 Interval(theta.lower(), theta.upper()),
                //                 Interval(phi.lower(), phi.upper())
                //             };
                //     plot_boxes.filled_box(box.normalized(), box.color());
                //     plot_boxes.circle(box.center(), 0.1, box.color() / 2, 2);
                // }
                if(!boxes.empty()) {
                    std::cout << std::format("alpha=[{}, {}], theta=[{}, {}], phi=[{}, {}]\n", hole_alpha.lower(), hole_alpha.upper(), hole_theta.lower(), hole_theta.upper(), hole_phi.lower(), hole_phi.upper());
                } else {
                    std::cout << "empty" << std::endl;
                }
            }
        }
        break;
    }
}

void draw() {
    test_3_2();
    return;
    constexpr Box hole_box(
        Interval(1, 1),
        Interval(1, 1)
    );
    constexpr Interval hole_alpha(1.01, 1.02);

    std::vector<Vector2> hole_all;
    for(double alpha = hole_alpha.min; alpha <= hole_alpha.max; alpha += hole_alpha.length() / 100) {
        for(double theta = hole_box.theta_interval.min; theta <= hole_box.theta_interval.max; theta += 0.01) {
            for(double phi = hole_box.phi_interval.min; phi <= hole_box.phi_interval.max; phi += 0.01) {
                for(const Vector3 &vertex: polyhedron) {
                    hole_all.push_back(vertex.project(theta, phi).rotate(alpha));
                }
            }
        }
    }
    std::vector<Vector2> hole = Vector2::convex_hull(hole_all);
    std::ranges::sort(hole, [](const Vector2 &v0, const Vector2 &v1) {
        return v0.get_angle() < v1.get_angle();
    });
    std::vector<double> hole_angles;
    for(const Vector2 &hole_point: hole) {
        hole_angles.push_back(hole_point.get_angle());
    }
    plot_main.polygon(hole, Color::CYAN, 2);

    const auto start = std::chrono::high_resolution_clock::now();
    for(int t = 0; t < 1000; t++) {
        if(boxes.empty()) {
            break;
        }
        const Box box = boxes.pop();
        bool is_any_boundary_outside = false;
        for(const Vector3 &vertex: polyhedron) {
            if(!box.intersects_polygon(vertex, hole, hole_angles)) {
                is_any_boundary_outside = true;
                // if(box.theta_interval.min == 0 && box.phi_interval.min == 0) {
                //     plot_main.filled_polygon(box.boundary(vertex, 0.1, 0.1), Color::RED / 4);
                //     plot_main.polygon(box.boundary(vertex, 0.1, 0.1), Color::RED, 1);
                // }
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
            // plot_main.polygon(box.boundary(vertex, 0.1, 0.1), box.color());
            // plot_main.point(vertex.project(box.theta_interval.min, box.phi_interval.min), box.color());
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
