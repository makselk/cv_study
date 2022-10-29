#include "src/telega.hpp"

int main() {
    Telega telega("../img/bkg.png", "../img/model.png");
    telega.run(300, 80, 150);
    return 0;
}