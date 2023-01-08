#include "src/teplovisor.hpp"
#include "src/robots.hpp"
#include "src/gk.hpp"

int main() {
    //teplovisor_gray_largest_pipeline("../vid/allababah/1.mp4");
    //teplovisor_bgr_pipeline("../img/teplovizor/5.jpg");
    //ROBOTS::pipelineImage("../img/roboti/roi_robotov_1.jpg");
    //ROBOTS::pipelineVideo("../vid/roboti/roi.mp4");
    GK::defineGK("../img/gk/gk_tmplt.jpg", "../img/gk/gk.jpg");
    return 0;
}