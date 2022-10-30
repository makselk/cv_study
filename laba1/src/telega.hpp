#ifndef TELEGA_HPP
#define TELEGA_HPP
#include "opencv2/opencv.hpp"


class Telega {
public:
    explicit Telega(const std::string &background_file,
                    const std::string &model_file);
    ~Telega() = default;
public:
    void run(int start_pos, int amplitude, int T);
private:
    bool telegaSinus(const cv::Mat &background,
                     cv::Mat &model,
                     cv::Mat &trajectory_layer,
                     cv::Point2i &position,
                     int amplitude,
                     float w);
    void drawOnBackground(cv::Mat &model,
                          cv::Mat &background, 
                          const cv::Point2i &position = cv::Point2i(-1, -1));
private:
    cv::Mat background;
    cv::Mat model;
};



#endif //TELEGA_HPP
