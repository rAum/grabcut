/// This is a simple GUI for interactive grabcut tests
#include <iostream>
#include <string>
#include <filesystem>


#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <grabcut/opencv/grabcut.h>

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
    // for quick eval
    if (std::string("../../tests/data/flower1.jpg") == argv[1]) {
        selection = { 68, 23, 539, 510};
        cv::imshow(window_name, input_image);
    } else {
        do {
            selection = cv::selectROI(window_name, input_image);
        } while (selection.area() < 10);
        std::cout << "cv::Rect selection = { "
                  << selection.x << ", " << selection.y << ", "
                  << selection.width << ", " << selection.height << "};\n";
        auto result_image = cv::Mat::zeros(input_image.size(), CV_8UC4);
    }
    cv::TickMeter timer;

    constexpr int steps = 5;

    timer.reset();
    timer.start();
    cv::Mat src = input_image;
    cv::Mat mask = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
    cv::Mat bgModel = cv::Mat::zeros(1, 65, CV_64FC1);
    cv::Mat fgModel = cv::Mat::zeros(1, 65, CV_64FC1);
    cv::grabCut(src, mask, selection, bgModel, fgModel, steps, cv::GC_INIT_WITH_RECT);
    timer.stop();
    std::cout << "OpenCV time: " << timer.getTimeSec() << std::endl;

    cv::Mat mask2 = (mask == 1) + (mask == 3);  // 0 = cv::GC_BGD, 1 = cv::GC_FGD, 2 = cv::PR_BGD, 3 = cv::GC_PR_FGD
    cv::Mat dest;
    src.copyTo(dest, mask2);
    cv::imshow("baseline_opencv", dest);

    timer.reset();
    timer.start();
    cv::Mat src_mask;
    grabcut::run_grabcut(src, selection, src_mask, steps);
    timer.stop();
    std::cout << "Time: " << timer.getTimeSec() << std::endl;

    cv::Mat dest2;
    src.copyTo(dest2, src_mask);
    cv::imshow("this_implementation", dest2);

    cv::waitKey();
    return 0;
}