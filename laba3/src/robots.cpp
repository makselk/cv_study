#include "robots.hpp"


void ROBOTS::pipelineImage(const std::string &path) {
    // Чтение
    cv::Mat src = cv::imread(path);
    // Обработка
    cv::Mat dst;
    pipeline(src, dst);
    // Визуализация
    cv::imshow("img", dst);
    cv::waitKey(0);
}

void ROBOTS::pipelineVideo(const std::string &path) {
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
        pipeline(frame, dst);

        //Визуализация
        cv::imshow("Roboti", dst);
        if(cv::waitKey(30) == 'q') 
            break;
    }
}

namespace {
    void pipeline(cv::Mat &src, cv::Mat &dst) {
        // Сглаживание
        cv::Mat src_clone;
        cv::GaussianBlur(src, src_clone, cv::Size(5,5), 3);
    
        // Поиск положения лампы
        cv::Point lamp_pos;
        findLampPos(src_clone, lamp_pos);

        // Переход от BGR к HSV
        cv::cvtColor(src_clone, src_clone, cv::COLOR_BGR2HSV);

        // Поиск контуров роботов каждого цвета
        std::vector<std::vector<cv::Point>> red_contours, green_contours, blue_contours;
        findRobots(src_clone, red_contours, RED);
        findRobots(src_clone, green_contours, GREEN);
        findRobots(src_clone, blue_contours, BLUE);

        // Поиск ближайших к лампе роботов каждого цвета
        cv::Point red_nearest, green_nearest, blue_nearest;
        findNearestRobot(red_contours, lamp_pos, red_nearest);
        findNearestRobot(green_contours, lamp_pos, green_nearest);
        findNearestRobot(blue_contours, lamp_pos, blue_nearest);

        // Нанесение опознавательной графики
        dst = src.clone();
        cv::polylines(dst, red_contours, true, cv::Scalar(0, 0, 255), 3);
        cv::polylines(dst, green_contours, true, cv::Scalar(0, 255, 0), 3);
        cv::polylines(dst, blue_contours, true, cv::Scalar(255, 0, 0), 3);
        placeCross(dst, lamp_pos, 10, 3, BLACK);
        placeCross(dst, red_nearest, 10, 3, RED);
        placeCross(dst, green_nearest, 10, 3, GREEN);
        placeCross(dst, blue_nearest, 10, 3, BLUE);
    }

    void findLampPos(cv::Mat &src, cv::Point &pt) {
        // Переход к чб изображению
        cv::Mat tmp;
        cv::cvtColor(src, tmp, cv::COLOR_BGR2GRAY);

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

    void findRobots(cv::Mat &src, std::vector<std::vector<cv::Point>> &contours, COLORS color) {
        // Поиск маски с заданным цветом
        cv::Mat mask;
        cv::Mat mask1;
        cv::Mat mask2;
        switch(color) {
            case RED:
                cv::inRange(src, cv::Scalar(0, 40, 0), cv::Scalar(10, 255, 255), mask1);
                cv::inRange(src, cv::Scalar(170, 40, 0), cv::Scalar(180, 255, 255), mask2);
                cv::bitwise_or(mask1, mask2, mask);
                break;
            case GREEN:
                cv::inRange(src, cv::Scalar(60, 40, 0), cv::Scalar(78, 255, 255), mask);
                break;
            case BLUE:
                cv::inRange(src, cv::Scalar(85, 40, 0), cv::Scalar(105, 255, 255), mask);
                break;
            default:
                cv::inRange(src, cv::Scalar(85, 40, 0), cv::Scalar(105, 255, 255), mask);
                break;
        }

        // Убираем очевидный мусор
        cv::erode(mask, mask, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(15,15)));
        cv::dilate(mask, mask, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(15,15)));

        // Поиск всех контуров на маске
        std::vector<std::vector<cv::Point>> all_contours;
        cv::findContours(mask, all_contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

        // Создание шаблонного контура
        cv::Mat temp = cv::Mat::zeros(50, 50, CV_8UC1);
        cv::circle(temp, cv::Point(25, 25), 20, cv::Scalar(255, 255, 255));
        std::vector<std::vector<cv::Point>> template_contour;
        cv::findContours(temp, template_contour, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

        // Наполнение вектора подходящими по форме конутрами
        contours.clear();
        for(auto &contour: all_contours) {
            if(cv::contourArea(contour) < 100)
                continue;
            if(cv::matchShapes(template_contour[0], contour, cv::CONTOURS_MATCH_I2, 0) < 0.07)
                contours.emplace_back(contour);
        }
    }

    void findNearestRobot(std::vector<std::vector<cv::Point>> &contours,
                          cv::Point &point, 
                          cv::Point &nearest_robot_pt) {
        // Переменная под минимальное расстояние до заданной точки
        double dist = 9999.0;

        for(auto &contour: contours) {
            // Поиск моментов контура
            cv::Moments m = cv::moments(contour);
            // Поиск центра масс
            double center_x = m.m10 / m.m00;
            double center_y = m.m01 / m.m00;
            cv::Point pt = cv::Point(center_x, center_y);
            // Проверка расстояния до заданной точки
            double new_dist = cv::norm(point - pt);
            if(new_dist < dist) {
                nearest_robot_pt = pt;
                dist = new_dist;
            }
        }
    }

    void placeCross(cv::Mat &src, cv::Point &center, int size, int width, COLORS color) {
        // Выбор цвета
        cv::Scalar color_code;
        switch(color) {
            case RED:
                color_code = cv::Scalar(0,0,255);
                break;
            case GREEN:
                color_code = cv::Scalar(0,255,0);
                break;
            case BLUE:
                color_code = cv::Scalar(255,0,0);
                break;
            default:
                color_code = cv::Scalar(0,0,0);
                break;
        }
        // Рисуем линии
        cv::line(src, 
                 cv::Point(center.x - size, center.y), 
                 cv::Point(center.x + size, center.y),
                 color_code,
                 width);
        cv::line(src, 
                 cv::Point(center.x, center.y - size), 
                 cv::Point(center.x, center.y + size),
                 color_code,
                 width);
    }
}
