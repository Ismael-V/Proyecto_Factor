#include "Solve_Factor.hpp"

//Pre: True
//Post: Devuelve, de proveerle del grado target adecuado uno de los factores del numero, de encontrarlo
void solve_factor(std::vector<Z2_poly<U_TYPE>>& factores, uint32_t grado_target, const mpz_t N, mpz_t& factor){

    Profiler ("Search Factor");

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

        //Si quedan aun factores
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

                //Si el grado de nuestra guess es el grado target
                if(poly_guesses[anidamiento + 1].poli_grado() == grado_target){

                    //Nuestra guess sera el polinomio convertido a numero
                    mpz_set_str(num_guess, poly_guesses[anidamiento + 1].to_binary_representation().c_str(), 2);

                    //std::cout << "Number guessed: ";
                    //mpz_out_str(stdout, 10, num_guess);
                    //std::cout << std::endl;

                    //Si divide a N
                    if(mpz_divisible_p(N, num_guess) != 0){

                        //Escribimos el resultado
                        mpz_set(factor, num_guess);

                        //Liberamos memoria
                        delete[] prev_factor;
                        delete[] poly_guesses;
                        delete[] factor_apuntado;
                        mpz_clear(num_guess);

                        //Volvemos
                        return;

                    //Sino, bajamos un nivel de anidamiento
                    }else{
                        anidamiento--;
                    }
                
                //Si la guess es muy grande
                }else if(poly_guesses[anidamiento + 1].poli_grado() > grado_target){

                    //Bajamos un nivel de anidamiento
                    anidamiento--;

                //Si la guess es pequeña
                }else{

                    //Pasamos nuestro factor apuntado
                    factor_apuntado[anidamiento + 1] = factor_apuntado[anidamiento];

                    //Subimos el nivel de anidamiento
                    anidamiento++;
                }
            }else{

                //Avanzamos el factor apuntado
                ++factor_apuntado[anidamiento];
            }
        }else{

            //Bajamos un nivel de anidamiento
            anidamiento--;

        }
    }

    //Devolvemos un 0 de no encontrar nada
    mpz_set_ui(factor, 0);

    //Liberamos memoria
    delete[] prev_factor;
    delete[] poly_guesses;
    delete[] factor_apuntado;
    mpz_clear(num_guess);

    //Volvemos
    return;

}


//Pre: True
//Post: Devuelve uno de los factores del numero, de encontrarlo
void solve_factor_blind(std::vector<Z2_poly<U_TYPE>>& factores, const mpz_t N, mpz_t& factor){

    Profiler ("Search Factor");

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

                    //Escribimos el resultado
                    mpz_set(factor, num_guess);

                    //Liberamos memoria
                    delete[] prev_factor;
                    delete[] poly_guesses;
                    delete[] factor_apuntado;

                    mpz_clear(num_guess);

                    //Volvemos
                    return;

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

    //Devolvemos un 0 de no encontrar nada
    mpz_set_ui(factor, 0);

    //Liberamos memoria
    delete[] prev_factor;
    delete[] poly_guesses;
    delete[] factor_apuntado;

    mpz_clear(num_guess);

    //Volvemos
    return;

}