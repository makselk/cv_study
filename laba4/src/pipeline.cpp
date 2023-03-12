#include "pipeline.h"

cv::Mat PIPELINE::readImage(const std::string& path) {
    // Считываем в чб
    cv::Mat input = cv::imread(path, cv::IMREAD_GRAYSCALE);
    // Подгон размеров, удобных для преобразования фурье
    cv::Mat padded;
    int optimal_rows = cv::getOptimalDFTSize(input.rows);
    int optimal_cols = cv::getOptimalDFTSize(input.cols);
    cv::copyMakeBorder(input, padded,
                       0, input.rows - optimal_rows,
                       0, input.cols - optimal_cols, 
                       cv::BORDER_CONSTANT, cv::Scalar::all(0));
    // Переход в комплексное пространство (двумерный массив - реальная + мнимая части)
    cv::Mat planes[] = {cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F)};
    // Пакуем в двухканальное изображение
    cv::Mat complex_input;
    cv::merge(planes, 2, complex_input);
    return complex_input;
}

cv::Mat PIPELINE::countMagnitude(cv::Mat& complex_image) {
    // Контейнер под разбитое на каналы комплексное изображение
    cv::Mat planes[] = {cv::Mat::zeros(complex_image.size(), CV_32F),
                        cv::Mat::zeros(complex_image.size(), CV_32F)};
    // Разбиваем на одноканальные изображения
    cv::split(complex_image, planes);
    // Абсолютное значение комплексных значений
    cv::Mat mag;
    cv::magnitude(planes[0], planes[1], mag);
    // Перевод в логарифмическую шкалу
    mag += cv::Scalar::all(1);
    log(mag, mag);
    // Нормализация значений между 0 и 1, для визуального различия
    cv::normalize(mag, mag, 0, 1, cv::NORM_MINMAX);
    return mag;
}

cv::Mat PIPELINE::swapQuadrants(cv::Mat& img) {
    cv::Mat out = img.clone();
    // Середины изображения
    int cx = img.cols/2;
    int cy = img.rows/2;
    // Создаем ROI для каждого квадранта
    cv::Mat q0(out, cv::Rect(0, 0, cx, cy));   // Top-Left
    cv::Mat q1(out, cv::Rect(cx, 0, cx, cy));  // Top-Right
    cv::Mat q2(out, cv::Rect(0, cy, cx, cy));  // Bottom-Left
    cv::Mat q3(out, cv::Rect(cx, cy, cx, cy)); // Bottom-Right
    // swap quadrants (Top-Left with Bottom-Right)
    cv::Mat tmp;                           
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    // swap quadrant (Top-Right with Bottom-Left)
    q1.copyTo(tmp);                    
    q2.copyTo(q1);
    tmp.copyTo(q2);
    return out;
}
