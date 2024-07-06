#include<iostream>
#include<inttypes.h>
#include<set>
#include<stack>
#include<gmp.h>

using namespace std;

#define CLAVE_PUBLICA "10966278391"
#define PRUEBA "49479483219330162579835181237"
#define RSA_100 "1522605027922533360535618378132637429718068114961380688657908494580122963258952897654000350692006139"

/*
 * Soy un Gracioso al pensar que podriamos factorizar alguno de estos :)
 */
#define RSA_896 "412023436986659543855531365332575948179811699844327982845455626433876445565248426198098870423161841879261420247188869492560931776375033421130982397485150944909106910269861031862704114880866970564902903653658867433731720813104105190864254793282601391257624033946373269391"
#define RSA_1024 "135066410865995223349603216278805969938881475605667027524485143851526510604859533833940287150571909441798207282164471551373680419703964191743046496589274256239341020864383202110372958725762358509643110564073501508187510676594629205563685529475213500852879416377328533906109750544334999811150056977236890927563"
#define RSA_1536 "1847699703211741474306835620200164403018549338663410171471785774910651696711161249859337684305435744585616061544571794052229717732524660960646946071249623720442022269756756687378427562389508764678440933285157496578843415088475528298186726451339863364931908084671990431874381283363502795470282653297802934916155811881049844908319545009848393775227257052578591944993870073695755688436933812779613089230392569695253261620823676490316036551371447913932347169566988069"
#define RSA_2048 "25195908475657893494027183240048398571429282126204032027777137836043662020707595556264018525880784406918290641249515082189298559149176184502808489120072844992687392807287776735971418347270261896375014971824691165077613379859095700097330459748808428401797429100642458691817195118746121515172654632282216869987549182422433637259085141865462043576798423387184774447920739934236584823824281198163815010674810451660377306056201619676256133844143603833904414952634432190114657544454178424020924616515723350778707749817125772467962926386356373289912154831438167899885040445364023527381951378636564391212010397122822120720357"

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

void mpz_factoriza_iterativo(mpz_t rop, const mpz_t N, const mpz_t M, uint16_t peso){
	//La solucion con la que trabajaremos
	mpz_t solucion_fabricada;
	mpz_init(solucion_fabricada);

	//El indice
	mp_bitcnt_t index = 0;

	//La condicion de terminacion
	bool seguir = true;

	//Aqui iran los indices para cada 1 insertado
	stack<mp_bitcnt_t> pila_de_indices;

	while(seguir){
		//Colocamos el siguiente 1 en la solucion fabricada
		mpz_setbit(solucion_fabricada, index);

		//Lo contabilizamos en el peso
		peso--;

		//Si la solucion fabricada es impar
		if(mpz_tstbit (solucion_fabricada, 0) == 1){

			//Si la solucion fabricada no excede el limite
			if(mpz_cmp(solucion_fabricada, M) <= 0){

				//Si hemos apilado lo suficiente
				if(peso == 0){
	
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

					//Quitamos el 1 de esa posicion
					mpz_clrbit(solucion_fabricada, index);

					//Si la pila de indices no se ha vaciado por completo
					if(!pila_de_indices.empty()){

						//Desapilamos el indice anterior
						index = pila_de_indices.top();
						pila_de_indices.pop();

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

				//Si no se ha apilado lo suficiente entonces apilamos y volvemos a avanza 1
				}else{
					//Apilamos el indice
					pila_de_indices.push(index);

					//Avanzamos el indice en 1
					index++;

				}

			//Si excedemos el limite en cambio
			}else{
				//Quitamos el 1 de esa posicion
				mpz_clrbit(solucion_fabricada, index);

				//Si la pila de indices no se ha vaciado por completo
				if(!pila_de_indices.empty()){

					//Desapilamos el indice anterior
					index = pila_de_indices.top();
					pila_de_indices.pop();

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
	mpz_set_str(N, PRUEBA, 10);
	
	//Metemos la raiz cuadrada de la clave publica en M
	mpz_sqrt(M, N);
	
	//Calculamos el peso de Hamming de la clave publica
	uint16_t peso = mpz_peso_hamming(N);
	
	cout << "Se va ha factorizar el numero: \n\n";
	mpz_out_str(stdout, 10, N);
	cout << "\n\nEl peso del numero es de: " << peso << "\n\n";
	
	//Para todos los divisores de la clave
	for(uint16_t elem : divisores(peso)){

		cout << "Probando para peso: " << 27 << endl;
	
		//Buscamos en ese subconjunto el primo que factorice nuestra clave
		mpz_set_ui(s, 0);
		mpz_factoriza_iterativo(p, N, M, 27);
		
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
