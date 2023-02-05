#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/aruco/dictionary.hpp>
#include <opencv2/aruco_detector.hpp>
#include "aruco_samples_utility.hpp"


int defineDictionary(cv::Mat& img) {
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


void drawCube(cv::Mat &img, 
              std::vector<cv::Point2f> &points, 
              std::vector<double> &z_vector) {
    
    std::map<double, cv::Mat*> remote_map;
    std::vector<double> remote_vector;

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
    cv::fillPoly(base_layer, base, cv::Scalar(0,0,255));
    remote_map[remote_distance] = &base_layer;
    
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
    remote_map[remote_distance] = &cap_layer;

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
    remote_map[remote_distance] = &front_layer;

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
    remote_map[remote_distance] = &back_layer;

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
    remote_map[remote_distance] = &left_layer;

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
    remote_map[remote_distance] = &right_layer;

    

    std::sort(remote_vector.begin(), remote_vector.end(), std::greater<>());

    for(auto &elem: remote_vector) {
        //drawOver(img, *remote_map[elem]);
        cv::addWeighted(img, 1, *remote_map[elem], 1, 0, img);
        
        //cv::polylines(img, *remote_map[elem], true, cv::Scalar(0,0,0), 3);
    }
}


void drawCubes(cv::Mat &img, 
               float markerLength, 
               cv::Mat &cameraMatrix, 
               cv::Mat &distCoeffs, 
               std::vector<cv::Vec3d> &rvecs, 
               std::vector<cv::Vec3d> &tvecs) {
    std::vector<cv::Point3f> points(8);
    points[0] = cv::Point3f(-markerLength / 2, -markerLength / 2, 0);
    points[1] = cv::Point3f(markerLength / 2, -markerLength / 2, 0);
    points[2] = cv::Point3f(markerLength / 2, markerLength / 2, 0);
    points[3] = cv::Point3f(-markerLength / 2, markerLength / 2, 0);
    points[4] = cv::Point3f(-markerLength / 2, -markerLength / 2, markerLength);
    points[5] = cv::Point3f(markerLength / 2, -markerLength / 2, markerLength);
    points[6] = cv::Point3f(markerLength / 2, markerLength / 2, markerLength);
    points[7] = cv::Point3f(-markerLength / 2, markerLength / 2, markerLength);

    std::vector<cv::Point2f> new_points;
    for(int i = 0; i != rvecs.size(); ++i) {
        cv::projectPoints(points, rvecs[i], tvecs[i], cameraMatrix, distCoeffs, new_points);

        cv::Mat rot_mat;
        cv::Rodrigues(rvecs[i], rot_mat);

        cv::Mat matrica = cv::Mat::zeros(4,4,CV_64F);
        for(int row = 0; row != 3; ++row) {
            for(int col = 0; col != 3; ++col) {
                matrica.at<double>(row,col) = rot_mat.at<double>(row,col);
            }
        }
        matrica.at<double>(0,3) = tvecs[i][0];
        matrica.at<double>(1,3) = tvecs[i][1];
        matrica.at<double>(2,3) = tvecs[i][2];
        matrica.at<double>(3,3) = 1.0;

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

        drawCube(img, new_points, z_vector);
        //cv::polylines(img, base, true, cv::Scalar(0,255,0), 3);
    }
}


int main() {
    cv::Mat cameraMatrix, distCoeffs;
    readCameraParameters("../config/calibration.yml", cameraMatrix, distCoeffs);

    cv::Ptr<cv::aruco::Dictionary> dictionary = nullptr;
    cv::Ptr<cv::aruco::DetectorParameters> params = cv::aruco::DetectorParameters::create();

    cv::VideoCapture video_capture;
    video_capture.open(0);
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
        else {
            out = frame.clone();
            std::vector<int> ids;
            std::vector<std::vector<cv::Point2f>> corners/*,rejected*/;
            std::vector<cv::Vec3d> rvecs, tvecs;
        
            cv::aruco::detectMarkers(frame, dictionary, corners, ids, params/*, rejected*/);
            
            for(auto &corners_: corners) {
                for(int i = 1; i != corners_.size(); ++i) {
                    cv::line(out, corners_[i - 1], corners_[i], cv::Scalar(0,0,255), 2);
                }
                cv::line(out, corners_[corners_.size() - 1], corners_[0], cv::Scalar(0,0,255), 2);
            }

            if(ids.size() > 0) {
                cv::aruco::estimatePoseSingleMarkers(corners, 50, cameraMatrix, distCoeffs, rvecs, tvecs);
                drawCubes(out, 50, cameraMatrix, distCoeffs, rvecs, tvecs);
            }
        }

        //Визуализация
        cv::imshow("Video", out);
        char key = (char) cv::waitKey(30);
        if(key == 'e') {
            int dict = defineDictionary(frame);
            if(dict == 0)
                dictionary = nullptr;
            else 
                dictionary = cv::aruco::Dictionary::get(dict);
        }
        else if(key == 'q' || key == 27) 
            break;
    }
    return 0;
}
