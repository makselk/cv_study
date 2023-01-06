#ifndef FILTERS_HPP
#define FILTERS_HPP

#include "opencv4/opencv2/opencv.hpp"


namespace FILTERS {
    void customBoxFilter(cv::Mat &src, cv::Mat &dst, int kernel_size);
    void customUnsharpMask(cv::Mat &src, cv::Mat &dst, int kernel_size, int smoothing);
    void calculateSimilarity(cv::Mat &src1, cv::Mat &src2);
    void boxFilterPart1_3(const std::string &path, int kernel_size);
    void gaussAndBoxComapre4(const std::string &path, int kernel_size);
    
    void speedCompare(void (*function1) (cv::Mat &src, cv::Mat &dst, int kernel_size, int smoothing),
                      void (*function2) (cv::Mat &src, cv::Mat &dst, int kernel_size, int smoothing));
    
};


#endif //FILTERS_HPP
