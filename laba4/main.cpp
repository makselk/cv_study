#include "src/pipeline.h"


int main() {
    //PIPELINE::directoryCompareDFT("../img");
    //PIPELINE::startConvolveDFT("../img/billy.jpeg");
    //PIPELINE::startCutSpectrum("../img/billy.jpeg", 0.3);
    //PIPELINE::startCorelateLicencePlates("../plates", "../plates_templates");
    PIPELINE::eyeFourier("../eye_fourier/fourier.jpg", "../eye_fourier/glaz.png");
    return 0;
}
