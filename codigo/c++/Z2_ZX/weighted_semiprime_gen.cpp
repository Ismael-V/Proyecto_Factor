#include <fstream>
#include <iostream>
#include <random>
#include <vector>
#include <set>

constexpr uint64_t max_attemps = (uint64_t)1 << 26;

//Pre: True
//Post: Devuelve el peso de hamming del numero natural n
static inline
uint32_t hamming_weight(uint64_t n){
    //Este sera el peso
    uint32_t peso = 0;

    //Haz
    do{

        //Si el primer bit esta a 1 suma 1 al peso
        if(n & 1) peso++;

    //Mientras que al dividir por 2 de forma entera no se haga 0
    }while(n >>= 1);

    //Devolvemos el peso
    return peso;
}

//Pre: True
//Post: Devuelve si p es primo o no
static inline
bool is_prime(uint64_t p){
    
    //Si es divisible por 2 devuelve 0
    if(p % 2 == 0) return false;

    //Declaramos la raiz cuadrada del numero
    uint64_t p_sqrt = std::sqrt(p);

    //Para todo i menor o igual a la raiz cuadrada
    for(uint64_t i = 3; i <= p_sqrt; i += 2){

        //Si alguno divide a p, no es primo
        if(p % i == 0) return false;
    }

    //Sino p es primo
    return true;
}

//Pre: 0 < num_bits <= 64
//Post: Genera una lista de items elementos primos impares de num_bits, bits
std::vector<uint64_t> generate_random_ui_list(uint32_t items, uint8_t num_bits){

    //Declaramos un dispositivo aleatorio de verdad
    std::random_device real_random;

    //Declaramos un mersenne_twister de 64 bits
    std::mt19937_64 mersenne_twister;

    //Le metemos nuestra semilla de un generador real
    mersenne_twister.seed(real_random());

    //Declaramos una distribucion uniforme entre 2^(k-1) y 2^(k) - 1
    std::uniform_int_distribution<uint64_t> unsigned_int_pool((uint64_t)1 << (num_bits - 2), ((uint64_t)1 << (num_bits - 1)) - 1);

    //Declaramos un vector y alojamos espacio para los elementos
    std::vector<uint64_t> list;
    list.reserve(items);

    //Generamos tantos elementos como items halla en el rango de 2^(k) + 1 y 2^(k + 1) - 1, numeros impares primos de k bits
    for(uint32_t i = 0; i < items; ){
        uint64_t num = (unsigned_int_pool(mersenne_twister) << 1) + 1;
        if(is_prime(num)){
            list.push_back(num);
            i++;
        }
    }

    //Devolvemos la lista de elementos
    return list;
}

int main(int argc, char** argv){
    
    if(argc == 4){

        //Sacamos el numero de bits
        uint8_t num_bits = std::stoi(argv[1]);
        
        //Si excede los 32 bits o no supera los 4 bits alertamos al usuario
        if(num_bits > 32 && num_bits < 4){
            std::cout << "El numero de bits debe ser como poco 4 y como mucho 32\n";
            return 1;
        }

        //Declaramos la cantidad
        uint32_t cantidad = std::stoi(argv[2]);

        //Declaramos el numero de acarreos de diferencia
        uint32_t c = std::stoi(argv[3]);

        //Si c es 0 advertimos de los numeros generados
        if(c == 0) std::cout << "Advertencia!! ---> Generando numeros con: (" + std::to_string(num_bits - 1) + ", " + std::to_string(num_bits) + ") bits para forzar equidad\n";

        //Esto cuenta los semiprimos generados
        uint32_t semiprimos_generados = 0;

        //Esto cuenta el numero de intentos totales
        uint32_t attemps = 0;

        //Este es el conjunto de primos repetidos
        std::set<uint64_t> repeated_primes;

        //Mientras que queden semiprimos por generar
        while(semiprimos_generados < cantidad){

            std::vector<uint64_t> p_list;
            std::vector<uint64_t> q_list;

            //Si la c es mayor a 0 generaremos las listas de forma normal.
            if(c > 0){
                p_list = generate_random_ui_list(cantidad, num_bits);
                q_list = generate_random_ui_list(cantidad, num_bits);

            //Sino generaremos la primera lista con un bit de menos
            }else{
                p_list = generate_random_ui_list(cantidad, num_bits - 1);
                q_list = generate_random_ui_list(cantidad, num_bits);
            }

            for(uint32_t i = 0; i < cantidad && semiprimos_generados < cantidad; i++, attemps++){
                uint64_t key = p_list[i]*q_list[i];

                //Si se da la igualdad de pesos buscada, no se forma un cuadrado perfecto y no hemos repetido factores
                if( p_list[i] != q_list[i] &&
                    hamming_weight(key) == (hamming_weight(p_list[i])*hamming_weight(q_list[i]) - c) &&
                    !repeated_primes.contains(p_list[i]) &&
                    !repeated_primes.contains(q_list[i]))
                {

                    //Metemos los primos para no repetirlos
                    repeated_primes.insert(p_list[i]);
                    repeated_primes.insert(q_list[i]);

                    //Colocamos los factores en orden de menor a mayor
                    if(p_list[i] < q_list[i]){
                        std::cout << std::to_string(key) + " = " + std::to_string(p_list[i]) + " x " + std::to_string(q_list[i]) + "\n";
                    }else{
                        std::cout << std::to_string(key) + " = " + std::to_string(q_list[i]) + " x " + std::to_string(p_list[i]) + "\n";
                    }
                    
                    //Anyadimos 1 a los semiprimos generados
                    semiprimos_generados++;
                }
            }

            if(attemps > max_attemps){
                std::cout << "Se ha excedido el numero de intentos, abortando...\n";
                return 1;
            }
        }



    }else{
        std::cout << std::string(argv[0]) + " <num_bits_p_q> <cantidad> <c>\n";
        return 1;
    }

    return 0;
}