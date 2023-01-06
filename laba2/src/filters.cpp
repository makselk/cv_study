#include "filters.hpp"
#include "opencv2/intensity_transform.hpp"
#include <chrono>


void FILTERS::customBoxFilter(cv::Mat &src, cv::Mat &dst, int kernel_size) {
    // Размер ядра должен быть нечетным
    if(kernel_size % 2 == 0) {
        dst = src.clone();
        return;
    }

    // Создание ядра бокс фильтра
    cv::Mat kernel = cv::Mat::ones(kernel_size, kernel_size, CV_8UC1);
    int kernel_center = kernel_size / 2;
    int kernel_weight = kernel_size * kernel_size;

    // Создание промежуточного изображения, чтобы не повредить исходное
    // в случае, если src и dst одинаковые
    cv::Mat tmp = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);

    // Инициализация положения ОИ
    cv::Rect roi_position(0, 0, kernel_size, kernel_size);
    
    // Проход по всем доступным пикселям (таким, чтобы ядро не выходило за пределы изображения)
    for(int x = 0; x != src.cols - kernel_size; ++x) {
        for(int y = 0; y != src.rows - kernel_size; ++y) {
            // Определение области интереса
            roi_position.x = x;
            roi_position.y = y;
            cv::Mat roi = src(roi_position);

            // Итераторы для прохода по ядру и ОИ
            cv::MatIterator_<uint8_t> it_roi = roi.begin<uint8_t>();
            cv::MatIterator_<uint8_t> it_kernel = kernel.begin<uint8_t>();

            // Переменная для хранения суммы свертки ядра с ОИ
            int weight = 0;

            // Свертка ядра и ОИ
            for(; it_roi != roi.end<uint8_t>(); ++it_roi, ++it_kernel)
                weight += static_cast<int>(*it_kernel) * static_cast<int>(*it_roi);
            
            // Нормировка значения
            double val = static_cast<double>(weight) / static_cast<double>(kernel_weight);
            val = std::round(val);

            // Перенос значения в промежуточное изображение
            tmp.at<uint8_t>(y + kernel_center, x + kernel_center) = static_cast<uint8_t>(val);
        }
    }

    // Присваиваем выходному изображению значение промежуточного
    dst = tmp;
}

void FILTERS::customLaplace(cv::Mat &src, cv::Mat &dst) {
    // Инициализация выходного изображения
    dst = cv::Mat(src.rows, src.cols, CV_8UC1);
    for(int y = 1; y != src.rows - 1; ++y) {
        for(int x = 1; x != src.cols - 1; ++x) {
            // Вычисление суммы
            int sum = 0;
            sum += src.at<uint8_t>(y, x + 1);
            sum += src.at<uint8_t>(y, x - 1);
            sum += src.at<uint8_t>(y + 1, x);
            sum += src.at<uint8_t>(y - 1, x);
            sum -= 4 * static_cast<int>(src.at<uint8_t>(y, x));

            // Приведение int к uint8_t (обрезаем все, что выше 255 и ниже 0)
            dst.at<uint8_t>(y, x) = cv::saturate_cast<uint8_t>(sum);
        }
    }
}

void FILTERS::calculateSimilarity(cv::Mat &src1, cv::Mat &src2) {
    // Проверка размеров изображений
    if(src1.rows != src2.rows || src1.cols != src2.cols) {
        std::cout << "Different image size" << std::endl;
        std::cout << "First:  " << src1.cols << "x" << src1.rows << std::endl;
        std::cout << "Second: " << src2.cols << "x" << src2.rows << std::endl;
        return;
    }

    // Изображение, показывающее различия исходных изображений, и его итератор
    cv::Mat difference = cv::Mat::zeros(src1.rows, src1.cols, CV_8UC1);
    cv::MatIterator_<uint8_t> it_dif = difference.begin<uint8_t>();

    // Итераторы доступа к данным сравниваемых изобаржений
    cv::MatIterator_<uint8_t> it1 = src1.begin<uint8_t>();
    cv::MatIterator_<uint8_t> it2 = src2.begin<uint8_t>();

    // Счетчик совпадающих пикселей
    int similar = 0;
    for(; it1 != src1.end<uint8_t>(); ++it1, ++it2, ++it_dif)
        if(*it1 == *it2) ++similar;
        else *it_dif = 255;

    // Вычисление процента совпадения
    double similarity = static_cast<double>(similar) / static_cast<double>(src1.rows * src1.cols);
    similarity *= 100.0;

    // Вывод результата
    std::cout << "Similarity = " << similarity << "%" << std::endl;
    cv::imshow("Difference", difference);
}

void FILTERS::unsharpMaskBox(cv::Mat &src, cv::Mat &dst, int kernel_size, int sharp) {
    cv::Mat box;
    cv::Mat difference;
    cv::blur(src, box, cv::Size(kernel_size, kernel_size));
    cv::subtract(src, box, difference);

    dst = cv::Mat(src.rows, src.cols, CV_8UC1);
    cv::MatIterator_<uint8_t> it_src = src.begin<uint8_t>();
    cv::MatIterator_<uint8_t> it_dst = dst.begin<uint8_t>();
    cv::MatIterator_<uint8_t> it_dif = difference.begin<uint8_t>();

    for( ; it_src != src.end<uint8_t>(); ++it_src, ++it_dst, ++it_dif) {
        int val = static_cast<int>(*it_src) + sharp * static_cast<int>(*it_dif);
        if(val > 255)
            *it_dst = 255;
        else
            *it_dst = static_cast<uint8_t>(val);
    }
}


void FILTERS::unsharpMaskGauss(cv::Mat &src, cv::Mat &dst, int kernel_size, int sharp) {
    cv::Mat gauss;
    cv::Mat difference;
    cv::GaussianBlur(src, gauss, cv::Size(kernel_size, kernel_size), kernel_size - 2);
    cv::subtract(src, gauss, difference);

    dst = cv::Mat(src.rows, src.cols, CV_8UC1);
    cv::MatIterator_<uint8_t> it_src = src.begin<uint8_t>();
    cv::MatIterator_<uint8_t> it_dst = dst.begin<uint8_t>();
    cv::MatIterator_<uint8_t> it_dif = difference.begin<uint8_t>();

    for( ; it_src != src.end<uint8_t>(); ++it_src, ++it_dst, ++it_dif) {
        int val = static_cast<int>(*it_src) + sharp * static_cast<int>(*it_dif);
        if(val > 255)
            *it_dst = 255;
        else
            *it_dst = static_cast<uint8_t>(val);
    }

    //g(x)=(1−α)f(x) + αg(x)
    //cv::addWeighted();
}

void FILTERS::boxFilterPart1_3(const std::string &path, int kernel_size) {
    // Инициализация исходных изображений
    cv::Mat src = cv::imread(path);
    cv::cvtColor(src, src, cv::COLOR_BGR2GRAY);
    cv::Mat custom;
    cv::Mat built_in;
    
    // Выполнение двух вариантов фильтра среднего
    // Засекаем время
    auto t1 = std::chrono::high_resolution_clock::now();
    FILTERS::customBoxFilter(src, custom, kernel_size);
    auto t2 = std::chrono::high_resolution_clock::now();
    cv::blur(src, built_in, cv::Size(kernel_size, kernel_size));
    auto t3 = std::chrono::high_resolution_clock::now();

    // Вычисение продолжительности вычислений двух методов
    auto custom_duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    auto built_in_duration = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2);
    std::cout << "Custom function time: " << custom_duration.count() << " milliseconds" << std::endl;
    std::cout << "Built in function time: " << built_in_duration.count() << " microseconds" << std::endl;

    // Вычисление схожести полученных изображений
    FILTERS::calculateSimilarity(custom, built_in);

    // Вывод результатов
    cv::imshow("Source", src);
    cv::imshow("Custom", custom);
    cv::imshow("Built in", built_in);
    cv::waitKey(0);
}

void FILTERS::gaussAndBoxComapre4(const std::string &path, int kernel_size) {
    // Инициализация исходных изображений
    cv::Mat src = cv::imread(path);
    cv::cvtColor(src, src, cv::COLOR_BGR2GRAY);
    cv::Mat gauss;
    cv::Mat box;
    cv::Mat difference;

    // Применение фильтров к исходнику
    cv::GaussianBlur(src, gauss, cv::Size(kernel_size, kernel_size), kernel_size - 2);
    cv::blur(src, box, cv::Size(kernel_size, kernel_size));

    // Вычисление разностного изображения
    cv::subtract(gauss, box, difference);

    // Логарифмическая трансформация для удобства просмотра
    cv::intensity_transform::logTransform(difference, difference);

    // Визуализация
    cv::imshow("Source", src);
    cv::imshow("Gaussian Blur", gauss);
    cv::imshow("Box Filter", box);
    cv::imshow("Difference", difference);
    cv::waitKey(0);
}

void FILTERS::unsharpMask5(const std::string &path, int kernel_size, int sharp) {
    // Инициализация исходных изображений
    cv::Mat src = cv::imread(path);
    cv::cvtColor(src, src, cv::COLOR_BGR2GRAY);
    cv::Mat sharp_gauss;
    cv::Mat sharp_box;
    cv::Mat difference;

    // Применение фильтров к исходнику
    FILTERS::unsharpMaskGauss(src, sharp_gauss, kernel_size, sharp);
    FILTERS::unsharpMaskBox(src, sharp_box, kernel_size, sharp);
    
    // Вычисление разностного изображения
    cv::subtract(sharp_gauss, sharp_box, difference);

    // Логарифмическая трансформация для удобства просмотра
    cv::intensity_transform::logTransform(difference, difference);
    
    // Визуализация
    cv::imshow("Source", src);
    cv::imshow("Sharp Gaussian", sharp_gauss);
    cv::imshow("Sharp Box", sharp_box);
    cv::imshow("Difference", difference);
    cv::waitKey(0);
}

void FILTERS::laplace6(const std::string &path) {
    // Инициализация исходного изображений
    cv::Mat src = cv::imread(path);
    cv::cvtColor(src, src, cv::COLOR_BGR2GRAY);

    cv::Mat dst;
    FILTERS::customLaplace(src, dst);

    cv::imshow("Laplace", dst);
    cv::waitKey(0);
}
