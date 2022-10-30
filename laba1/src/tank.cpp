#include "tank.hpp"


Tank::Tank(const std::string &map_file,
           const std::string &model_file,
           const std::string &fire_file) {
    /// Инициализация изображения фона
    map = cv::imread(map_file);
    /// Инициализация моделей первого танка
    tank_img[0] = cv::imread(model_file);
    cv::resize(tank_img[0], tank_img[0], cv::Size(50, 50));
    cv::rotate(tank_img[0], tank_img[1], cv::ROTATE_90_CLOCKWISE);
    cv::rotate(tank_img[0], tank_img[2], cv::ROTATE_180);
    cv::rotate(tank_img[0], tank_img[3], cv::ROTATE_90_COUNTERCLOCKWISE);
    /// Инициализация объекта снаряда
    if(fire_file == "_") {
        ball_img = cv::Mat::zeros(cv::Size(30, 30), CV_8UC3);
        cv::circle(ball_img, cv::Point2i(15, 15), 15, cv::Scalar(203, 192, 255), -1);
    } else {
        ball_img = cv::imread(fire_file);
        cv::resize(ball_img, ball_img, cv::Size(30, 30));
    }
    /// Подгон удобных размеров
    cv::resize(map, map, cv::Size(1000, 1000));
}

void Tank::run() {
    tank_pos = cv::Point2i(map.cols / 2 - tank_img[0].cols / 2,
                           map.rows / 2 - tank_img[0].rows / 2);
    while(1) {
        cv::Mat output = renderFrame();
        cv::imshow("Tanks", output);
        int key = cv::waitKey(20);
        switch(key) {
            case 'x':
                return;
            case -1:
                continue;
            case 81:
                tank_orientation = 3;
                break;
            case 82:
                tank_orientation = 0;
                break;
            case 83:
                tank_orientation = 1;
                break;
            case 84:
                tank_orientation = 2;
                break;
        }
    }
}

cv::Mat Tank::renderFrame() {
    cv::Mat output = map.clone();
    drawOnMap(tank_img[tank_orientation], output, tank_pos);
    for(auto &ball_pos: balls_pos)
        drawOnMap(ball_img, output, ball_pos);
    return output;
}

void Tank::drawOnMap(cv::Mat &object,
                     cv::Mat &map,
                     const cv::Point2i &position) {
    /// Цикл перемещения модели на фон
    for(int i = 0; i != object.rows; ++i) {
        for(int j = 0; j != object.cols; ++j) {
            /// Черные пикселим принимаем фоном модели
            if(object.at<cv::Vec3b>(i, j) == cv::Vec3b(0, 0, 0))
                continue;
            map.at<cv::Vec3b>(position.y + i, position.x + j) = object.at<cv::Vec3b>(i, j);
        }
    }
}

bool Tank::outOfMap(cv::Mat &map, cv::Mat &object, cv::Point2i &position) {
    if(position.x < 0)
        return true;
    if(position.y < 0)
        return true;
    if(position.x + object.cols > map.cols)
        return true;
    if(position.y + object.rows > map.rows)
        return true;
    return false;
}
