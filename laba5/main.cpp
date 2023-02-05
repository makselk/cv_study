#include "src/cube_utility.h"


int main() {
    CUBE_UTILITY::start(50,
                        "../config/calibration.yml",
                        "config/detector_params.yml");
    return 0;
}
