#include <iostream>
#include <chrono>
#include <gmp.h>
#include "Z2_linear_solver.hpp"
#include "Z2_decarrier.hpp"
#include "Z2_guided_decarrier.hpp"
using namespace std;

#define U_TYPE uint64_t

constexpr uint32_t KEY_SIZE = 2048;

std::string claves[] = {
    "803469022129496566413832787617400811301815554554649653438361",
    "6455624695367578199454821262392757575390333697927097565235108436845886183891812477262923580543007926644351019495797658417",
    "2593447234415158150880561759103712937719035952138904312295269762787651828920346118692012314486480674433331797011170307454308621532932644248696937123376305647601073835281257670796849",
    "853505847408621347174758406861792240861042398140499291717193475474824769393417320362647557977823694603531621600606151332030034668234021157582824004509785207324386394493413822591403231216715524473407168578525428020258331483383953419137912635373",
    "89884656743115795386465259539451236680898848947115328636715040578866337902750481566354238661203768010560056939935696678829394884407208311246423945664124328274421597392891154851259652058805873851245969775641083292269319271006402181322880012243214961934234908953700164107933415187770613461286001336894523179153",
    "89884656743115795386465259539451236680898848947238333669346023329529379793821935691190576553606718228943622646692787649533396472437575100331643021452356799270624548666821618684530456538819237381161921526926730215518397251842884136508158600364010339632773140265833335871757206095545873422627664335643611047869",
    "359538626972463181545937173441671075415267175255535079644558347735467119057617110613994049314107928149804605744470838482720720128924948666126951368045435618886973105091364723032067678843703378594563440148856361548681362392739186663226884957937817394230632924905285484312704871823549413298764914802394260700157"
};

uint32_t grados_target[] = {99, 200, 300, 403, 511, 511, 512};

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

static inline
void test_equality_attack(mpz_t &clave_publica, mpz_t &p){
    //Para todas las claves definidas
    for(uint32_t i = 0; i < 7; i++){

        //Colocamos el valor de la clave
        mpz_set_str(clave_publica, claves[i].c_str(), 10);

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
        std::cout << "Resultado: ";
        mpz_out_str(stdout, 10, p);
        std::cout << std::endl;
    }
}

static inline
void test_n_carry_attack(mpz_t &clave_publica, mpz_t &p, std::string key, uint32_t carrys, uint32_t bin_size){
    //Colocamos el valor de la clave
    mpz_set_str(clave_publica, key.c_str(), 10);

    //Declaramos un array donde almacenaremos la representacion binaria
    char binary_representation[KEY_SIZE] = {};

    //Colocamos la representación binaria en el array
    mpz_get_str(binary_representation, 2, clave_publica);

    std::cout << "Representacion: " << binary_representation << std::endl;

    G_Decarrier d(binary_representation, carrys, true);
    std::string next_guess;

    uint32_t guesses_made = 0;

    while(d.nextDecarry(next_guess)){

        if(guesses_made % 128 == 0) std::cout << guesses_made << " / " << bin_size*bin_size << std::endl;
      
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
            std::cout << "Resultado: ";
            mpz_out_str(stdout, 10, p);
            std::cout << std::endl;

            std::cout << next_guess << std::endl;

            //Salimos del bucle de intentos
            break;
        }
        
        guesses_made++;
    }
}

int main(){

    //Declaramos una clave pública y un factor
    mpz_t clave_publica, p;
    mpz_inits(clave_publica, p, NULL);


    //Mide el tiempo ahora
    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> principio = std::chrono::high_resolution_clock::now();

    //Realizamos un test con primos que cumplen E(pq) = E(p)E(q)
    test_equality_attack(clave_publica, p);

    //Realizamos un test con primos que cumplen E(pq) = E(p)E(q) - c
    test_n_carry_attack(clave_publica, p, "8101977715751532059", 2, 64);

    //Mide el tiempo al finalizar todo
    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> final = std::chrono::high_resolution_clock::now();

    std::chrono::nanoseconds time_ellapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(final - principio);

    std::cout << "\nTime elapsed: " << time_ellapsed.count() << " ns ==> " << (std::chrono::duration_cast<std::chrono::seconds>(time_ellapsed)).count() << " s\n\n";

    Profiler();

    //Liberamos la memoria
    mpz_clears(clave_publica, p , NULL);
    return 0;
}
