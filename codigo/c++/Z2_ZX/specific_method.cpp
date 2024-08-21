#include <iostream>
#include <chrono>
#include "source/Z2_guided_decarrier.hpp"
#include "source/Solve_Factor.hpp"

using namespace std;

#define U_TYPE uint64_t

constexpr uint32_t KEY_SIZE = 2048;

int main(int argc, char** argv){

    if(argc == 2){

        //Declaramos una clave pública y un factor
        mpz_t clave_publica, p;
        mpz_inits(clave_publica, p, NULL);
 
        //Mide el tiempo ahora
        std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> principio = std::chrono::high_resolution_clock::now();

        //Colocamos el valor de la clave
        mpz_set_str(clave_publica, argv[1], 10);

        //Declaramos un array donde almacenaremos la representacion binaria
        char binary_representation[KEY_SIZE];

        //Colocamos la representación binaria en el array
        mpz_get_str(binary_representation, 2, clave_publica);

        //Declaramos un polinomio con esa representación
        Z2_poly<U_TYPE> clave_polinomica(binary_representation);

        //Declaramos un vector de factores
        std::vector<Z2_poly<U_TYPE>> factores = {};

        //Aplicamos el algoritmo de Berlekamp para factorizar el polinomio representación
        berlekamp_factorize(clave_polinomica, factores);

        //Ordenamos los factores
        std::sort(factores.begin(), factores.end());

        //Buscamos la clave
        solve_factor_blind(factores, clave_publica, p);

        //Escribimos el resultado
        std::cout << "Clave: " + std::string(argv[1]) + "\n";
        std::cout << "Resultado: ";
        if(mpz_cmp_ui(p, 0) != 0){
            
            mpz_out_str(stdout, 10, p);
            std::cout << "\n";
        }else{
            std::cout << "No encontrado\n";
        }

        //Mide el tiempo al finalizar todo
        std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> final = std::chrono::high_resolution_clock::now();

        int64_t time_ellapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(final - principio).count();

        std::cout << "\nTime elapsed: " << time_ellapsed << " ns ==> " << time_ellapsed/1000000000.0f << " s\n\n";

        //Liberamos la memoria
        mpz_clears(clave_publica, p , NULL);

    }else{
        std::cout << "Utilizacion: " + std::string(argv[0]) + " <clave_publica>\n";
    }
    
    return 0;
}
