#ifndef ROBOTS_HPP
#define ROBOTS_HPP

#include "opencv4/opencv2/opencv.hpp"


typedef enum {
    RED,
    GREEN,
    BLUE,
    BLACK
} COLORS;

namespace {
    void pipeline(cv::Mat &src, cv::Mat &dst);
    void findLampPos(cv::Mat &src, cv::Point &pt);
    void findRobots(cv::Mat &src, std::vector<std::vector<cv::Point>> &contours, COLORS color);
    void findNearestRobot(std::vector<std::vector<cv::Point>> &contours,
                          cv::Point &point, 
                          cv::Point &nearest_robot_pt);
    void placeCross(cv::Mat &src, cv::Point &center, int size, int width, COLORS color);
};

namespace ROBOTS {
    void pipelineImage(const std::string &path);
    void pipelineVideo(const std::string &path);
};



#endif //ROBOTS_HPP
