#include "src/teplovisor.hpp"
#include "src/robots.hpp"
#include "src/gk.hpp"

int main() {
    TEPLOVISOR::grayPipelineVideo("../vid/allababah/2.mp4");
    TEPLOVISOR::bgrPipelineDirectory("../img/teplovizor");
    //ROBOTS::pipelineImage("../img/roboti/roi_robotov_1.jpg");
    ROBOTS::pipelineVideo("../vid/roboti/roi.mp4");
    GK::defineGK("../img/gk/gk_tmplt.jpg", "../img/gk/gk.jpg");
    return 0;
}