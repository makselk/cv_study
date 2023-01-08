#include "teplovisor.hpp"
#include <filesystem>


void TEPLOVISOR::grayPipelineVideo(const std::string &path) {
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
        grayThreeTargets(frame, dst);
        //Визуализация
        cv::imshow("Teplo", dst);
        if(cv::waitKey(30) == 'q') 
            break;
    }
}

void TEPLOVISOR::grayPipelineImage(const std::string &path) {
    cv::Mat frame = cv::imread(path);
    // Инициализация выходного кадра
    cv::Mat dst;
    grayThreeTargets(frame, dst);
    //Визуализация
    cv::imshow("Teplo", dst);
    cv::waitKey(0);
}

void TEPLOVISOR::bgrPipelineImage(const std::string &path) {
    cv::Mat src = cv::imread(path);
    cv::Mat dst;
    bgrOneTarget(src, dst);
    cv::imshow("dst", dst);
    cv::waitKey(0);
}

void TEPLOVISOR::bgrPipelineDirectory(const std::string &path) {
    std::vector<std::string> paths;
    for(const auto& entry: std::filesystem::recursive_directory_iterator(path)) {
        if(!entry.is_directory())
            paths.emplace_back(entry.path().string());
    }
    for(auto &img_path: paths)
        bgrPipelineImage(img_path);
}

namespace {
    void grayOneTarget(cv::Mat &src, cv::Mat &dst) {
        dst = src.clone();
        // Инициализация изображений
        cv::Mat tmp;

        // Перевод в черно-белое
        cv::cvtColor(src, tmp, cv::COLOR_BGR2GRAY);
        // Пороговая фильтрация по интенсивности
        cv::threshold(tmp, tmp, 220, 255, cv::THRESH_BINARY);
        cv::GaussianBlur(tmp, tmp, cv::Size(5,5), 3);

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

    void grayThreeTargets(cv::Mat &src, cv::Mat &dst) {
        dst = src.clone();
        // Инициализация изображения
        cv::Mat tmp;

        // Перевод в черно-белое
        cv::cvtColor(src, tmp, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(tmp, tmp, cv::Size(5,5), 3);
        // Пороговая фильтрация по интенсивности
        cv::threshold(tmp, tmp, 210, 255, cv::THRESH_BINARY);

        // Посик конутров
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(tmp, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

        // Если контуров нет, пропускаем
        if(contours.empty())
            return;

        // Lookup таблица с площадями
        std::vector<double> areas;
        for(int i = 0; i != contours.size(); ++i){
            // Получение площади
            double area = cv::contourArea(contours[i]);
            // Если конутр не совсем мелкий, записываем
            if(area > 10) 
                areas.emplace_back(area);
            // Если слишком мелкий - выкидываем 
            else {
                contours.erase(contours.begin() + i);
                --i;
            }
        }
        
        // Поиск до 3ех наибольших по площади контуров
        std::vector<int> largest_areas; 
        int n_contours = contours.size() > 3 ? 3 : contours.size();
        for(int i = 0; i != n_contours; ++i) {
            // Поиск максимального значения
            auto max_pt = std::max_element(areas.begin(), areas.end());
            // Поиск порядкового номера наибольшего контура
            int j = max_pt - areas.begin();
            // Запоминаем
            largest_areas.emplace_back(j);
            // Обнуляем, чтобы найти следующий
            areas[j] = 0;
        }

        // Рисуем на выходном изображении
        for(auto &iter: largest_areas) {
            // Поиск моментов у самого большого конутра
            cv::Moments m = cv::moments(contours[iter]);
            // Поиск центра масс
            double center_x = m.m10 / m.m00;
            double center_y = m.m01 / m.m00;
            // Рисуем красный крест в центре масс
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
    }

    void bgrOneTarget(cv::Mat &src, cv::Mat &dst) {
        dst = src.clone();
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
}
