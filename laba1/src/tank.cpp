#include "tank.hpp"


Ball::Ball(int orientation, cv::Point2i &position, int speed) {
    this->orientation = orientation;
    this->position = position;
    this->speed = speed;
}

void Ball::updatePosition() {
    switch (orientation) {
    case 0:
        position.y -= speed;
        break;
    case 1:
        position.x += speed;
        break;
    case 2:
        position.y += speed;
        break;
    case 3:
        position.x -= speed;
        break;
    default:
        break;
    }
}

Tank::Tank(const std::string &map_file,
           const std::string &tank_file,
           const std::string &ball_file,
           int speed,
           int balls_speed) {
    /// Инициализация изображения фона
    map = cv::imread(map_file);
    /// Подгон удобных размеров
    cv::resize(map, map, cv::Size(1000, 1000));
    /// Инициализация модели танка
    initTankModel(tank_file);
    /// Инициализация объекта снаряда
    initBallModel(ball_file);
    /// Инициализация скоростей
    this->speed = speed;
    this->balls_speed = balls_speed;
}

Tank::Tank(const std::string &map_file,
           const std::string &tank_file,
           int speed,
           int balls_speed,
           const std::string &ball_file) {
    /// Инициализация изображения фона
    map = cv::imread(map_file);
    /// Подгон удобных размеров
    cv::resize(map, map, cv::Size(1000, 1000));
    /// Инициализация модели танка
    initTankModel(tank_file);
    /// Инициализация объекта снаряда
    initBallModel(ball_file);
    /// Инициализация скоростей
    this->speed = speed;
    this->balls_speed = balls_speed;
}

void Tank::initTankModel(const std::string &tank_file) {
    tank_img[0] = cv::imread(tank_file);
    cv::resize(tank_img[0], tank_img[0], cv::Size(50, 50));
    cv::rotate(tank_img[0], tank_img[1], cv::ROTATE_90_CLOCKWISE);
    cv::rotate(tank_img[0], tank_img[2], cv::ROTATE_180);
    cv::rotate(tank_img[0], tank_img[3], cv::ROTATE_90_COUNTERCLOCKWISE);
}

void Tank::initBallModel(const std::string &ball_file) {
    if(ball_file == "_") {
        ball_img[0] = cv::Mat::zeros(cv::Size(30, 30), CV_8UC3);
        cv::circle(ball_img[0], cv::Point2i(15, 15), 15, cv::Scalar(203, 192, 255), -1);
    } else {
        ball_img[0] = cv::imread(ball_file);
        cv::resize(ball_img[0], ball_img[0], cv::Size(30, 30));   
    }
    cv::rotate(ball_img[0], ball_img[1], cv::ROTATE_90_CLOCKWISE);
    cv::rotate(ball_img[0], ball_img[2], cv::ROTATE_180);
    cv::rotate(ball_img[0], ball_img[3], cv::ROTATE_90_COUNTERCLOCKWISE);
}

void Tank::run() {
    tank_pos = cv::Point2i(map.cols / 2 - tank_img[0].cols / 2,
                           map.rows / 2 - tank_img[0].rows / 2);
    while(keyHandler()) {
        updateBalls();
        updateTank();
        cv::Mat output = renderFrame();
        cv::imshow("Invalid Battle City", output);
    }
}

cv::Mat Tank::renderFrame() {
    cv::Mat output = map.clone();
    drawOnMap(tank_img[tank_orientation], output, tank_pos);
    for(auto &ball: balls)
        drawOnMap(ball_img[ball.orientation], output, ball.position);
    return output;
}

bool Tank::keyHandler() {
    int key = cv::waitKey(10);
    switch(key) {
        case 'x':
            return false;
        case -1:
            return true;
        case 81:
            tank_orientation = 3;
            tank_pos.x -= speed;
            break;
        case 82:
            tank_orientation = 0;
            tank_pos.y -= speed;
            break;
        case 83:
            tank_orientation = 1;
            tank_pos.x += speed;
            break;
        case 84:
            tank_orientation = 2;
            tank_pos.y += speed;
            break;
        case 'q':
            shotHandler();
            break;
        default:
            break;
    }
    return true;
}

void Tank::shotHandler() {
    cv::Point2i pos;
    switch(tank_orientation) {
        case 0:
            pos = cv::Point2i(tank_pos.x + tank_img[tank_orientation].cols/2 - ball_img[tank_orientation].cols/2,
                              tank_pos.y - ball_img[tank_orientation].rows);
            break;
        case 1:
            pos = cv::Point2i(tank_pos.x + tank_img[tank_orientation].cols,
                              tank_pos.y + tank_img[tank_orientation].rows/2 - ball_img[tank_orientation].rows/2);
            break;
        case 2:
            pos = cv::Point2i(tank_pos.x + tank_img[tank_orientation].cols/2 - ball_img[tank_orientation].cols/2,
                              tank_pos.y + tank_img[tank_orientation].rows);
            break;
        case 3:
            pos = cv::Point2i(tank_pos.x - ball_img[tank_orientation].cols,
                              tank_pos.y + tank_img[tank_orientation].rows/2 - ball_img[tank_orientation].rows/2);
            break;
        default:
            break;
    }
    balls.emplace_back(tank_orientation, pos, 5);
}

void Tank::updateBalls() {
    for(auto &ball: balls)
        ball.updatePosition();
    for(int i = 0; i != balls.size(); ++i) {
        if(outOfMap(map, ball_img[balls[i].orientation], balls[i].position)) {
            balls.erase(balls.begin() + i);
            --i;
        }
    }
}

void Tank::updateTank() {
    if(!outOfMap(map, tank_img[tank_orientation], tank_pos))
        return;

    switch(tank_orientation) {
        case 0:
            tank_pos.y += speed;
            break;
        case 1:
            tank_pos.x -= speed;
            break;
        case 2:
            tank_pos.y -= speed;
            break;
        case 3:
            tank_pos.x += speed;
            break;
        default:
            break;
    }
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
