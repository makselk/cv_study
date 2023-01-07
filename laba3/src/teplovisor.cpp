#include "teplovisor.hpp"


void teplovisor_gray_largest(cv::Mat &src,
                             cv::Mat &dst,
                             int cross_size,
                             int cross_width) {
    // Инициализация изображений
    cv::Mat tmp;
    dst = src.clone();

    // Перевод в черно-белое
    cv::cvtColor(src, tmp, cv::COLOR_BGR2GRAY);
    // Пороговая фильтрация по интенсивности
    cv::threshold(tmp, tmp, 220, 255, cv::THRESH_BINARY);

    // Посик конутров
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

    // Рисуем красный крест в центре масс
    cv::line(dst,
             cv::Point(center_x, center_y + cross_size), 
             cv::Point(center_x, center_y - cross_size), 
             cv::Scalar(0,0,255), 
             cross_width);
    cv::line(dst, 
             cv::Point(center_x + cross_size, center_y), 
             cv::Point(center_x - cross_size, center_y), 
             cv::Scalar(0,0,255), 
             cross_width);
}

void teplovisor_gray_largest(cv::Mat &src,
                             cv::Mat &dst) {
    // Инициализация изображений
    cv::Mat tmp;
    dst = src.clone();

    // Перевод в черно-белое
    cv::cvtColor(src, tmp, cv::COLOR_BGR2GRAY);
    // Пороговая фильтрация по интенсивности
    cv::threshold(tmp, tmp, 220, 255, cv::THRESH_BINARY);

    // Посик конутров
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

    // Рисуем красный крест в центре масс
    cv::line(dst,
             cv::Point(center_x, center_y + 10), 
             cv::Point(center_x, center_y - 10), 
             cv::Scalar(0,0,255), 
             3);
    cv::line(dst, 
             cv::Point(center_x + 10, center_y), 
             cv::Point(center_x - 10, center_y), 
             cv::Scalar(0,0,255), 
             3);
}

void teplovisor_bgr(cv::Mat &src, cv::Mat &dst) {
    // Перевод изображения в цветовое пространство BGR
    cv::Mat tmp;
    cv::cvtColor(src, tmp, cv::COLOR_BGR2HSV);
    cv::inRange(tmp, cv::Scalar(0, 0, 0), cv::Scalar(33, 255, 255), tmp);

    // Посик конутров
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

    // Рисуем красный крест в центре масс
    dst = src.clone();
    cv::line(dst,
             cv::Point(center_x, center_y + 15), 
             cv::Point(center_x, center_y - 15), 
             cv::Scalar(0,0,255), 
             3);
    cv::line(dst, 
             cv::Point(center_x + 15, center_y), 
             cv::Point(center_x - 15, center_y), 
             cv::Scalar(0,0,255), 
             3);
}

void teplovisor_gray_largest_pipeline(const std::string &path) {
    // Класс для видео
    cv::VideoCapture cap(path);
    for(;;) {
        // Вытаскиваем один кадр из видео
        cv::Mat frame;
        cap >> frame;
        // Если кадр пустой, значит видос закончился
        if(frame.empty()) 
            break;

        // Инициализация выходного кадра
        cv::Mat dst;
        teplovisor_gray_largest(frame, dst);

        //Визуализация
        cv::imshow("Teplo", dst);
        if(cv::waitKey(30) == 'q') 
            break;
    }
}

void teplovisor_gray_largest_pipeline(const std::string &path, int cross_size, int cross_width) {
    // Класс для видео
    cv::VideoCapture cap(path);
    for(;;) {
        // Вытаскиваем один кадр из видео
        cv::Mat frame;
        cap >> frame;
        // Если кадр пустой, значит видос закончился
        if(frame.empty()) 
            break;

        // Инициализация выходного кадра
        cv::Mat dst;
        teplovisor_gray_largest(frame, dst, cross_size, cross_width);

        //Визуализация
        cv::imshow("Teplo", dst);
        if(cv::waitKey(30) == 'q') 
            break;
    }
}

void teplovisor_bgr_pipeline(const std::string &path) {
    cv::Mat src = cv::imread(path);
    cv::Mat dst;
    teplovisor_bgr(src, dst);
    cv::imshow("dst", dst);
    cv::waitKey(0);
}