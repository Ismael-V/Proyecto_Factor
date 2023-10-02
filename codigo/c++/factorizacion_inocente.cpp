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

void mpz_factorizacion_inocente(mpz_t rop, const mpz_t N){
    //La solucion con la que trabajaremos
	mpz_t solucion_fabricada;
	mpz_init(solucion_fabricada);

    //Comenzamos en el 3
    mpz_set_ui(solucion_fabricada, 3);

    //Mientras que no divida la solucion_fabricada al numero N
    while(!mpz_divisible_p (N, solucion_fabricada)){

        //Probamos con el siguiente numero primo
        mpz_nextprime(solucion_fabricada, solucion_fabricada);
    }

    //Devolvemos la solucion pertinente
    mpz_set(rop, solucion_fabricada);
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
#define RESULTADOS "inocent_semiprimes_results_64.csv"

int main(){

	//Inicializamos el numero a factorizar y el limite de factorizacion
	mpz_t N, p, q, s;
	mpz_inits(N, p, q, s, NULL);

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

		salida << "Numero,Estado,Tiempo de Ejecución (us)\n";
		
		//Para cada elemento del benchmark
		for(uint16_t i = 0; i < numero_datos; i++){
			//Metemos la clave publica en N
			mpz_set_str(N, semiprimos[i].clave_publica.c_str(), 10);
			
			cout << "Se va ha factorizar el numero: ";
			mpz_out_str(stdout, 10, N);
			cout << "\n";
			
			//Buscamos en ese subconjunto el primo que factorice nuestra clave
			invocacion = reloj.now();
			mpz_factorizacion_inocente(p, N);
			finalizacion = reloj.now();
			tiempo_factorizado = chrono::duration_cast<chrono::microseconds>(finalizacion - invocacion);

			//Sacamos su factor para comprobar que coinciden
			mpz_set_str(s, semiprimos[i].factor_p.c_str(), 10);

			if(mpz_cmp(p, s) == 0){
				salida << semiprimos[i].clave_publica << ",OK," << tiempo_factorizado.count() << endl;
			}else{
				salida << semiprimos[i].clave_publica << ",FAILURE!!!," << tiempo_factorizado.count() << endl;
			}

			cout << "\nTiempo requerido: " << tiempo_factorizado.count() << " us\n";
			
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
	mpz_clears(N, p, q, s, NULL);

    return 0;
}