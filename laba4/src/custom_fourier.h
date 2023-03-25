#pragma once

#include <opencv2/opencv.hpp>


namespace CUSTOM_FOURIER {
    // Лобовой подход к реализации преобразования фурье
    cv::Mat dft(cv::Mat& input, bool reverse = false);
    // Более оптимальный подход, использующий алгоритм radix-2
    cv::Mat fft(cv::Mat& input);
    cv::Mat fft2(cv::Mat& input);
}
