#include<iostream>
#include<fstream>
#include<string>
#include<chrono>
#include<inttypes.h>
#include<gmp.h>
using namespace std;

#define NUM_INDEXS 1024
#define DATA_SET 1025

//Esta es la definicion de la pila de indices de forma estatica
mp_bitcnt_t vector_de_indices[NUM_INDEXS];
uint16_t siguiente_indice = 0;

//Aqui iran los datos del dataset para realizar los tests
struct datos{
    string clave_publica;
    string factor_p;
    uint16_t peso;
} semiprimos[DATA_SET];

//Numero de datos que contiene el dataset
uint16_t numero_datos = 0;

//Relojes para medir tiempos
chrono::high_resolution_clock reloj;
chrono::time_point<chrono::high_resolution_clock>	invocacion, finalizacion, inicio_divisibilidad, 
													fin_divisibilidad, inicio_exploracion, fin_exploracion;
chrono::microseconds tiempo_factorizado = chrono::duration<int64_t>::zero();
chrono::microseconds tiempo_divisibilidad = chrono::duration<int64_t>::zero();
chrono::microseconds tiempo_exploracion = chrono::duration<int64_t>::zero();

//Pre: True
//Post: Devuelve el peso de Hamming de un numero de tamanyo arbitrario.
#pragma inline
uint16_t mpz_peso_hamming(const mpz_t num){
	mp_bitcnt_t peso_hamming = mpz_popcount(num);
	return (uint16_t)peso_hamming;
}

//Pre: True
//Post: Devuelve la posicion del digito a 1 mas significativo de un numero dado interpretado en base binaria o 0 si es 0
#pragma inline
mp_bitcnt_t last_one_position(const mpz_t n){
	if(mpz_cmp_ui(n, 0) == 0){
		return 0;
	}
	return (mp_bitcnt_t)mpz_sizeinbase(n, 2);
}

void mpz_factoriza_iterativo(mpz_t rop, const mpz_t N, const mpz_t M, uint16_t peso){
	//La solucion con la que trabajaremos
	mpz_t solucion_fabricada;
	mpz_init(solucion_fabricada);

	//El indice
	mp_bitcnt_t index = 0;

	//La condicion de terminacion
	bool seguir = true;

    //Borramos el vector de indices
	siguiente_indice = 0;

	tiempo_divisibilidad = chrono::duration<int64_t>::zero();
	tiempo_exploracion = chrono::duration<int64_t>::zero();

	//<---- Inicio de exploracion
	inicio_exploracion = reloj.now();

	while(seguir){
		
		//Colocamos el siguiente 1 en la solucion fabricada
		mpz_setbit(solucion_fabricada, index);

		//Lo contabilizamos en el peso
		peso--;

		//Si la solucion fabricada es impar
		if(mpz_tstbit (solucion_fabricada, 0) == 1){

			//Si la solucion fabricada no excede el limite
			if(mpz_cmp(solucion_fabricada, M) <= 0){

				//Si hemos colocado lo suficiente
				if(peso == 0){

					//<---- Fin de exploracion
					fin_exploracion = reloj.now();
					tiempo_exploracion += chrono::duration_cast<chrono::microseconds>(fin_exploracion - inicio_exploracion);
                    
					//<---- Inicio de divisibilidad
					inicio_divisibilidad = reloj.now();

					//Mientras que no excedamos el limite donde buscar
					while(mpz_cmp(solucion_fabricada, M) <= 0){
						
						/*
							mpz_out_str(stdout, 2, solucion_fabricada);
							cout << endl;
						*/
						
						//Si la solucion fabricada divide a N la devolvemos
						if(mpz_divisible_p (N, solucion_fabricada)){
							mpz_set(rop, solucion_fabricada);
							return;
						}

						//Si no probamos con el siguiente indice

						//Quitamos el 1 de esa posicion
						mpz_clrbit(solucion_fabricada, index);
						
						//Sumamos una posicion
						index = index + 1;
						
						//Ponemos el 1 en la nueva posicion
						mpz_setbit(solucion_fabricada, index);
					}

					//<---- Fin de divisibilidad
					fin_divisibilidad = reloj.now();
					tiempo_divisibilidad += chrono::duration_cast<chrono::microseconds>(fin_divisibilidad - inicio_divisibilidad);

					//Quitamos el 1 de esa posicion
					mpz_clrbit(solucion_fabricada, index);

					//Si el vector de indices no se ha vaciado por completo
					if(siguiente_indice != 0){

						//Sacamos el indice anterior
                        siguiente_indice--;
                        index = vector_de_indices[siguiente_indice];

						//Contabilizamos la salida de ambas dos
						peso += 2;

						//Quitamos el 1 de esa posicion
						mpz_clrbit(solucion_fabricada, index);

						//Avanzamos el indice
						index++;

					}else{

						//*** Salimos del bucle de busqueda ***
						seguir = false;

					}

					//<---- Fin de exploracion (Usando el timestamp de divisibilidad)
					fin_exploracion = reloj.now();
					tiempo_exploracion += chrono::duration_cast<chrono::microseconds>(fin_exploracion - fin_divisibilidad);

					//<---- Inicio de exploracion
					inicio_exploracion = reloj.now();

				//Si no se ha colocado lo suficiente entonces colocamos y volvemos a avanza 1
				}else{
					//Colocamos el indice
                    vector_de_indices[siguiente_indice] = index;
                    siguiente_indice++;

					//Avanzamos el indice en 1
					index++;

				}

			//Si excedemos el limite en cambio
			}else{
				//Quitamos el 1 de esa posicion
				mpz_clrbit(solucion_fabricada, index);

				//Si el vector de indices no se ha vaciado por completo
				if(siguiente_indice != 0){

					//Sacamos el indice anterior
                    siguiente_indice--;
                    index = vector_de_indices[siguiente_indice];

					//Contabilizamos la salida de ambas dos
					peso += 2;

					//Quitamos el 1 de esa posicion
					mpz_clrbit(solucion_fabricada, index);

					//Avanzamos el indice
					index++;

				}else{

					//*** Salimos del bucle de busqueda ***
					seguir = false;

				}
			}

		//Si la solucion fabricada es par, para de buscar
		}else{

			seguir = false;

		}
	}

	//<---- Fin de exploracion
	fin_exploracion = reloj.now();
	tiempo_exploracion += chrono::duration_cast<chrono::microseconds>(fin_exploracion - inicio_exploracion);
}

bool leer_datos(istream& entrada){
    string peso;

    while(!entrada.eof()){
        getline(entrada, semiprimos[numero_datos].clave_publica,' ');
        getline(entrada, semiprimos[numero_datos].factor_p,' ');
        getline(entrada, peso);

        semiprimos[numero_datos].peso = stoi(peso);

        numero_datos++;
    }

    return true;
}

#define BENCHMARKS "semiprimes_64.txt"
#define RESULTADOS "semiprimes_results_64.csv"

int main(){

	//Inicializamos el numero a factorizar y el limite de factorizacion
	mpz_t N, M, p, q, s;
	mpz_inits(N, M, p, q, s, NULL);

    bool encontrado = false;
   
    //Leemos el fichero con los Benchmarks
    ifstream entrada (BENCHMARKS);
    if(entrada.is_open()){
        leer_datos(entrada);
        entrada.close();
    }

	//Escribimos los resultados en un csv
	ofstream salida (RESULTADOS);
	if(salida.is_open()){

		salida << "Numero,Estado,Tiempo de Ejecución (us),Tiempo de Exploración (us),Tiempo de Divisibilidad (us)\n";
		
		//Para cada elemento del benchmark
		for(uint16_t i = 0; i < numero_datos; i++){
			//Metemos la clave publica en N
			mpz_set_str(N, semiprimos[i].clave_publica.c_str(), 10);
			
			//Metemos la raiz cuadrada de la clave publica en M
			mpz_sqrt(M, N);
			
			cout << "Se va ha factorizar el numero: ";
			mpz_out_str(stdout, 10, N);
			cout << "\n";
			
			cout << "\nProbando para peso: " << semiprimos[i].peso << endl;

			//Buscamos en ese subconjunto el primo que factorice nuestra clave
			invocacion = reloj.now();
			mpz_factoriza_iterativo(p, N, M, semiprimos[i].peso);
			finalizacion = reloj.now();
			tiempo_factorizado = chrono::duration_cast<chrono::microseconds>(finalizacion - invocacion);

			//Sacamos su factor para comprobar que coinciden
			mpz_set_str(s, semiprimos[i].factor_p.c_str(), 10);

			if(mpz_cmp(p, s) == 0){
				salida << semiprimos[i].clave_publica << ",OK," << tiempo_factorizado.count() << "," << tiempo_exploracion.count() << "," << tiempo_divisibilidad.count() << endl;
			}else{
				salida << semiprimos[i].clave_publica << ",FAILURE!!!," << tiempo_factorizado.count() << "," << tiempo_exploracion.count() << "," << tiempo_divisibilidad.count() << endl;
			}

			cout << "\nTiempo requerido: " << tiempo_factorizado.count() << " us\n";
			cout << "Tiempo requerido de divisibilidad: " << tiempo_divisibilidad.count() << " us\n";
			cout << "Tiempo requerido de exploracion: " << tiempo_exploracion.count() << " us\n";
			
			//Si encontramos un factor imprimimos por pantalla la factorizacion
			if(mpz_cmp_ui(p, 0) != 0){
				mpz_cdiv_q(q, N, p);
				cout << endl;
				mpz_out_str(stdout, 10, N);
				cout << " = ";
				mpz_out_str(stdout, 10, p);
				cout << " x ";
				mpz_out_str(stdout, 10, q);
				cout << "\n\n";

				encontrado = true;
			}

			cout << "----------------\n";
			
			//Si no encontramos nada
			if(!encontrado){
			
				//Escribimos que no hubo suerte
				cout << "No hubo suerte...\n";
			}
		}

		salida.close();
	}

	//En cualquier caso esperamos entrada por parte del usuario
	cout << "Presione enter para continuar . . . \n";
	cin.get();
	
	//Liberamos la memoria
	mpz_clears(N, M, p, q, s, NULL);

    return 0;
}