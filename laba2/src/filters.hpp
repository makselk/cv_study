#ifndef FILTERS_HPP
#define FILTERS_HPP

#include "opencv4/opencv2/opencv.hpp"


namespace FILTERS {
    void customBoxFilter(cv::Mat &src, cv::Mat &dst, int kernel_size);
    void customLaplace(cv::Mat &src, cv::Mat &dst);
    void calculateSimilarity(cv::Mat &src1, cv::Mat &src2);
    void unsharpMaskBox(cv::Mat &src, cv::Mat &dst, int kernel_size, int sharp);
    void unsharpMaskGauss(cv::Mat &src, cv::Mat &dst, int kernel_size, int sharp);
    void unsharpMaskLaplace(cv::Mat &src, cv::Mat &dst, int sharp);

    void boxFilterPart1_3(const std::string &path, int kernel_size);
    void gaussAndBoxComapre4(const std::string &path, int kernel_size);
    void unsharpMask5(const std::string &path, int kernel_size, int sharp);
    void laplace6(const std::string &path);
    void unsharpLaplace7(const std::string &path, int kernel_size, int sharp);

    void launchAll(const std::string &path, int kernel_size, int sharp);
};


#endif //FILTERS_HPP
