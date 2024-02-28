#include <iostream>
#include<gmp.h>
#include "Z2_linear_solver.hpp"
using namespace std;

#define CLAVE_PUBLICA "803469022129496566413832787617400811301815554554649653438361"

#define U_TYPE uint64_t

int main(){

    mpz_t clave_publica;
    mpz_init(clave_publica);
    mpz_set_str(clave_publica, CLAVE_PUBLICA, 10);

    char binary_representation[1024];

    mpz_get_str(binary_representation, 2, clave_publica);

    Z2_poly<U_TYPE> clave_polinomica(binary_representation);

    std::vector<Z2_poly<U_TYPE>> factores = {};

    berlekamp_factorize(clave_polinomica, factores);

    std::sort(factores.begin(), factores.end());

    Z2_poly<U_TYPE> resultado("1");

    for(Z2_poly<U_TYPE> factor : factores){
        std::cout << factor.to_string() << std::endl;
        resultado = resultado * factor;
    }

    std::cout << "Original  : " << binary_representation << std::endl;
    std::cout << "Calculado : " << resultado.to_binary_representation() << std::endl;

    std::cout << "El polinomio factorizado es: " << resultado.to_string() << std::endl;

    return 0;
}