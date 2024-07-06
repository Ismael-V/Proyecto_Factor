#include<iostream>
#include<inttypes.h>
#include<set>
#include<gmp.h>

using namespace std;

#define CLAVE_PUBLICA "4091540513"
#define CLAVE_PUBLICA_0 "1522605027922533360535618378132637429718068114961380688657908494580122963258952897654000350692006139"
#define CLAVE_PUBLICA_1 "35794234179725868774991807832568455403003778024228226193532908190484670252364677411513516111204504060317568667"
#define CLAVE_PUBLICA_2 "412023436986659543855531365332575948179811699844327982845455626433876445565248426198098870423161841879261420247188869492560931776375033421130982397485150944909106910269861031862704114880866970564902903653658867433731720813104105190864254793282601391257624033946373269391"


//Pre: True
//Post: Devuelve el peso de Hamming de un numero de tamanyo arbitrario.
uint16_t mpz_peso_hamming(const mpz_t num){
	mp_bitcnt_t peso_hamming = mpz_popcount(num);
	return (uint16_t)peso_hamming;
}

//Pre: True
//Post: Devuelve la lista de los divisores de un numero dado excepto el 1 y el mismo.
set<uint16_t> divisores(uint16_t n){
	set<uint16_t> divisores;

	for(uint16_t d = 2; d < n; d++){
		if(n % d == 0){
			divisores.insert(d);
		}
	}

	return divisores;
}

//Pre: True
//Post: Devuelve la posicion del digito a 1 mas significativo de un numero dado interpretado en base binaria o 0 si es 0
mp_bitcnt_t last_one_position(const mpz_t n){
	if(mpz_cmp_ui(n, 0) == 0){
		return 0;
	}
	return (mp_bitcnt_t)mpz_sizeinbase(n, 2);
}

void mpz_factoriza_recursivo(mpz_t rop, const mpz_t N, const mpz_t M, mpz_t solucion_fabricada, uint16_t peso){
	//Calculamos donde colocar el siguiente bit a 1
	mp_bitcnt_t index = last_one_position(solucion_fabricada);
	
	//Lo colocamos en la solucion construida
	mpz_setbit(solucion_fabricada, index);
	
	//Mientras que la solucion construida sea menor al limite de busqueda
	while(mpz_cmp(solucion_fabricada, M) <= 0){
	
		//Si quedan por colocar 1s
		if((peso - 1) > 0){
		
			//Exploramos las soluciones a partir de aqui
			mpz_factoriza_recursivo(rop, N, M, solucion_fabricada, (peso - 1));
			
			//Si hemos encontrado una solucion
			if(mpz_cmp_ui(rop, 0) != 0 ){
			
				//Regresamos
				return;
			}
		//Si ya no quedan 1s por colocar
		}else{

			/*
				mpz_out_str(stdout, 2, solucion_fabricada);
				cout << endl;
			*/
			
			//Si la solucion fabricada divide a N la devolvemos
			if(mpz_divisible_p (N, solucion_fabricada)){
				mpz_set(rop, solucion_fabricada);
				return;
			}
		}
		
		//Quitamos el 1 de esa posicion
		mpz_clrbit(solucion_fabricada, index);
		
		//Sumamos una posicion
		index = index + 1;
		
		//Ponemos el 1 en la nueva posicion
		mpz_setbit(solucion_fabricada, index);
	}
	
	mpz_clrbit(solucion_fabricada, index);
	
	//Devolvemos 0 de no encontrar ningun factor
	mpz_set_ui(rop, 0);
	return;
	
}

int main (int argc, char **argv) {
	bool encontrado = false;

	//Inicializamos el numero a factorizar y el limite de factorizacion
	mpz_t N, M, p, q, s;
	mpz_inits(N, M, p, q, s, NULL);
	
	//Metemos la clave publica en N
	mpz_set_str(N, CLAVE_PUBLICA ,10);
	
	//Metemos la raiz cuadrada de la clave publica en M
	mpz_sqrt(M, N);
	
	//Calculamos el peso de Hamming de la clave publica
	uint16_t peso = mpz_peso_hamming(N);
	
	cout << "Se va ha factorizar el numero: \n\n";
	mpz_out_str(stdout, 10, N);
	cout << "\n\nEl peso del numero es de: " << peso << "\n\n";
	
	//Para todos los divisores de la clave
	for(uint16_t elem : divisores(peso)){
		cout << "Probando para peso: " << elem << endl;
	
		//Buscamos en ese subconjunto el primo que factorice nuestra clave
		mpz_set_ui(s, 0);
		mpz_factoriza_recursivo(p, N, M, s, elem);
		
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
			break;
		}
	}
	
	//Si no encontramos nada
	if(!encontrado){
	
		//Escribimos que no hubo suerte
		cout << "No hubo suerte...\n";
	}
	
	//En cualquier caso esperamos entrada por parte del usuario
	cout << "Presione enter para continuar . . . \n";
	cin.get();
	
	//Liberamos la memoria
	mpz_clears(N, M, p, q, s, NULL);

    return 0;
}
