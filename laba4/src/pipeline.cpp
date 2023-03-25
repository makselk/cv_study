#include "pipeline.h"
#include "custom_fourier.h"
#include <chrono>
#include <filesystem>


static float box_kernel[3][3] = {{1, 1, 1},
                                 {1, 1, 1},
                                 {1, 1, 1}};
static float laplace_kernel[3][3] = {{0, 1, 0},
                                     {1,-4, 1},
                                     {0, 1, 0}};
static float sobel_kernel_x[3][3] = {{-1, 0, 1},
                                     {-2, 0, 2},
                                     {-1, 0, 1}};
static float sobel_kernel_y[3][3] = {{ 1, 2, 1},
                                     { 0, 0, 0},
                                     {-1,-2,-1}};


cv::Mat PIPELINE::readImage(const std::string& path) {
    // Считываем в чб
    cv::Mat input = cv::imread(path, cv::IMREAD_GRAYSCALE);
    // Подгон размеров, удобных для преобразования фурье
    cv::Mat padded;
    // Оптимальные размеры для встроенного преобразования
    int optimal_rows = cv::getOptimalDFTSize(input.rows);
    int optimal_cols = cv::getOptimalDFTSize(input.cols);
    cv::copyMakeBorder(input, padded,
                       0, input.rows - optimal_rows,
                       0, input.cols - optimal_cols, 
                       cv::BORDER_CONSTANT, cv::Scalar::all(0));
    // Переход в комплексное пространство (двумерный массив - реальная + мнимая части)
    cv::Mat complex_input = toCvComplex(padded);
    return complex_input;
}

cv::Mat PIPELINE::readImagePow2(const std::string& path) {
    // Считываем в чб
    cv::Mat input = cv::imread(path, cv::IMREAD_GRAYSCALE);
    // Подгон размеров, удобных для преобразования фурье
    cv::Mat padded;
    // Это оптимальные размеры для кастомной функции с radix-2 (степени двойки)
    cv::resize(input, padded, cv::Size(512, 512));
    // Переход в комплексное пространство (двумерный массив - реальная + мнимая части)
    cv::Mat complex_input = toCvComplex(padded);
    return complex_input;
}

cv::Mat PIPELINE::toCvComplex(cv::Mat& input) {
    // Переход в комплексное пространство (двумерный массив - реальная + мнимая части)
    cv::Mat planes[] = {cv::Mat_<float>(input), cv::Mat::zeros(input.size(), CV_32F)};
    // Пакуем в двухканальное изображение
    cv::Mat complex;
    cv::merge(planes, 2, complex);
    return complex;
}

cv::Mat PIPELINE::countMagnitude(cv::Mat& complex_image) {
    // Контейнер под разбитое на каналы комплексное изображение
    cv::Mat planes[] = {cv::Mat(complex_image.size(), CV_32FC1),
                        cv::Mat(complex_image.size(), CV_32FC1)};
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

double PIPELINE::verifyImages(cv::Mat& a, cv::Mat& b) {
    int mistakes = 0;
    for(int i = 0; i != a.rows; ++i) {
        for(int j = 0; j != a.cols; ++j) {
            if(a.at<float>(i,j) - b.at<float>(i,j) > 0.01) {
                ++mistakes;
            }
        }
    }
    double result = 1.0 - (double)mistakes / (double)(a.rows * a.cols);
    return result;
}

void PIPELINE::compareDFT(const std::string& path) {
    cv::Mat input = PIPELINE::readImagePow2(path);
    // DFT opencv
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    cv::Mat opencv_fourier;
    cv::dft(input, opencv_fourier);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    int64_t opencv_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    
    // Custom naive fourier
    begin = std::chrono::steady_clock::now();
    cv::Mat naive_fourier = CUSTOM_FOURIER::dft(input);
    end = std::chrono::steady_clock::now();
    int64_t naive_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    // Custom cooley-turkey
    begin = std::chrono::steady_clock::now();
    cv::Mat cooley_fourier = CUSTOM_FOURIER::fft(input);
    end = std::chrono::steady_clock::now();
    int64_t cooley_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    std::cout << "OpenCV time:" << opencv_time << "[ms]" << std::endl;
    std::cout << "Naive time :" << naive_time << "[ms]" << std::endl;
    std::cout << "Cooley time:" << cooley_time << "[ms]" << std::endl;
    
    // Magnitude
    cv::Mat opencv_magnitude = PIPELINE::countMagnitude(opencv_fourier);
    cv::Mat naive_magnitude = PIPELINE::countMagnitude(naive_fourier);
    cv::Mat cooley_magnitude = PIPELINE::countMagnitude(cooley_fourier);

    // Сравнение совпадения 
    double ocv_naive = verifyImages(opencv_magnitude, naive_magnitude);
    double ocv_cooley = verifyImages(opencv_magnitude, cooley_magnitude);
    std::cout << "Overlap opencv/naive: " << (double)ocv_naive << std::endl;
    std::cout << "Overlap opencv/cooley: " << (double)ocv_cooley << std::endl;

    // Свапаем квадранты для красивого просмотра
    cv::Mat opencv_swapped = PIPELINE::swapQuadrants(opencv_magnitude);
    cv::Mat naive_swapped = PIPELINE::swapQuadrants(naive_magnitude);
    cv::Mat cooley_swapped = PIPELINE::swapQuadrants(cooley_magnitude);

    // Обратное преобразование
    cv::Mat naive_reverse = CUSTOM_FOURIER::dft(naive_fourier, true);
    cv::Mat reverse_magnitude = PIPELINE::countMagnitude(naive_reverse);
    
    // Показ
    cv::imshow("opencv_fourier", opencv_swapped);
    cv::imshow("naive_fourier", naive_swapped);
    cv::imshow("cooley_fourier", cooley_swapped);
    cv::imshow("naive_fourier_reverse", reverse_magnitude);
    cv::waitKey(0);
}

void PIPELINE::directoryCompareDFT(const std::string& path) {
    std::vector<std::string> paths;
    for(const auto& entry: std::filesystem::recursive_directory_iterator(path)) {
        if(!entry.is_directory())
            paths.emplace_back(entry.path().string());
    }
    for(auto &img_path: paths)
        compareDFT(img_path);
}

void PIPELINE::initConvolveDFT(cv::Mat& img, 
                               KERNEL filter,
                               cv::Mat& img_complex,
                               cv::Mat& kernel_complex) {
    // Инициализируем ядро
    cv::Mat kernel;
    switch(filter) {
        case BOX:
            kernel = cv::Mat(3,3,CV_32F,&box_kernel);
            break;
        case LAPLACE:
            kernel = cv::Mat(3,3,CV_32F,&laplace_kernel);
            break;
        case SOBEL_X:
            kernel = cv::Mat(3,3,CV_32F,&sobel_kernel_x);
            break;
        case SOBEL_Y:
            kernel = cv::Mat(3,3,CV_32F,&sobel_kernel_y);
            break;
    }
    // Приводим к общему размеру для выполнения свертки
    cv::Size common_size = img.size() + kernel.size();
    cv::Mat img_ = cv::Mat::zeros(common_size, img.type());
    cv::Mat kernel_ = cv::Mat::zeros(common_size, kernel.type());
    
    // Размещаем исходники в верхнем левом углу
    img.copyTo(img_(cv::Rect(0, 0, img.cols, img.rows)));
    kernel.copyTo(kernel_(cv::Rect(0, 0, kernel.cols, kernel.rows)));

    // Переход в комплексное пространство
    img_complex = toCvComplex(img_);
    kernel_complex = toCvComplex(kernel_);
}

cv::Mat PIPELINE::reverseDFT(cv::Mat& spectrum,
                             cv::Mat& base_img,
                             KERNEL filter) {   
    // Обратное преобразование сразу в вещественные значения
    cv::Mat inverse;
    cv::idft(spectrum, inverse, cv::DFT_SCALE | cv::DFT_REAL_OUTPUT);
    // Обрезаем до исходного размера
    cv::Mat inverse_ = inverse(cv::Rect(1,1, base_img.cols, base_img.rows)).clone();
    // Ответвление при работе с box fliter
    if(filter == KERNEL::BOX) {
        cv::normalize(inverse_, inverse_, 0, 1, cv::NORM_MINMAX);
        return inverse_;
    }
    // Перевод в 8-битное изображение
    cv::Mat out;
    inverse_.convertTo(out, CV_8U);
    return out;
}

cv::Mat PIPELINE::convolveDFT(cv::Mat& img, KERNEL filter) {
    // Инициализация ядра и изображения в комплексном виде нужного размера
    cv::Mat img_complex, kernel_complex;
    initConvolveDFT(img, filter, img_complex, kernel_complex);

    // Преобразование фурье для изображения и ядра
    cv::Mat img_f, kernel_f;
    cv::dft(img_complex, img_f, cv::DFT_COMPLEX_OUTPUT);
    cv::dft(kernel_complex, kernel_f, cv::DFT_COMPLEX_OUTPUT);

    cv::Mat img_magn = countMagnitude(img_f);
    cv::Mat kernel_magn = countMagnitude(kernel_f);
    cv::Mat img_magn_swapped = swapQuadrants(img_magn);
    cv::Mat kernel_magn_swapped = swapQuadrants(kernel_magn);

    // Свертка
    cv::Mat conv_f;
    cv::mulSpectrums(img_f, kernel_f, conv_f, 0);

    // Обратное преобразование в пригодный для просмтра вид
    cv::Mat out = reverseDFT(conv_f, img, filter);
    cv::imshow("Image magnitude", img_magn_swapped);
    cv::imshow("Kernel magnitude", kernel_magn_swapped);
    cv::imshow("Convolved", out);
    cv::waitKey(0);

    return out;
}

void PIPELINE::startConvolveDFT(const std::string& path) {
    cv::Mat img = cv::imread(path, cv::IMREAD_GRAYSCALE);
    convolveDFT(img, KERNEL::BOX);
    convolveDFT(img, KERNEL::LAPLACE);
    convolveDFT(img, KERNEL::SOBEL_X);
    convolveDFT(img, KERNEL::SOBEL_Y);
}


void PIPELINE::cutSpectrum(cv::Mat& img, double w) {
    // Параметры для обрезания частот
    int radius = img.rows < img.cols ? img.rows * w / 2 : img.cols * w / 2;
    cv::Point center(img.cols / 2, img.rows / 2);

    // Перевод в комплексное представление и преобразование фурье
    cv::Mat img_complex = toCvComplex(img);
    cv::Mat img_fourier;
    cv::dft(img_complex, img_fourier, cv::DFT_COMPLEX_OUTPUT);
    
    // Красивый спектр
    cv::Mat spectrum = swapQuadrants(img_fourier);
    // Матрицы, в которых останутся верхние и нижние частоты
    cv::Mat spectrum_upper = spectrum.clone();
    cv::Mat spectrum_lower;
    // Обнуляем центральные значения - убираем низкие частоты (оставляем высокие)
    cv::circle(spectrum_upper, center, radius, cv::Scalar::all(0.0), -1);
    // Исключающее или, чтобы убрать высокие частоты (оставить нижние)
    cv::bitwise_xor(spectrum, spectrum_upper, spectrum_lower);
    
    // Вычисляем магнитуду
    cv::Mat magnitude = countMagnitude(spectrum);
    // Производим те же операции, что и со спектром, чтоб не уплывало среднее значение интенсивности
    cv::Mat magnitude_upper = magnitude.clone();
    cv::Mat magnitude_lower;
    cv::circle(magnitude_upper, center, radius, cv::Scalar::all(0.0), -1);
    cv::bitwise_xor(magnitude, magnitude_upper, magnitude_lower);

    // Возвращаем квадранты в исходное положение
    spectrum_upper = swapQuadrants(spectrum_upper);
    spectrum_lower = swapQuadrants(spectrum_lower);

    // Обратное преобразование
    cv::Mat inverse_upper, inverse_lower, out_upper, out_lower;
    cv::idft(spectrum_upper, inverse_upper, cv::DFT_SCALE | cv::DFT_REAL_OUTPUT);
    cv::idft(spectrum_lower, inverse_lower, cv::DFT_SCALE | cv::DFT_REAL_OUTPUT);
    inverse_upper.convertTo(out_upper, CV_8U);
    inverse_lower.convertTo(out_lower, CV_8U);

    // Показ всего
    cv::imshow("Original", img);
    cv::imshow("Out upper", out_upper);
    cv::imshow("Out lower", out_lower);
    cv::imshow("Magnitude original", magnitude);
    cv::imshow("Magnitude upper", magnitude_upper);
    cv::imshow("Magnitude lower", magnitude_lower);
    cv::waitKey(0);

}

void PIPELINE::startCutSpectrum(const std::string& path, double radius) {
    cv::Mat img = cv::imread(path, cv::IMREAD_GRAYSCALE);
    cv::resize(img, img, cv::Size(550, 350));
    cutSpectrum(img, radius);
}
