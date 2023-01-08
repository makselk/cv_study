#include "opencv4/opencv2/opencv.hpp"


int hue_low = 0;
int hue_high = 160;
int s_low = 0;
int s_high = 255;
int v_low = 0;
int v_high = 255;
cv::Mat src;

void onChange(int, void*) {
    cv::Mat tmp = src.clone();
    cv::Mat edges;
    cv::GaussianBlur(tmp, tmp, cv::Size(5, 5), 3);
    cv::cvtColor(tmp, tmp, cv::COLOR_BGR2HSV);
    cv::inRange(tmp, cv::Scalar(hue_low, s_low, v_low), cv::Scalar(hue_high, s_high, v_high), tmp);
    cv::imshow("Thresh", tmp);
}

int main() {
    src = cv::imread("../img/roboti/roi_robotov_1.jpg");
    cv::namedWindow("Thresh", cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("hue low", "Thresh", &hue_low, 255, onChange);
    cv::createTrackbar("hue high", "Thresh", &hue_high, 255, onChange);
    cv::createTrackbar("sat low", "Thresh", &s_low, 255, onChange);
    cv::createTrackbar("sat high", "Thresh", &s_high, 255, onChange);
    cv::createTrackbar("val low", "Thresh", &v_low, 255, onChange);
    cv::createTrackbar("val high", "Thresh", &v_high, 255, onChange);
    onChange(hue_high, 0);
    cv::waitKey(0);

    return 0;
}