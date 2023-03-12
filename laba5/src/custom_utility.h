#ifndef CUSTOM_UTILITIES_HPP
#define CUSTOM_UTILITIES_HPP

#include <opencv2/opencv.hpp>

namespace CUSTOM_UTILITY {
    // Автоматически определяет словарь, по наибольшему количеству
    // обнаруженных маркеров на изображении
    int defineDictionary(cv::Mat &img);
}

#endif //CUSTOM_UTILITIES_HPP