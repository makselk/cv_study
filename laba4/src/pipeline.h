#ifndef PIPELINE_H
#define PIPELINE_H

#include <opencv2/opencv.hpp>


namespace PIPELINE {
    // Возвращает двуканальное изображение
    // 1 канал - значения интенсивности пикселей чб изображения
    // 2 канал - нулевая матрица под мнимые значения
    cv::Mat readImage(const std::string& path);

    // Вычисляет одноканальное изображение - 
    // модуль двухканального комплексного
    cv::Mat countMagnitude(cv::Mat& complex_image);

    // Свапает квадранты
    // Левый верхний <--> правый нижний
    // Правый верхний <--> левый нижний
    cv::Mat swapQuadrants(cv::Mat& img);

    // Выводит долю точек, совпадающих с точностью 99%
    // Так как вычисления выводятся во float, добиться идеально
    // одинаковых значений +- невозможно
    double verifyImages(cv::Mat& a, cv::Mat& b);

    // Сравнивает преобразование фурье в лоб и функция в опенсв
    void compareDFT(const std::string& path);
}

#endif //PIPELINE_H
