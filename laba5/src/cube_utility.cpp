#include "cube_utility.h"
#include "custom_utility.h"
#include "aruco_samples_utility.hpp"
#include <opencv2/aruco.hpp>


namespace {
    // Считывает парметры детектирования из файла
    static bool readDetectorParameters(std::string filename, cv::aruco::DetectorParameters &params) {
        cv::FileStorage fs(filename, cv::FileStorage::READ);
        if(!fs.isOpened())
            return false;

        fs["adaptiveThreshWinSizeMin"] >> params.adaptiveThreshWinSizeMin;
        fs["adaptiveThreshWinSizeMax"] >> params.adaptiveThreshWinSizeMax;
        fs["adaptiveThreshWinSizeStep"] >> params.adaptiveThreshWinSizeStep;
        fs["adaptiveThreshConstant"] >> params.adaptiveThreshConstant;
        fs["minMarkerPerimeterRate"] >> params.minMarkerPerimeterRate;
        fs["maxMarkerPerimeterRate"] >> params.maxMarkerPerimeterRate;
        fs["polygonalApproxAccuracyRate"] >> params.polygonalApproxAccuracyRate;
        fs["minCornerDistanceRate"] >> params.minCornerDistanceRate;
        fs["minDistanceToBorder"] >> params.minDistanceToBorder;
        fs["minMarkerDistanceRate"] >> params.minMarkerDistanceRate;
        fs["cornerRefinementWinSize"] >> params.cornerRefinementWinSize;
        fs["cornerRefinementMaxIterations"] >> params.cornerRefinementMaxIterations;
        fs["cornerRefinementMinAccuracy"] >> params.cornerRefinementMinAccuracy;
        fs["markerBorderBits"] >> params.markerBorderBits;
        fs["perspectiveRemovePixelPerCell"] >> params.perspectiveRemovePixelPerCell;
        fs["perspectiveRemoveIgnoredMarginPerCell"] >> params.perspectiveRemoveIgnoredMarginPerCell;
        fs["maxErroneousBitsInBorderRate"] >> params.maxErroneousBitsInBorderRate;
        fs["minOtsuStdDev"] >> params.minOtsuStdDev;
        fs["errorCorrectionRate"] >> params.errorCorrectionRate;
        return true;
    }


    // Рисует изображение layer поверх img
    void drawOver(cv::Mat &img, cv::Mat &layer) {
        for(int i = 0; i != layer.rows; ++i) {
            for(int j = 0; j != layer.cols; ++j) {
                cv::Vec3b pixel = layer.at<cv::Vec3b>(i,j);
                if(pixel[0] || pixel[1] || pixel[2]) {
                    img.at<cv::Vec3b>(i,j) = pixel;
                }
            }
        }
    }


    // Рисует куб на рисунке img по точкам points в порядке убывания расстояния
    // центра поверхности от начала системы координат камеры (z_vector)
    void drawCube(cv::Mat &img, 
                  std::vector<cv::Point2f> &points, 
                  std::vector<double> &z_vector) {
        // Вектор, содержащий удаленность центра каждой поверхности куба от
        // начала системы координат камеры по оси Z
        std::vector<double> remote_vector;

        // Мапа, пердоставляющая доступ к шаблону поверхности по ее удаленности
        // по оси Z от начала системы координат камеры
        std::map<double, cv::Mat*> layers_map;

        // Базовая поверхность
        double remote_distance = 0.0;
        cv::Mat base_layer = cv::Mat::zeros(img.rows, img.cols, img.type());
        std::vector<cv::Point2i> base(4);
        for(int i = 0; i != 4; ++i) {
            base[i] = points[i];
            remote_distance += z_vector[i];
        }
        remote_distance /= 4;
        remote_vector.push_back(remote_distance);
        // На пустом cv::Mat рисуем поверхность по точкам
        cv::fillPoly(base_layer, base, cv::Scalar(0,0,255));
        layers_map[remote_distance] = &base_layer;

        // Верхняя поверхность
        remote_distance = 0.0;
        cv::Mat cap_layer = cv::Mat::zeros(img.rows, img.cols, img.type());
        std::vector<cv::Point2i> cap(4);
        for(int i = 0; i != 4; ++i) {
            cap[i] = points[4 + i];
            remote_distance += z_vector[4+i];
        }
        remote_distance /= 4;
        remote_vector.push_back(remote_distance);
        cv::fillPoly(cap_layer, cap, cv::Scalar(0,128,255));
        layers_map[remote_distance] = &cap_layer;

        // Фронтальная поверхность
        cv::Mat front_layer = cv::Mat::zeros(img.rows, img.cols, img.type());
        std::vector<cv::Point2i> front(4);
        front[0] = points[1];
        front[1] = points[2];
        front[2] = points[6];
        front[3] = points[5];
        remote_distance = z_vector[1] + z_vector[2] + z_vector[6] + z_vector[5];

        remote_distance /= 4;
        remote_vector.push_back(remote_distance);
        cv::fillPoly(front_layer, front, cv::Scalar(0,255,255));
        layers_map[remote_distance] = &front_layer;

        // Задняя поверхнсоть
        cv::Mat back_layer = cv::Mat::zeros(img.rows, img.cols, img.type());
        std::vector<cv::Point2i> back(4);
        back[0] = points[0];
        back[1] = points[3];
        back[2] = points[7];
        back[3] = points[4];
        remote_distance = z_vector[0] + z_vector[3] + z_vector[7] + z_vector[4];

        remote_distance /= 4;
        remote_vector.push_back(remote_distance);
        cv::fillPoly(back_layer, back, cv::Scalar(0,255,0));
        layers_map[remote_distance] = &back_layer;

        // Левая поверхность
        cv::Mat left_layer = cv::Mat::zeros(img.rows, img.cols, img.type());
        std::vector<cv::Point2i> left(4);
        left[0] = points[0];
        left[1] = points[1];
        left[2] = points[5];
        left[3] = points[4];
        remote_distance = z_vector[0] + z_vector[1] + z_vector[5] + z_vector[4];

        remote_distance /= 4;
        remote_vector.push_back(remote_distance);
        cv::fillPoly(left_layer, left, cv::Scalar(255,0,255));
        layers_map[remote_distance] = &left_layer;

        // Правая поверхность
        cv::Mat right_layer = cv::Mat::zeros(img.rows, img.cols, img.type());
        std::vector<cv::Point2i> right(4);
        right[0] = points[2];
        right[1] = points[6];
        right[2] = points[7];
        right[3] = points[3];
        remote_distance = z_vector[2] + z_vector[6] + z_vector[7] + z_vector[3];

        remote_distance /= 4;
        remote_vector.push_back(remote_distance);
        cv::fillPoly(right_layer, right, cv::Scalar(255,0,0));
        layers_map[remote_distance] = &right_layer;

        // Сортируем вектор удаленности по убыванию
        std::sort(remote_vector.begin(), remote_vector.end(), std::greater<>());

        // Добавляем на выходное изображение спроецированные стороны куба
        for(auto &elem: remote_vector) {
            // Непрозрачные стороны
            drawOver(img, *layers_map[elem]);

            // Полупрозрачные стороны
            //cv::addWeighted(img, 1, *layers_map[elem], 1, 0, img);

            //cv::polylines(img, *remote_map[elem], true, cv::Scalar(0,0,0), 3);
        }
    }
    

    // Рисует кубы со стороной markerLength на рисунке img полученном с камеры с параметрами
    // camera_matrix и dist_coeffs на маркере с параметрами rvecs и tvecs
    void drawCubes(cv::Mat &img, 
                   float markerLength, 
                   cv::Mat &camera_matrix, 
                   cv::Mat &dist_coeffs, 
                   std::vector<cv::Vec3d> &rvecs, 
                   std::vector<cv::Vec3d> &tvecs) {
        // Инициализируем точки куба в соответствии с размером маркера
        std::vector<cv::Point3f> points(8);
        points[0] = cv::Point3f(-markerLength / 2, -markerLength / 2, 0);
        points[1] = cv::Point3f(markerLength / 2, -markerLength / 2, 0);
        points[2] = cv::Point3f(markerLength / 2, markerLength / 2, 0);
        points[3] = cv::Point3f(-markerLength / 2, markerLength / 2, 0);
        points[4] = cv::Point3f(-markerLength / 2, -markerLength / 2, markerLength);
        points[5] = cv::Point3f(markerLength / 2, -markerLength / 2, markerLength);
        points[6] = cv::Point3f(markerLength / 2, markerLength / 2, markerLength);
        points[7] = cv::Point3f(-markerLength / 2, markerLength / 2, markerLength);

        // Проход по всем найденным маркерам
        for(int i = 0; i != rvecs.size(); ++i) {
            // Поиск вектора точек, спроецированных на изображение камеры (2D)
            std::vector<cv::Point2f> new_points;
            cv::projectPoints(points, rvecs[i], tvecs[i], camera_matrix, dist_coeffs, new_points);

            // Поиск вектора точек в системе координат камеры (3D)
            // Перевод вектора поворота к виду матрицы 3х3
            cv::Mat rot_mat;
            cv::Rodrigues(rvecs[i], rot_mat);

            // Инициализация матрицы 4х4 для перобразования точек из системы координат
            // маркера в систему координат камеры
            cv::Mat matrica = cv::Mat::zeros(4,4,CV_64F);
            // Элемент поворота
            for(int row = 0; row != 3; ++row) {
                for(int col = 0; col != 3; ++col) {
                    matrica.at<double>(row,col) = rot_mat.at<double>(row,col);
                }
            }
            // Элемент перемещения
            matrica.at<double>(0,3) = tvecs[i][0];
            matrica.at<double>(1,3) = tvecs[i][1];
            matrica.at<double>(2,3) = tvecs[i][2];
            matrica.at<double>(3,3) = 1.0;

            // Вычисление положения точек в системе координат камеры
            // Для поиска наиболее удаленных от камеры точек, нас интересуют
            // только координаты по оси z, поэтому сохраняем только их
            std::vector<double> z_vector;
            for(auto &point: points) {
                cv::Mat orig_mat(4,1,CV_64F);
                orig_mat.at<double>(0,0) = point.x;
                orig_mat.at<double>(0,1) = point.y;
                orig_mat.at<double>(0,2) = point.z;
                orig_mat.at<double>(0,3) = 1;

                cv::Mat new_mat = matrica * orig_mat;
                z_vector.emplace_back(new_mat.at<double>(2,0));
            }

            // Рисуем куб
            drawCube(img, new_points, z_vector);
        }
    }
    

    // По словарю dictionary находит маркеры на изображении frame, полученном с камеры
    // с параметрами camera_matrix и dist_coeffs
    void projectCubesOnMarkers(cv::Mat &frame, 
                               cv::Ptr<cv::aruco::Dictionary> dictionary,
                               cv::Ptr<cv::aruco::DetectorParameters> params,
                               float marker_length,
                               cv::Mat &camera_matrix,
                               cv::Mat &dist_coeffs,
                               cv::Mat &out) {
        // Копируем оригинальный кадр
        out = frame.clone();

        // Переменные под найденные маркеры
        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f>> corners;

        // Поиск маркеров
        cv::aruco::detectMarkers(frame, dictionary, corners, ids, params);
        
        // Если найден хотя бы один маркер
        if(ids.size() > 0) {
            // Поиск веторов поворота и перемещения для перевода координат маркера в координаты камеры
            std::vector<cv::Vec3d> rvecs, tvecs;
            cv::aruco::estimatePoseSingleMarkers(corners, 50, camera_matrix, dist_coeffs, rvecs, tvecs);
            // Рисуем кубы на маркерах
            drawCubes(out, 50, camera_matrix, dist_coeffs, rvecs, tvecs);
        }
    }
}


void CUBE_UTILITY::start(float marker_length,
                         const std::string &calibration_path,
                         const std::string &detector_params_path,
                         const std::string &video_path) {
    // Извлечение данных калибровки камеры
    cv::Mat camera_matrix, dist_coeffs;
    readCameraParameters(calibration_path, camera_matrix, dist_coeffs);

    // Извлечение параметров детектора
    cv::aruco::DetectorParameters par;
    cv::Ptr<cv::aruco::DetectorParameters> params;
    if(readDetectorParameters(detector_params_path, par))
        params = &par;
    else 
        params = cv::aruco::DetectorParameters::create();

    // Создание указателя на словарь
    cv::Ptr<cv::aruco::Dictionary> dictionary = nullptr;

    // Захват видео с камеры / чтение из файла
    cv::VideoCapture video_capture;
    if(video_path == "")
        video_capture.open(0);
    else
        video_capture.open(video_path);


    // Обработка кадров
    while(1) {
        // Вытаскиваем один кадр
        cv::Mat frame;
        video_capture >> frame;
        
        // Если кадр пустой, значит видос закончился
        if(frame.empty()) 
            break;

        // Если не определен словарь - ничего не ищем
        cv::Mat out;
        if(dictionary == nullptr)
            out = frame;
        
        // Если определен, можно искать
        else
            projectCubesOnMarkers(frame, dictionary, params, marker_length, camera_matrix, dist_coeffs, out);

        //Визуализация
        cv::imshow("Video", out);
        char key = (char) cv::waitKey(30);
        if(key == 'e') {
            int dict = CUSTOM_UTILITY::defineDictionary(frame);
            if(dict == 0)
                dictionary = nullptr;
            else 
                dictionary = cv::aruco::Dictionary::get(dict);
        }
        else if(key == 'q' || key == 27) 
            break;
    }
} 