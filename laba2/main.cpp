#include "src/filters.hpp"

int main() {
    //FILTERS::boxFilterPart1_3("../img/lena.png", 5);
    //FILTERS::gaussAndBoxComapre4("../img/billy.jpeg", 5);
    //FILTERS::unsharpMask5("../img/billy.jpeg", 9, 2);
    FILTERS::laplace6("../img/billy.jpeg");
    return 0;
}