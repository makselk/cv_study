#ifndef TANK_HPP
#define TANK_HPP
#include "opencv2/opencv.hpp"


class Ball {
public:
    explicit Ball(int orientation,
                  cv::Point2i &position,
                  int speed = 5);
    ~Ball() = default;
public:
    void updatePosition();
public:
    cv::Point2i position;
    int orientation;
    int speed;
};

class Tank {
public:
    /// Конструктор, где можно пропустить задание скоростей и задать модель снаряда
    explicit Tank(const std::string &map_file,
                  const std::string &tank_file,
                  const std::string &ball_file = "_",
                  int speed = 8,
                  int balls_speed = 5);
    /// Конструктор, где можно пропустить задание модели снаряда и задать скорости
    explicit Tank(const std::string &map_file,
                  const std::string &tank_file,
                  int speed = 8,
                  int balls_speed = 5,
                  const std::string &ball_file = "_");
    ~Tank() = default;
private:
    /// Инициализация полей для танка и снаряда
    void initTankModel(const std::string &model_file);
    void initBallModel(const std::string &model_file);
public:
    /// Публичная функция для запуска программы
    void run();
private:
    /// Обработчик нажатий на кнопку
    bool keyHandler();
    /// Добавляет снаряд и задает его базовый параметры
    void shotHandler();
    /// Обновление положений снарядов и танка
    void updateBalls();
    void updateTank();
    /// Проверка на выход за пределы карты
    bool outOfMap(cv::Mat &map, cv::Mat &object, cv::Point2i &position);
    /// Ренедрит кадр
    cv::Mat renderFrame();
    /// Рисует на крате модельку
    void drawOnMap(cv::Mat &map,
                   cv::Mat &background,
                   const cv::Point2i &position);
private:
    cv::Mat map;                        /// Карта
    std::array<cv::Mat, 4> tank_img;    /// 4 варианта ориентаций модели танка
    cv::Point2i tank_pos;               /// Положение танка
    int tank_orientation = 0;           /// Ориентация танка
    int speed;                          /// Скорость танка
private:
    std::array<cv::Mat, 4> ball_img;    /// 4 варианта ориентаций модели снаряда
    std::vector<Ball> balls;            /// Вектор с существующими объектами снарядов
    int balls_speed;                    /// Скорость снарядов
};


#endif //TANK_HPP