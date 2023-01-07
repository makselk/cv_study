#include "robots.hpp"


void ROBOTS::robotsPipeline(const std::string &path) {
    cv::Mat src = cv::imread(path);
    cv::Mat red;
    cv::Mat green;
    cv::Mat blue;

    findRobots(src, red, RED);
    findRobots(src, green, GREEN);
    findRobots(src, blue, BLUE);

    cv::Point lamp_pos;
    findLampPos(src, lamp_pos);



    
}

namespace {
    void findLampPos(cv::Mat &src, cv::Point &pt) {
        // Переход к чб изображению
        cv::Mat tmp;
        cv::cvtColor(src, tmp, cv::COLOR_BGR2GRAY);

        // Сглаживание шумов
        cv::medianBlur(tmp, tmp, 5);
        // Пороговая фильтрация
        cv::threshold(tmp, tmp, 250, 255, cv::THRESH_BINARY);

        // Поиск контуров
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(tmp, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

        // Если контуров нет, пропускаем
        if(contours.empty())
            return;
    
        // Поиск наибольшего конутра по площади
        int largest = 0;
        double largest_area;
        for(int i = 0; i != contours.size(); ++i) {
            double area = cv::contourArea(contours[i]);
            if(area > largest_area) {
                largest_area = area;
                largest = i;
            }
        }

        // Поиск моментов у самого большого конутра
        cv::Moments m = cv::moments(contours[largest]);
        // Поиск центра масс
        double center_x = m.m10 / m.m00;
        double center_y = m.m01 / m.m00;

        pt = cv::Point(center_x, center_y);
    }

    void findRobots(cv::Mat &src, cv::Mat &dst, COLORS color) {
        cv::Mat tmp;
        cv::GaussianBlur(src, tmp, cv::Size(5,5), 3);
        cv::cvtColor(tmp, tmp, cv::COLOR_BGR2HSV);

        cv::Mat mask;
        cv::Mat mask1;
        cv::Mat mask2;
        switch(color) {
            case RED:
                cv::inRange(tmp, cv::Scalar(0, 40, 0), cv::Scalar(3, 255, 255), mask1);
                cv::inRange(tmp, cv::Scalar(170, 40, 0), cv::Scalar(180, 255, 255), mask2);
                cv::bitwise_or(mask1, mask2, mask);
                break;
            case GREEN:
                cv::inRange(tmp, cv::Scalar(60, 75, 0), cv::Scalar(80, 255, 255), mask);
                break;
            case BLUE:
                cv::inRange(tmp, cv::Scalar(78, 40, 0), cv::Scalar(105, 255, 255), mask);
                break;
        }

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

        cv::Mat img = src.clone();
        cv::polylines(img, contours, true, cv::Scalar(0,0,255), 3);

        cv::imshow("img", img);
        cv::waitKey(0);
    }
}