#include <opencv2/highgui.hpp>
#include <opencv2/aruco_detector.hpp>
#include <iostream>


int main() {
    int markersX = 7;
    int markersY = 10;
    int markerLength = 50;
    int markerSeparation = 25;
    int margins = 25;
    int dict = cv::aruco::DICT_5X5_100;
    int borderBits = 1;
    std::string save_path = "../tags/board.png";

    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::Dictionary::get(dict);

    cv::Size imageSize;
    imageSize.width = markersX * (markerLength + markerSeparation) - markerSeparation + 2 * margins;
    imageSize.height = markersY * (markerLength + markerSeparation) - markerSeparation + 2 * margins;

    cv::Ptr<cv::aruco::GridBoard> board = cv::aruco::GridBoard::create(markersX, markersY, float(markerLength),
                                                                       float(markerSeparation), dictionary);
    cv::Mat boardImage;
    board->draw(imageSize, boardImage, margins, borderBits);
    cv::imwrite(save_path, boardImage);

    cv::imshow("board", boardImage);
    cv::waitKey(0);
    return 0;
}