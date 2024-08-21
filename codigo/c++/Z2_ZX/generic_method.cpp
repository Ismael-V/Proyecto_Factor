#include <iostream>
#include <chrono>
#include "source/Z2_guided_decarrier.hpp"
#include "source/Solve_Factor.hpp"

using namespace std;

#define U_TYPE uint64_t

constexpr uint32_t KEY_SIZE = 2048;

int main(int argc, char** argv){

    if(argc == 4){
        //Declaramos una clave pública y un factor
        mpz_t clave_publica, p, max_moves;
        mpz_inits(clave_publica, p, max_moves, NULL);

        //Declaramos el maximo de deacarreos
        uint32_t carrys = std::stoi(argv[2]);

        //Declaramos si comprobaremos todos los deacarreos o solo los que tienen carrys deacarreos
        bool target_carry = false;

        //Si solo vamos a mirar los que tienen carrys deacarreos
        if(std::string(argv[3]) == std::string("true")){

            //Colocamos la variable a true
            target_carry = true;
            std::cout << "Targeting carry\n";
        }

        //Mide el tiempo ahora
        std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> principio = std::chrono::high_resolution_clock::now();

        //Colocamos el valor de la clave
        mpz_set_str(clave_publica, argv[1], 10);

        //Calculamos el numero de bits de la clave
        uint32_t b = mpz_sizeinbase(clave_publica, 2);

        //Iniciamos la base
        mpz_set_ui(max_moves, b);

        //Para cada acarreo habra b^carrys movimientos posibles
        mpz_pow_ui(max_moves, max_moves, carrys);

        //Declaramos un array donde almacenaremos la representacion binaria
        char binary_representation[KEY_SIZE] = {};

        //Colocamos la representación binaria en el array
        mpz_get_str(binary_representation, 2, clave_publica);

        //Declaramos el deacarreador
        G_Decarrier d(binary_representation, carrys, target_carry);
        std::string next_guess;

        uint64_t guesses_made = 0;

        //Indicamos si hemos encontrado solucion
        bool solution_found = false;

        while(d.nextDecarry(next_guess)){

            //Cada 128 guesses indicaremos el progreso
            if(guesses_made % 128 == 0) {
                std::cout << "Tried/Total --- " + std::to_string(guesses_made) + "/";
                mpz_out_str(stdout, 10, max_moves);
                std::cout << "\n";
            }

            //Declaramos un polinomio con esa representación
            Z2_poly<U_TYPE> clave_polinomica(next_guess);

            //Declaramos un vector de factores
            std::vector<Z2_poly<U_TYPE>> factores = {};

            //Aplicamos el algoritmo de Berlekamp para factorizar el polinomio representación
            berlekamp_factorize(clave_polinomica, factores);

            //Ordenamos los factores
            std::sort(factores.begin(), factores.end());

            //Buscamos la clave
            solve_factor_blind(factores, clave_publica, p);

            //Escribimos el resultado de no ser 0
            if(mpz_cmp_ui(p, 0) != 0){
                std::cout << "Clave: " + std::string(argv[1]) + "\n";
                std::cout << "Resultado: ";
                mpz_out_str(stdout, 10, p);
                std::cout << "\n";

                solution_found = true;
                //Salimos del bucle de intentos
                break;
            }
            
            guesses_made++;
        }

        //Si no se ha encontrado solucion lo indicamos
        if(!solution_found) std::cout << "Clave: " + std::string(argv[1]) + "\nResultado: No encontrado\n";
        
        //Mide el tiempo al finalizar todo
        std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> final = std::chrono::high_resolution_clock::now();

        int64_t time_ellapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(final - principio).count();

        std::cout << "\nTime elapsed: " << time_ellapsed << " ns ==> " << time_ellapsed/1000000000.0f << " s\n\n";

        //Liberamos la memoria
        mpz_clears(clave_publica, p, max_moves, NULL);
    }else{

        //Indicamos el uso
        std::cout << "Utilizacion: " << argv[0] << " <clave_publica> <max_deacarreos> <target_carry[true]>\n";
    }

    return 0;
}
