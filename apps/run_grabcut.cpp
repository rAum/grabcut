/// This is a simple GUI for interactive grabcut tests
#include <iostream>
#include <string>
#include <filesystem>

#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>

int main(int argc, char** argv) {
    if (argc <= 1) {
        std::cerr << "Pass image file" << std::endl;
        return 1;
    }
    cv::Mat input_image;
    try {
        std::filesystem::path image_path(argv[1]);
        input_image = cv::imread(image_path.c_str());
        std::cout << "const std::string file = " << std::quoted(argv[1]) << "\n";
    } catch (...) {
        std::cerr << "Failed to load " << std::quoted(argv[1]) << std::endl;
        return 3;
    }

    std::string window_name = "Grabcut Interactive Demo";
    cv::namedWindow(window_name);

    cv::Rect selection;
    do {
        selection = cv::selectROI(window_name, input_image);
    } while (selection.area() < 10);
    std::cout << "cv::Rect selection = { "
        << selection.x << ", " << selection.y << ", "
        << selection.width << ", " << selection.height << "};\n";
    auto result_image = cv::Mat::zeros(input_image.size(), CV_8UC4);
    cv::waitKey();
    cv::destroyAllWindows();
    return 0;
}