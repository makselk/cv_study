#include "src/telega.hpp"
#include "src/tank.hpp"

int main() {
    //Telega telega("../img/bkg.png", "../img/model.png");
    //telega.run(300, 80, 150);

    Tank tank("../img/back_city.jpg", "../img/tank.png");
    tank.run();
    return 0;
}