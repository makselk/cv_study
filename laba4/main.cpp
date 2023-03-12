#include "src/pipeline.h"


int main() {
    cv::Mat input = PIPELINE::readImage("../img/billy.jpeg");
    // DFT
    cv::Mat fourier;
    cv::dft(input, fourier);
    // Magnitude
    cv::Mat magnitude = PIPELINE::countMagnitude(fourier);
    // Свапаем квадранты
    cv::Mat swapped = PIPELINE::swapQuadrants(magnitude);
    cv::imshow("billy", swapped);
    cv::waitKey(0);
    return 0;
}
