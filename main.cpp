#include <iostream>
#include <Eigen/Dense>
#include <opencv2/opencv.hpp>

int main() {
    std::cout << "Hello, World!" << std::endl;

    Eigen::Matrix2f matrix;
    matrix << 1, 2, 3, 4;
    std::cout << matrix << std::endl;

    cv::Mat image = cv::Mat::zeros(400, 400, CV_8UC3);
    cv::Point center = cv::Point(200, 200);
    cv::Scalar color = cv::Scalar(0, 255, 0);
    cv::circle(image, center, 100, color, 2);
    cv::imwrite("image.png", image);

    return 0;
}
