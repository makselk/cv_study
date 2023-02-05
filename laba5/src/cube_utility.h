#ifndef CUBE_UTILITY_H
#define CUBE_UTILITY_H

#include <opencv2/opencv.hpp>


namespace CUBE_UTILITY {
    // Запускает программу по поиску маркеров на изображении и рисует на этих маркерах кубы
    //                                                                                      
    // Первоначально словарь для поиска не задан. Для его определения необходимо в рабочем окне
    // нажать на 'e'. Словарь будет автоматически определен по имеющимся на изображении маркерам
    // В случае отсутствия маркеров будет выведено соответствующее сообщение, а словарь обнулен
    //                                                                                      
    // При использовании изоображения с камеры устройства, путь к видео указывать не требуется
    void start(float marker_length,
               const std::string &calibration_path,
               const std::string &detector_params_path,
               const std::string &video_path = "");
}


#endif //CUBE_UTILITY_H