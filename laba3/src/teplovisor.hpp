#ifndef TEPLOVISOR_HPP
#define TEPLOVISOR_HPP

#include "opencv4/opencv2/opencv.hpp"

void teplovisor_gray_largest(cv::Mat &src, cv::Mat &dst);
void teplovisor_gray_largest(cv::Mat &src,
                            cv::Mat &dst,
                            int cross_size,
                            int cross_width);
void teplovisor_bgr(cv::Mat &src, cv::Mat &dst);
void teplovisor_bgr_pipeline(const std::string &path);
void teplovisor_gray_largest_pipeline(const std::string &path);
void teplovisor_gray_largest_pipeline(const std::string &path,
                                      int cross_size, 
                                      int cross_width);

#endif //TEPLOVISOR_HPP
