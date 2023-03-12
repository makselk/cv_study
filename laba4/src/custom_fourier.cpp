#include "custom_fourier.h"
#include <math.h>
#include <complex>


namespace {
    // Перевод в комплексный тип
    cv::Mat_<cv::Complex<float>> convertToComplex(cv::Mat& input) {
        cv::Mat_<cv::Complex<float>> out(input.rows, input.cols);
        for(int i = 0; i != input.rows; ++i) {
            for(int j = 0; j != input.cols; ++j) {
                cv::Vec2f val = input.at<cv::Vec2f>(i,j);
                out.at<cv::Complex<float>>(i,j) = cv::Complex<float>(val[0], val[1]);
            }
        }
        return out;
    }
    
    // Перевод из комплексного типа в CV_32FC1
    cv::Mat convertFromComplex(cv::Mat_<cv::Complex<float>>& input) {
        cv::Mat out(input.rows, input.cols, CV_32FC2);
        for(int i = 0; i != out.rows; ++i) {
            for(int j = 0; j != out.cols; ++j) {
                cv::Complex<float> val = input.at<cv::Complex<float>>(i,j);
                out.at<cv::Vec2f>(i,j) = cv::Vec2f(val.re, val.im);
            }
        }
        return out;
    }

    // Построение матрицы со значениями комплексных чисел,
    // использующихся для преобразования фурье
    cv::Mat_<cv::Complex<float>> buildEulerMatrix(int size, bool reverse = false) {
        // Инициализация базовых элементов для матрицы со значениями фурье
        float w = 2 * CV_PI / size;
        cv::Complex<float> w_base(cos(w), -sin(w));
        // Меняем знак в зависимости от типа преобразования
        if(reverse)
            w_base.im *= -1;
        // Инициализация буффера для матрицы со значениями фурье
        std::vector<cv::Complex<float>> w_buffer(size * size + 1);
        w_buffer.at(0) = cv::Complex<float> (1,0);
        for(int i = 1; i != w_buffer.size(); ++i)
            w_buffer.at(i) = w_buffer.at(i - 1) * w_base;
        // Инициализация матрицы со значениями фурье
        cv::Mat_<cv::Complex<float>> out(size, size);
        for(int i = 0; i != size; ++i) {
            for(int j = 0; j != size; ++j) {
                out.at<cv::Complex<float>>(i,j) = w_buffer.at(i*j);
            }
        }
        return out;
    }
}

cv::Mat CUSTOM_FOURIER::dft(cv::Mat& input, bool reverse) {
    // Перевод изображения в комплексное представление
    cv::Mat_<cv::Complex<float>> img = convertToComplex(input);

    // Поиск значений для выполнения преобразования
    cv::Mat_<cv::Complex<float>> euler_mat_cols = buildEulerMatrix(img.cols, reverse);
    cv::Mat_<cv::Complex<float>> euler_mat_rows = buildEulerMatrix(img.rows, reverse);
    
    // Выполнение преобразования
    cv::Mat_<cv::Complex<float>> out_complex_cols = img * euler_mat_cols;
    cv::Mat_<cv::Complex<float>> out_complex = euler_mat_rows * out_complex_cols;
    
    // Возврат к исходному типу
    return convertFromComplex(out_complex);
}
