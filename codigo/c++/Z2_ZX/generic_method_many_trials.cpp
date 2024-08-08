#include <iostream>
#include <fstream>
#include <chrono>
#include "source/Z2_guided_decarrier.hpp"
#include "source/Solve_Factor.hpp"

constexpr bool TRY_MERSSENE_CONST = true;

int main(int argc, char** argv){

    //Si el numero de argumentos es 3
    if(argc == 2){

        //Inicializamos el numero a factorizar, la clave, el factor que buscamos, el catalizador, la base para adulterar y un numero auxiliar
        mpz_t N, key, p, catalyst, base, aux, zero;
        mpz_inits(N, key, p, catalyst, base, aux, zero, NULL);

        //Metemos la clave en base 10
        mpz_set_str(key, argv[1], 10);

        //Iniciamos el 0
        mpz_set_ui(zero, 0);

        //Inicializamos la base y el catalizador
        mpz_set_ui(catalyst, 1);
        mpz_set_ui(base, 3);



        //Si intentamos con una constante de Mersenne
        if(TRY_MERSSENE_CONST){

            //Calculamos el numero de digitos aproximado de uno de sus factores
            uint32_t digits = (mpz_sizeinbase(key, 2)/2);

            //Colocamos a 0, luego una potencia de 2 y le restamos 1
            mpz_set_ui(catalyst, 0);
            mpz_setbit(catalyst, digits);
            mpz_sub_ui(catalyst, catalyst, 1);

        }

        //Declaramos una variable para indicar si encontramos solucion
        bool solution_found = false;

        //Indicamos el numero de factores que se han empleado
        uint32_t elements = 0;

        //Colocamos la clave
        mpz_set(N, key);

        char reagent[UINT16_MAX];

        //Mientras que no encontremos solucion
        while(!solution_found){

            //Sacamos la representacion binaria de la clave con catalizador
            if(mpz_sizeinbase(N, 2) + 1 > UINT16_MAX){
                std::cout << "Error, memoria excedida\n";
                return 1;
            }

            mpz_get_str(reagent, 2, N);

            //Lo representamos con una cadena
            std::string next_guess(reagent);

            //Declaramos un polinomio con esa representación
            Z2_poly<U_TYPE> clave_polinomica(next_guess);

            //Declaramos un vector de factores
            std::vector<Z2_poly<U_TYPE>> factores = {};

            //Aplicamos el algoritmo de Berlekamp para factorizar el polinomio representación
            berlekamp_factorize(clave_polinomica, factores);

            //Ordenamos los factores
            std::sort(factores.begin(), factores.end());

            //Buscamos la clave
            
            mpz_t num_guess;
            mpz_init(num_guess);

            uint16_t num_factores = factores.size();

            Z2_poly<U_TYPE>* prev_factor = Z2_poly<U_TYPE>::z2polyArray(num_factores, "1");
            Z2_poly<U_TYPE>* poly_guesses = Z2_poly<U_TYPE>::z2polyArray(num_factores + 1, "1");
            std::vector<Z2_poly<U_TYPE>>::const_iterator* factor_apuntado = new std::vector<Z2_poly<U_TYPE>>::const_iterator[num_factores + 1];
            int16_t anidamiento = 0;

            factor_apuntado[anidamiento] = factores.begin();

            //uint32_t iteraciones = 0;
            //uint32_t max_iteraciones = num_factores*num_factores;

            while(anidamiento >= 0){

                //Si quedan aun factores y el anidamiento es el adecuado
                if(anidamiento < num_factores && factor_apuntado[anidamiento] != factores.end()){

                    //Si el factor previo es distinto al apuntado
                    if(prev_factor[anidamiento] != *factor_apuntado[anidamiento]){

                        //Ahora el factor previo sera el apuntado
                        prev_factor[anidamiento] = *factor_apuntado[anidamiento];

                        //Nuestra guess se construye multiplicando por el factor ahora apuntado por la anterior guess
                        poly_guesses[anidamiento + 1] = poly_guesses[anidamiento]*(*factor_apuntado[anidamiento]);

                        //std::cout << "Guess: " << poly_guesses[anidamiento].to_string() << " | Anidamiento = " << (uint32_t)anidamiento << std::endl;
                        //std::cout << "Grado guess: " << poly_guesses[anidamiento + 1].poli_grado() << " | Grado target: " << grado_target <<" | Anidamiento = " << (uint32_t)anidamiento << std::endl;

                        //Avanzamos el factor apuntado
                        ++factor_apuntado[anidamiento];

                        //Nuestra guess sera el polinomio convertido a numero
                        mpz_set_str(num_guess, poly_guesses[anidamiento + 1].to_binary_representation().c_str(), 2);

                        //std::cout << "Number guessed: ";
                        //mpz_out_str(stdout, 10, num_guess);
                        //std::cout << std::endl;

                        //Si divide a N y no es N
                        if(mpz_divisible_p(N, num_guess) != 0 && mpz_cmp(N, num_guess) != 0){

                            //Calculamos el maximo comun divisor con la clave
                            mpz_gcd(p, key, num_guess);

                            //Si son distintos y no es 1, hemos encontrado un factor
                            if(mpz_cmp_ui(p, 1) != 0 && mpz_cmp(key, p) != 0){

                                //Indicamos que hemos encontrado solucion
                                solution_found = true;

                            }else{
                                std::cout << "Posibilidad encontrada ha fallado\n";
                            }
                        }
                    
                        //Pasamos nuestro factor apuntado
                        factor_apuntado[anidamiento + 1] = factor_apuntado[anidamiento];

                        //Subimos el nivel de anidamiento
                        anidamiento++;
                    }else{

                        //Avanzamos el factor apuntado
                        ++factor_apuntado[anidamiento];
                    }
                }else{

                    //Bajamos un nivel de anidamiento
                    anidamiento--;

                }
            }

            //Liberamos memoria
            delete[] prev_factor;
            delete[] poly_guesses;
            delete[] factor_apuntado;

            mpz_clear(num_guess);

            //Si no hemos encontrado solucion
            if(!solution_found){
                
                //Anyadimos 2 al catalizador
                mpz_add_ui(base, base, 2);

                //Lo multiplicamos al catalizador
                mpz_mul(aux, catalyst, base);

                //Calculamos la nueva clave
                mpz_mul(N, key, aux);

            }

        }

        std::cout << "Clave: ";
        mpz_out_str(stdout, 10, key);
        std::cout << "\nFactor: ";
        mpz_out_str(stdout, 10, p);
        std::cout << "\n";

        mpz_clears(N, key, p, catalyst, base, aux, zero, NULL);
    }
}