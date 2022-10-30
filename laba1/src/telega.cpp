#include "telega.hpp"


Telega::Telega(const std::string &background_file,
               const std::string &model_file) {
    /// Инициализация фона и перемещающейся модели
    background = cv::imread("../img/bkg.png");
    model = cv::imread("../img/model.png");
    cv::resize(model, model, cv::Size(35, 70));
    cv::rotate(model, model, cv::ROTATE_180);
}

void Telega::run(int start_pos, int amplitude, int T) {
    /// Переменная текущего положения объекта
    cv::Point2i position(start_pos, 0);
    /// Слой с траекторией
    cv::Mat trajectory_layer = cv::Mat::zeros(background.rows, background.cols, CV_8UC3);
    /// Частота синусоиды
    float w = 2 * 3.14 / T;

    while(telegaSinus(background, model, trajectory_layer, position, amplitude, w)) {
        trajectory_layer = cv::Mat::zeros(background.rows, background.cols, CV_8UC3);
        position = cv::Point2i(start_pos, 0);
    }
}

bool Telega::telegaSinus(const cv::Mat &background,
                         cv::Mat &model,
                         cv::Mat &trajectory_layer,
                         cv::Point2i &position,
                         int amplitude,
                         float w) {
    int base_col = position.x;
    /// Проход фона по вертикали
    while(position.y != background.rows - model.rows / 2) {
        /// Следующая точка положения объекта
        cv::Point2i new_pos = cv::Point2i(base_col + amplitude * sin(w * (position.y + 1)), position.y + 1);
        /// Не даем выйти координате за пределы фона
        if(new_pos.x >= background.cols)
            new_pos.x = background.cols - 1;
        if(new_pos.x < 0)
            new_pos.x = 0;
        /// Линия перемещения объекта
        cv::line(trajectory_layer, new_pos, position, cv::Scalar(203, 192, 255), 2);
        /// Присвоение нового значения переменной позиции объекта
        position = new_pos;
        /// Выходное изображение
        cv::Mat output = background.clone();
        /// Расположение слоя траектории на главном фоне 
        drawOnBackground(trajectory_layer, output);
        /// Расположение модели на главном фоне
        drawOnBackground(model, output, position);
        /// Отображение полученного изображения
        cv::imshow("Telega", output);
        /// По нажатию клавиши 'x' закрыть окно
        if(cv::waitKey(10) == 'x') {
            cv::destroyWindow("Telega");
            return false;
        }
        /// Сохранение изображения на середине экрана
        if(position.y == background.rows / 2)
            cv::imwrite("middle_image.png", output);
    }
    return true;
}

void Telega::drawOnBackground(cv::Mat &model,
                              cv::Mat &background,
                              const cv::Point2i &position) {
    /// Проверка на соответствие размерам
    if(model.rows > background.rows || model.cols > background.cols) {
        std::cout << "Model is bigger than background" << std::endl;
        return;
    }

    /// Создание точки начала перемещения
    cv::Point2i pos;
    if(position == cv::Point2i(-1,-1))
        pos = cv::Point2i(0, 0);
    else
        pos = cv::Point2i(position.x - model.cols / 2, position.y - model.rows / 2);

    /// Проверка на выход модели за пределы поля
    if(pos.x < 0)
        pos.x = 0;
    if(pos.x + model.cols > background.cols)
        pos.x = background.cols - model.cols;
    if(pos.y < 0)
        pos.y = 0;
    if(pos.y + model.rows > background.rows)
        pos.y = background.rows - model.rows;

    /// Цикл перемещения модели на фон
    for(int i = 0; i != model.rows; ++i) {
        for(int j = 0; j != model.cols; ++j) {
            /// Черные пикселим принимаем фоном модели
            if(model.at<cv::Vec3b>(i, j) == cv::Vec3b(0, 0, 0))
                continue;
            background.at<cv::Vec3b>(pos.y + i, pos.x + j) = model.at<cv::Vec3b>(i, j);
        }
    }
}
