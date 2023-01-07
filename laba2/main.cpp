#include "src/filters.hpp"

int main() {
    //FILTERS::boxFilterPart1_3("../img/lena.png", 5);
    //FILTERS::gaussAndBoxComapre4("../img/lena.png", 5);
    //FILTERS::unsharpMask5("../img/lena.png", 5, 2);
    FILTERS::unsharpLaplace7("../img/lena.png", 5, 2);
    return 0;
}