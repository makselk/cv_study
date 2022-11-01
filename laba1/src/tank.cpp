#include "tank.hpp"


Ball::Ball(int orientation, cv::Point2i &position, int speed) {
    this->orientation = orientation;
    this->position = position;
    this->speed = speed;
}

void Ball::updatePosition() {
    /// В зависимости от ориентации меняем положение
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
    /// Изменяем размер под нужный, делаем все варианты ориентаций
    cv::resize(tank_img[0], tank_img[0], cv::Size(50, 50));
    cv::rotate(tank_img[0], tank_img[1], cv::ROTATE_90_CLOCKWISE);
    cv::rotate(tank_img[0], tank_img[2], cv::ROTATE_180);
    cv::rotate(tank_img[0], tank_img[3], cv::ROTATE_90_COUNTERCLOCKWISE);
}

void Tank::initBallModel(const std::string &ball_file) {
    /// Если изображение не задано, делаем шарик
    if(ball_file == "_") {
        ball_img[0] = cv::Mat::zeros(cv::Size(30, 30), CV_8UC3);
        cv::circle(ball_img[0], cv::Point2i(15, 15), 15, cv::Scalar(203, 192, 255), -1);
    }
    /// Иначе подгружаем шарик из исходника 
    else{
        ball_img[0] = cv::imread(ball_file);
        cv::resize(ball_img[0], ball_img[0], cv::Size(50, 50));   
    }
    cv::rotate(ball_img[0], ball_img[1], cv::ROTATE_90_CLOCKWISE);
    cv::rotate(ball_img[0], ball_img[2], cv::ROTATE_180);
    cv::rotate(ball_img[0], ball_img[3], cv::ROTATE_90_COUNTERCLOCKWISE);
}

void Tank::run() {
    /// Базовая позиция танка
    tank_pos = cv::Point2i(map.cols / 2 - tank_img[0].cols / 2,
                           map.rows / 2 - tank_img[0].rows / 2);
    while(keyHandler()) {
        /// Обновляем положения снарядов
        updateBalls();
        /// Обновляем положения танка
        updateTank();
        cv::Mat output = renderFrame();
        cv::imshow("Invalid Battle City", output);
    }
}

bool Tank::keyHandler() {
    int key = cv::waitKey(10);
    switch(key) {
        case 'x': //закрыть
            return false;
        case -1: //ничего не было нажато
            return true;
        case 81: //стрелка влево
            tank_orientation = 3;
            tank_pos.x -= speed;
            break;
        case 82: //стрелка вверх
            tank_orientation = 0;
            tank_pos.y -= speed;
            break;
        case 83: //стрелка вправо
            tank_orientation = 1;
            tank_pos.x += speed;
            break;
        case 84: //стрелка вниз
            tank_orientation = 2;
            tank_pos.y += speed;
            break;
        case 'q': //выстрел
            shotHandler();
            break;
        default:
            break;
    }
    return true;
}

void Tank::shotHandler() {
    /// В зависимости от ориентации танка выбираем ориентация и положение снаряда
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
    /// Добавляем объект снаряда в вектор
    balls.emplace_back(tank_orientation, pos, balls_speed);
}

void Tank::updateBalls() {
    /// Обновляем положения каждого снаярда
    for(auto &ball: balls)
        ball.updatePosition();
    /// Проверка на выход за пределы карты
    for(int i = 0; i != balls.size(); ++i) {
        if(outOfMap(map, ball_img[balls[i].orientation], balls[i].position)) {
            balls.erase(balls.begin() + i);
            --i;
        }
    }
}

void Tank::updateTank() {
    /// Проверка на выход за границы карты
    if(!outOfMap(map, tank_img[tank_orientation], tank_pos))
        return;
    /// Если выходит за пределы карты, делаем шаг назад
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

cv::Mat Tank::renderFrame() {
    /// Делаем копию карты
    cv::Mat output = map.clone();
    /// Рисуем на ней танк
    drawOnMap(tank_img[tank_orientation], output, tank_pos);
    /// Рисуем все снаряды
    for(auto &ball: balls)
        drawOnMap(ball_img[ball.orientation], output, ball.position);
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
