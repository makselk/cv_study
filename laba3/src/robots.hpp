#ifndef ROBOTS_HPP
#define ROBOTS_HPP

#include "opencv4/opencv2/opencv.hpp"


typedef enum {
    RED,
    GREEN,
    BLUE
} COLORS;

namespace {
    void findLampPos(cv::Mat &src, cv::Point &pt);
    void findRobots(cv::Mat &src, cv::Mat &dst, COLORS color);
};

namespace ROBOTS {
    void robotsPipeline(const std::string &path);
};



#endif //ROBOTS_HPP
