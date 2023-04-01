#include "src/pipeline.h"


int main() {
    PIPELINE::directoryCompareDFT("../img");
    //PIPELINE::startConvolveDFT("../img/billy.jpeg");
    //PIPELINE::startCutSpectrum("../img/billy.jpeg", 0.3);
    //PIPELINE::startCorelateLicencePlates("../plates", "../plates_templates");
    return 0;
}


// There are 2 pictures. The first is licence plate. The second is a letter, cut from the first image. Create a function to find the letter on the image of licence plate, using cv::mulSpectrums