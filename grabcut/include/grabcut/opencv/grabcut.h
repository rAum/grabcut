#pragma once
#ifndef GRABCUT_OPENCV_GRABCUT_H
#define GRABCUT_OPENCV_GRABCUT_H

#include <opencv2/core.hpp>
#include <grabcut/grabcut.h>

namespace grabcut {

/***
 * This is an adapter for OpenCV API compatibility, to simplify basic call.
 * For better control and performance, just use API directly.
 *
 * Example:
 * >>> cv::Rect selection{ 10, 10, 50, 50 }; // ROI rectangle
 * >>> cv::Mat mask = cv::Mat::zeros(image.rows, image.cols, CV_8UC1);
 * >>> cv::rectangle(mask, selection, cv::Scalar(1), -1);
 * >>> grabcut::run_grabcut(image, mask); // segment image given mask
 * >>> image.copyTo(destination, mask); // copy only pixels from foreground
 *
 * @param image BGR or RGB 8-bit image
 * @param mask input-output
 * @param iterations maximum number of iterations to be performed.
 */
void run_grabcut(cv::InputArray image, cv::InputOutputArray mask, int iterations = 3) {
    CV_Assert(image.isContinuous());
    CV_Assert(image.type() == CV_8UC3);
    CV_Assert(mask.type() == CV_8UC1);
    CV_Assert(mask.isContinuous());

    auto m = mask.getMat();
    auto src = image.getMat();
    CV_Assert(src.size == m.size);

    Grabcut grabcut;
    grabcut.init(src.data, m.data, src.cols, src.rows);
    grabcut.run(iterations);
    auto res = grabcut.get_mask();
    cv::Mat cv_res{src.rows, src.cols, CV_8UC1, res.data() };
    cv_res.copyTo(mask);
}

/***
 * This is an adapter for OpenCV API compatibility, to simplify basic call.
 * For better control and performance, just use API directly.
 * This version instead of mask, accepts rectangle selection
 *
 * Example:
 * >>> cv::Mat mask;
 * >>> grabcut::run_grabcut(image, cv::Rect{10, 10, 20, 20}, mask); // segment image given ROI
 * >>> image.copyTo(destination, mask); // copy only pixels from foreground
 *
 * @param image BGR or RGB 8-bit image
 * @param mask output
 * @param iterations maximum number of iterations to be performed.
 */
void run_grabcut(cv::InputArray image, cv::Rect selection, cv::OutputArray mask, int iterations = 3) {
    CV_Assert(image.isContinuous());
    CV_Assert(image.type() == CV_8UC3);
    auto src = image.getMat();

    cv::Mat src_mask = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
    cv::rectangle(src_mask, selection, cv::Scalar(1), -1);

    Grabcut grabcut;
    grabcut.init(src.data, src_mask.data, src.cols, src.rows);
    grabcut.run(iterations);
    auto res = grabcut.get_mask();
    cv::Mat cv_res{src.rows, src.cols, CV_8UC1, res.data() };
    cv_res.copyTo(mask);
}

} // namespace grabcut

#endif // GRABCUT_OPENCV_GRABCUT_H
