#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>


int main() {
    std::string save_path = "../tags/tag.png";
    cv::Mat tag;
    int id = 21;
    int side_pixels = 300;
    int type = cv::aruco::DICT_4X4_50;
    cv::Ptr<cv::aruco::Dictionary> dict = cv::aruco::Dictionary::get(type);
    cv::aruco::drawMarker(dict, id, side_pixels, tag);
    
    cv::imwrite(save_path, tag);
    cv::imshow("tag", tag);
    cv::waitKey(0);
    return 0;
}