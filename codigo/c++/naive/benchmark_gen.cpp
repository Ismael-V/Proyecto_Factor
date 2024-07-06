#include<iostream>
#include<string>
#include<stdio.h>
#include<stdlib.h>
#include<inttypes.h>
#include<set>
#include<gmp.h>
using namespace std;

//Pre: True
//Post: Devuelve el peso de Hamming de un numero de tamanyo arbitrario.
uint16_t mpz_peso_hamming(const mpz_t num){
	mp_bitcnt_t peso_hamming = mpz_popcount(num);
	return (uint16_t)peso_hamming;
}

//Pre: True
//Post: Devuelve la posicion del digito a 1 mas significativo de un numero dado interpretado en base binaria o 0 si es 0
mp_bitcnt_t last_one_position(const mpz_t n){
	if(mpz_cmp_ui(n, 0) == 0){
		return 0;
	}
	return (mp_bitcnt_t)mpz_sizeinbase(n, 2);
}

//Pre: True
//Post: Genera un numero primo que ronda el tamanyo de b bits
void randomPrime(mpz_t rop, gmp_randstate_t rand_state, uint16_t b){
    mpz_rrandomb(rop, rand_state, b);
    mpz_nextprime(rop, rop);
}

//Pre: True
//Post: Genera cantidad numero de semiprimos cuyos factores son de tamanyo bits_p y bits_q respectivamente,
//asi como el factor mas pequenyo y su peso de Hamming.
void generateSemiprimes(FILE* salida, uint16_t bits_p, uint16_t bits_q, uint16_t cantidad, uint16_t carry){
    //Generamos p, q, N y state
    mpz_t p, q, N;
    gmp_randstate_t state;

    //Iniciamos p, q, N
    mpz_inits(p, q, N, NULL);

    //Iniciamos un estado aleatorio.
    gmp_randinit_mt(state);

    //Almacenamos en una variable resultado
    int32_t resultado;

    //Mientras que haya semiprimos por generar
    while(cantidad > 0){

        //Generamos dos primos aleatorios de tamanyos bits_p y bits_q respectivamente
        randomPrime(p, state, bits_p);
        randomPrime(q, state, bits_q);

        //Comparamos los primos generados y guardamos el resultado
        resultado = mpz_cmp(p, q);

        //Si p y q son distintos
        if(resultado != 0){
            //Los multiplicamos
            mpz_mul(N, p, q);

            //Si cumple la cuasi-igualdad de los pesos entonces lo guardamos.
            if(mpz_peso_hamming(N) == mpz_peso_hamming(p)*mpz_peso_hamming(q) - carry){
                //Escribimos el semiprimo resultante en el fichero
                mpz_out_str(salida, 10, N);
                fprintf(salida, " p = ");
                
                //Si p < q
                if(resultado < 0){
                    //Escribimos en el fichero el factor p y su peso
                    mpz_out_str(salida, 10, p);
                    fprintf(salida, ", q = ");
                    mpz_out_str(salida, 10, q);
                    fprintf(salida, " %i\n", mpz_peso_hamming(p));
                
                //Si q > p
                }else{
                    //Escribimos en el fichero el factor q y su peso
                    mpz_out_str(salida, 10, q);
                    fprintf(salida, ", q = ");
                    mpz_out_str(salida, 10, p);
                    fprintf(salida, " %i\n", mpz_peso_hamming(q));
                }

                //Como lo anyadimos restamos a cantidad 1
                cantidad--;
            }
        }
    }

    //Una vez finalizado, liberamos la memoria
    mpz_clears(p, q, N, NULL);
    gmp_randclear(state);
}

int main(int argc, char** argv){

    uint16_t bits_p;
    uint16_t bits_q;
    uint16_t cantidad;
    uint16_t carry;
    
    if(argc != 6){
        cout << "benchmark_gen <fichero.txt> <bits_p> <bits_q> <n_carry> <cantidad>\n";
        return 1;
    }

    bits_p = atoi(argv[2]);
    bits_q = atoi(argv[3]);
    carry = atoi(argv[4]);
    cantidad = atoi(argv[5]);

    FILE* descSalida = fopen(argv[1], "w");
    if(descSalida != 0){
        cout << "Generando bechmarks . . .\n";
        generateSemiprimes(descSalida, bits_p, bits_q, cantidad, carry);
        fclose(descSalida);
        cout << "Benchmarks generados, para eliminar repetidos usar el comando\nsort <fichero.txt> | uniq | head -n <numero_pruebas> > <benchmark.txt>\n";
    }else if(descSalida != NULL){
        fclose(descSalida);
    }
    
    return 0;    
}