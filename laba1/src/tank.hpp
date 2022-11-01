#ifndef TANK_HPP
#define TANK_HPP
#include "opencv2/opencv.hpp"


class Ball {
public:
    explicit Ball(int orientation,
                  cv::Point2i &position,
                  int speed = 5);
    ~Ball() = default;
public:
    void updatePosition();
public:
    int orientation;
    cv::Point2i position;
    int speed;
};

class Tank {
public:
    explicit Tank(const std::string &map_file,
                  const std::string &tank_file,
                  const std::string &ball_file = "_",
                  int speed = 3,
                  int balls_speed = 5);
    explicit Tank(const std::string &map_file,
                  const std::string &tank_file,
                  int speed = 3,
                  int balls_speed = 5,
                  const std::string &ball_file = "_");
    ~Tank() = default;
private:
    void initTankModel(const std::string &model_file);
    void initBallModel(const std::string &model_file);
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
    bool keyHandler();
    void shotHandler();
    void updateBalls();
    void updateTank();
private:
    cv::Mat map;
    std::array<cv::Mat, 4> tank_img;
    cv::Point2i tank_pos;
    int tank_orientation = 0;
    int speed = 3;
private:
    std::array<cv::Mat, 4> ball_img;
    std::vector<Ball> balls;
    int balls_speed;
};


#endif //TANK_HPP