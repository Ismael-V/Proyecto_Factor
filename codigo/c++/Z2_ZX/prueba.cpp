#include <iostream>
#include "Z2_polynomial.h"
using namespace std;

int main(){

    Z2_poly<uint64_t> poly_x("100010101000000010100000001");
    Z2_poly<uint64_t> poly_y("100010101000000010100000001");

    log_message("Hola");

    //std::cout << (poly_x % poly_y).to_string() << std::endl;

    //std::cout << poly_y.to_binary_representation() << std::endl;
    //std::cout << "(" + poly_y.to_string() + ") >= (" + poly_x.to_string() + ") = " << (poly_y >= poly_x) << std::endl;

    std::cout << poly_x.to_string() << std::endl;
    std::cout << (poly_x + poly_y).to_string() << std::endl;

    return 0;
}