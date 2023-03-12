#pragma once

#include <opencv2/opencv.hpp>


namespace CUSTOM_FOURIER {
    cv::Mat dft(cv::Mat& input, bool reverse = false);
}
