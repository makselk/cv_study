#include "gk.hpp"

void GK::defineGK(const std::string &template_path,
                  const std::string &image_path) {
    // Чтение шаблона и исходника
    cv::Mat temp = cv::imread(template_path);
    cv::Mat img = cv::imread(image_path);

    // Перевод изображений в бинарные чб
    cv::Mat img_t;
    cv::GaussianBlur(img, img_t, cv::Size(5,5), 3);
    cv::cvtColor(temp, temp, cv::COLOR_BGR2GRAY);
    cv::cvtColor(img_t, img_t, cv::COLOR_BGR2GRAY);
    cv::threshold(img_t, img_t, 245, 255, cv::THRESH_BINARY_INV);
    cv::threshold(temp, temp, 127, 255, cv::THRESH_BINARY);

    // Поиск контуров на шаблоне и исходнике
    std::vector<std::vector<cv::Point>> template_contour, image_contours;
    cv::findContours(temp, template_contour, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    cv::findContours(img_t, image_contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    
    // Проверка схожести конутров
    for(auto &contour: image_contours) {
        cv::Moments m = cv::moments(contour);
        cv::Point center(m.m10 / m.m00, m.m01 / m.m00);
        if(cv::matchShapes(contour, template_contour[0], cv::CONTOURS_MATCH_I2, 0) < 0.8){
            cv::line(img, 
                     center, 
                     cv::Point(center.x + 20, center.y - 20), 
                     cv::Scalar(0,255,0),
                     3);
            cv::line(img, 
                     center, 
                     cv::Point(center.x - 10, center.y - 10), 
                     cv::Scalar(0,255,0),
                     3);
        } else {
            cv::line(img,
                     cv::Point(center.x + 10, center.y + 10), 
                     cv::Point(center.x - 10, center.y - 10), 
                     cv::Scalar(0,0,255),
                     3);
            cv::line(img, 
                     cv::Point(center.x + 10, center.y - 10), 
                     cv::Point(center.x - 10, center.y + 10), 
                     cv::Scalar(0,0,255),
                     3);
        }
    }
    cv::imshow("img", img);
    cv::waitKey(0);
}