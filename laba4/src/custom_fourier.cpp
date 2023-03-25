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
    
    // Перевод из комплексного типа в CV_32FC2
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

    // Изменяет направление битов в переменной
    uint32_t reverseBits(uint32_t n) {
        uint32_t ans = 0;
        for(int i = 31; i >= 0; i--){
            ans |= (n & 1) << i;
            n >>= 1;
        }
        return ans;
    }

    // Получает длину одномерного массива и выдает правильную последовательность элементов в нем
    std::vector<uint32_t> binaryReverseSequence(uint32_t sequence_len) {
        std::vector<uint32_t> out(sequence_len);
        int base = log2(sequence_len);
        for(uint32_t i = 0; i != out.size(); ++i)
            out[i] = reverseBits(i) >> (32 - base);
        return out;
    }

    // Выполняет двоичную инверсию для порядка элементов одномерно массива
    cv::Mat_<cv::Complex<float>> binaryReverseImage1D(cv::Mat_<cv::Complex<float>>& img) {
        cv::Mat_<cv::Complex<float>> out(1, img.cols);
        std::vector<uint32_t> sequence = binaryReverseSequence(img.cols);
        for(int i = 0; i != sequence.size(); ++i) {
            out.at<cv::Complex<float>>(0, sequence[i]) = img.at<cv::Complex<float>>(0, i);
        }
        return out;
    }

    // Перевод в одномерный массив для выполнения cooley-turkey
    cv::Mat_<cv::Complex<float>> convertTo1D(cv::Mat_<cv::Complex<float>>& img,
                                             bool rows_dir) {
        int size = img.rows * img.cols;
        cv::Mat_<cv::Complex<float>> out(1, size);
        // Собираем в единое целое по строчкам
        if(rows_dir) {
            for(int i = 0; i != img.rows; ++i) {
                int pos = i * img.cols;
                for(int j = 0; j != img.cols; ++j) {
                    out.at<cv::Complex<float>>(0, pos + j) = img.at<cv::Complex<float>>(i,j);
                }
            }
        } 
        // По столбцам
        else {
            for(int i = 0; i != img.cols; ++i) {
                int pos = i * img.rows;
                for(int j = 0; j != img.rows; ++j) {
                    out.at<cv::Complex<float>>(0, pos + j) = img.at<cv::Complex<float>>(j,i);
                }
            }
        }
        //out = binaryReverseImage1D(out);
        return out;
    }
    
    // Перевод из одномерного в двумерный
    cv::Mat_<cv::Complex<float>> convertTo2D(cv::Mat_<cv::Complex<float>>& img,
                                             bool rows_dir,
                                             int out_rows,
                                             int out_cols) {
        cv::Mat_<cv::Complex<float>> out(out_rows, out_cols);
        if(rows_dir) {
            for(int i = 0; i != out_rows; ++i) {
                int pos = i * out_cols;
                for(int j = 0; j != out_cols; ++j) {
                    out.at<cv::Complex<float>>(i,j) = img.at<cv::Complex<float>>(0, pos + j);
                }
            }
        }
        else {
            for(int i = 0; i != out_cols; ++i) {
                int pos = i * out_rows;
                for(int j = 0; j != out_rows; ++j) {
                    out.at<cv::Complex<float>>(j,i) = img.at<cv::Complex<float>>(0, pos + j);
                }
            }
        }
        cv::rotate(out, out, 0);
        cv::flip(out, out, 1);
        return out;
    }

    // Рекурсивный алгоритм для перобразования фурье
    void cooley_turkey(cv::Mat_<cv::Complex<float>>& input) {
        int N = input.cols;
        if(N <= 1)
            return;
        
        // Создаем 2 подпоследовательности
        cv::Mat_<cv::Complex<float>> odd(1, N / 2);
        cv::Mat_<cv::Complex<float>> even(1, N / 2);

        // Заполняем
        for(int i = 0; i != N/2; ++i) {
            even.at<cv::Complex<float>>(0, i) = input.at<cv::Complex<float>>(0, 2*i);
            odd.at<cv::Complex<float>>(0, i) = input.at<cv::Complex<float>>(0, 2*i+1);
        }

        // Рекрсивный алгоритм для каждой части
        cooley_turkey(even);
        cooley_turkey(odd);

        float w = 2 * CV_PI / N;
        cv::Complex<float> w_base(cos(w), -sin(w));
        // Инициализация буффера для матрицы со значениями фурье
        std::vector<cv::Complex<float>> w_buffer(N/2);
        w_buffer.at(0) = cv::Complex<float> (1,0);
        for(int i = 1; i != w_buffer.size(); ++i)
            w_buffer.at(i) = w_buffer.at(i - 1) * w_base;

        // Преобразование
        for(int i = 0; i != N/2; ++i) {
            cv::Complex<float> t = w_buffer[i] * odd.at<cv::Complex<float>>(0,i);
            input.at<cv::Complex<float>>(0,i) = even.at<cv::Complex<float>>(0,i) + t;
            input.at<cv::Complex<float>>(0,N/2 + i) = even.at<cv::Complex<float>>(0,i) - t;
        }
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

cv::Mat CUSTOM_FOURIER::fft(cv::Mat& input) {
    //cv::Mat inverse = input.clone();
    // Перевод изображения в комплексное представление
    cv::Mat_<cv::Complex<float>> img = convertToComplex(input);

    // Проход по строкам
    cv::Mat_<cv::Complex<float>> rows_image = convertTo1D(img, true);
    cooley_turkey(rows_image);
    cv::Mat_<cv::Complex<float>> output = convertTo2D(rows_image, true, img.rows, img.cols);

    // Возврат к исходному типу
    return convertFromComplex(output);
}

cv::Mat CUSTOM_FOURIER::fft2(cv::Mat& src) {
    int r = src.rows;
    int c = src.cols;
    src.convertTo(src, CV_32FC1);
    cv::Mat dst;
    dst.create(r, c, CV_32FC2);
    for(int i = 0; i < r; i++) {
        for(int j = 0; j < c; j++) {
            dst.at<cv::Vec2f>(i,j)[0] = src.at<float>(i,j);
            dst.at<cv::Vec2f>(i,j)[1] = 0.0;
        }
    }
    int log_r = log2(r);
    int log_c = log2(c);
    for(int i = 0; i < r; i++) {
        for(int j = 0; j < c; j++) {
            int rp = i;
            int cp = j;
            for(int k = 0; k < log_c; k++) {
                int new_cp = 0;
                if(cp & (1<<k)) {
                    new_cp |= 1<<(log_c-k-1);
                }
                cp = new_cp;
            }
            for(int k = 0; k < log_r; k++) {
                int new_rp = 0;
                if(rp & (1<<k)) {
                    new_rp |= 1<<(log_r-k-1);
                }
                rp = new_rp;
            }
            if(rp < i) {
                cv::Vec2f temp = dst.at<cv::Vec2f>(i,j);
                dst.at<cv::Vec2f>(i,j) = dst.at<cv::Vec2f>(rp,cp);
                dst.at<cv::Vec2f>(rp,cp) = temp;
            }
        }
    }
    cv::Mat_<cv::Complex<float>> out(dst.rows, dst.cols);
    for(int n = 2; n <= r; n<<=1) {
        for(int i = 0; i < r; i+=n) {
            int half_n = n>>1;
            for(int j = 0; j < half_n; j++) {
                float rp = cos(2*M_PI*j/n);
                float ip = -sin(2*M_PI*j/n);
                cv::Complex<float> w(rp,ip);
                cv::Vec2f u_ = dst.at<cv::Vec2f>(i+j+half_n,0);
                cv::Complex<float> u(u_.val[0], u_.val[1]);
                u = u * w;
                cv::Vec2f x_ = dst.at<cv::Vec2f>(i+j,0);
                cv::Complex<float> x(x_.val[0], x_.val[1]);
                out.at<cv::Complex<float>>(i+j+half_n,0) = x - u;
                out.at<cv::Complex<float>>(i+j,0) = x + u;
            }
        }
    }
    return convertFromComplex(out);
}
