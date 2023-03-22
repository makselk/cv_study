#pragma once

#include <opencv2/opencv.hpp>


namespace CUSTOM_FOURIER {
    cv::Mat dft(cv::Mat& input, bool reverse = false);
    cv::Mat fft(cv::Mat& input);
    cv::Mat fft2(cv::Mat& input);
}
