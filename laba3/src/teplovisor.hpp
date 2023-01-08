#ifndef TEPLOVISOR_HPP
#define TEPLOVISOR_HPP

#include "opencv4/opencv2/opencv.hpp"


namespace TEPLOVISOR {
    void grayPipelineVideo(const std::string &path);
    void grayPipelineImage(const std::string &path);
    void bgrPipeline(const std::string &path);
}

namespace {
    void grayOneTarget(cv::Mat &src, cv::Mat &dst);
    void grayThreeTargets(cv::Mat &src, cv::Mat &dst);
    void bgrOneTarget(cv::Mat &src, cv::Mat &dst);
}

#endif //TEPLOVISOR_HPP
