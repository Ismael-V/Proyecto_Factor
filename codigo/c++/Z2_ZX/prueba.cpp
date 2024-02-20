#include <iostream>
#include "Z2_linear_solver.hpp"
using namespace std;

int main(){

    Z2_poly<uint8_t> poly_x("100010101000000010100000001");
    Z2_poly<uint8_t> poly_y("100010101000000010100000001");

    //std::cout << (poly_x % poly_y).to_string() << std::endl;

    //std::cout << poly_y.to_binary_representation() << std::endl;
    //std::cout << "(" + poly_y.to_string() + ") >= (" + poly_x.to_string() + ") = " << (poly_y >= poly_x) << std::endl;

    Z2_Linear_Solver<uint8_t>(Z2_poly<uint8_t>("110001")).solve_linear();

    //std::cout << poly_x.to_string() << std::endl;
    //std::cout << (poly_x + poly_y).to_string() << std::endl;

    return 0;
}