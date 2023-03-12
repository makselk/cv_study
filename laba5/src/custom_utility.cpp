#include "custom_utility.h"
#include <opencv2/aruco.hpp>


int CUSTOM_UTILITY::defineDictionary(cv::Mat &img) {
    // Счетчик количества найденных aruco
    std::vector<int> counter;

    // буфферы
    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f>> corners;
    
    for(int i = 3; i < 16; i+=4) {
        cv::aruco::detectMarkers(img,
                                 cv::aruco::Dictionary::get(i),
                                 corners,
                                 ids);
        counter.emplace_back(ids.size());
    }

    // Поиск типа словаря с наибольшим количеством определенных aruco
    int max = 0;
    int dictionary = 0;
    for(int i = 0; i != counter.size(); ++i) {
        if(counter[i] <= max)
            continue;
        max = counter[i];
        dictionary = 3 + 4*i;
    }

    // Отдельно проверка словаря DICT_ARUCO_ORIGINAL, 
    // тк он по порядковому номеру в алгоритм не вписался
    cv::aruco::detectMarkers(img,
                             cv::aruco::Dictionary::get(cv::aruco::DICT_ARUCO_ORIGINAL),
                             corners,
                             ids);
    if(ids.size() > max)
        dictionary = cv::aruco::DICT_ARUCO_ORIGINAL;

    //Сообщение пользователю
    if(dictionary == cv::aruco::DICT_ARUCO_ORIGINAL)
        std::cout << "DICT_ARUCO_ORIGINAL defined" << std::endl;
    else if(dictionary == cv::aruco::DICT_4X4_1000)
        std::cout << "DICT_4X4_1000 defined" << std::endl;
    else if(dictionary == cv::aruco::DICT_5X5_1000)
        std::cout << "DICT_5X5_1000 defined" << std::endl;
    else if(dictionary == cv::aruco::DICT_6X6_1000)
        std::cout << "DICT_6X6_1000 defined" << std::endl;
    else if(dictionary == cv::aruco::DICT_7X7_1000)
        std::cout << "DICT_7X7_1000 defined" << std::endl;
    else 
        std::cout << "fuck you leatherman" << std::endl;
    
    return dictionary;
}
