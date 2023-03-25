#ifndef PIPELINE_H
#define PIPELINE_H

#include <opencv2/opencv.hpp>


namespace PIPELINE {
    // Возвращает двуканальное изображение
    // 1 канал - значения интенсивности пикселей чб изображения
    // 2 канал - нулевая матрица под мнимые значения
    cv::Mat readImage(const std::string& path);

    // Возвращает двуканальное изображение (512х512)
    // 1 канал - значения интенсивности пикселей чб изображения
    // 2 канал - нулевая матрица под мнимые значения
    cv::Mat readImagePow2(const std::string& path);

    // Переводит изображение в CV_32FC2 (Im+Re)
    cv::Mat toCvComplex(cv::Mat& input);

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

    // Для всех изображений в директории
    void directoryCompareDFT(const std::string& path);

//////////////////////////////////////////////////////////////////////////
    typedef enum {
        BOX,
        LAPLACE,
        SOBEL_X,
        SOBEL_Y
    } KERNEL;

    // Инициализация ядра и изображения в комплексном виде нужного размера
    void initConvolveDFT(cv::Mat& img, 
                         KERNEL filter,
                         cv::Mat& img_complex,
                         cv::Mat& kernel_complex);

    // Обратное преобразование 2д спектра фурье
    cv::Mat reverseDFT(cv::Mat& spectrum,
                       cv::Mat& base_img,
                       KERNEL filter);

    // Производит свертку с помощью перобразования фурье
    cv::Mat convolveDFT(cv::Mat& img, KERNEL kernel);

    // Производит свертку изображения по пути с ядрами,
    // представленными в PIPELINE::KERNEL
    void startConvolveDFT(const std::string& path);

////////////////////////////////////////////////////////////////////////////

    // Обрезает спектр кругом
    // radius определяет ширину круга (0 - min; 1 - max) 
    void cutSpectrum(cv::Mat& img, double radius);
    void startCutSpectrum(const std::string& path, double radius);

////////////////////////////////////////////////////////////////////////////

    // Запускает процесс выполнения корреляции каждого изображения номера
    // из директории plates_dir с шаблоном символа из директории templates_dir
    void startCorelateLicencePlates(const std::string& plates_dir,
                                    const std::string& templates_dir);
    // Выполняет корреляцию двух изображений
    void correlationLicencePlates(const std::string& plate_path,
                                  const std::string& template_path);
    // Инициализирует изображения необходимого размера со значениями комплексного типа 
    void initCorrelationDFT(cv::Mat& plate,
                            cv::Mat& temp,
                            cv::Mat& plate_complex,
                            cv::Mat& temp_complex);
    // Восстанавливает исходный размер номера
    void restoreSize(cv::Mat& result, cv::Mat& src);
}

#endif //PIPELINE_H
