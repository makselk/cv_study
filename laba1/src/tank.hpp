#ifndef TANK_HPP
#define TANK_HPP
#include "opencv2/opencv.hpp"


class Tank {
public:
    explicit Tank(const std::string &map_file,
                  const std::string &model_file,
                  const std::string &fire_file = "_");
    ~Tank() = default;
public:
    void run();
private:
    void drawOnMap(cv::Mat &model,
                   cv::Mat &background,
                   const cv::Point2i &position);
    bool outOfMap(cv::Mat &map, 
                  cv::Mat &object, 
                  cv::Point2i &position);
    cv::Mat renderFrame();
private:
    cv::Mat map;
    std::array<cv::Mat, 4> tank_img;
    cv::Point2i tank_pos;
    int tank_orientation = 0;
    cv::Mat ball_img;
    std::vector<cv::Point2i> balls_pos;
};


#endif //TANK_HPP