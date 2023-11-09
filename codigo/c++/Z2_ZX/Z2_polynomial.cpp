#include "Z2_polynomial.h"
#include <iostream>

//Pre: True
//Post: Envia un mensaje de control
void log_message(std::string message){
    std::cout << message << "\n";
    std::cout.flush();
}

//Pre: True
//Post: Genera el polinomio 0
Z2_poly::Z2_poly(){

    //Ponemos el grado a 0
    this->grado = 0;

    //Dedicamos solo un slot en nuestro polinomio
    this->bitwise_poly = new uint8_t[1];

    //Ponemos dicho slot a 0
    this->bitwise_poly[0] = 0;
}

//Pre: True
//Post: Genera un polinomio vacio de grado indicado
Z2_poly::Z2_poly(uint16_t grado, bool is_empty){

    //Metemos el grado indicado
    this->grado = grado;

    //Dedicamos el espacio correspondiente a ese grado
    this->bitwise_poly = new uint8_t[grado + 1];

    //Para cada elemento del polinomio anulamos dicho elemento
    for(uint16_t i = 0; i <= grado; i++){
        this->bitwise_poly[i] = 0;
    }

    if(!is_empty){
        this->bitwise_poly[grado] = 1;
    }
}

//Pre: True
//Post: Genera un monomio de grado indicado
Z2_poly::Z2_poly(uint16_t grado){

    this->grado = grado;
    this->bitwise_poly = new uint8_t[grado + 1];

    for(uint16_t i = 0; i <= grado; i++){
        this->bitwise_poly[i] = 0;
    }

    this->bitwise_poly[grado] = 1;    
}

//Pre: La cadena esta compuesta de 1s y 0s
//Post: Genera un polinomio con los correspondientes elementos.
Z2_poly::Z2_poly(std::string cadena_de_bits){

    //Miramos la longitud de la cadena
    uint16_t longitud = cadena_de_bits.length();

    //Buscamos el primer caracter a 1 de la cadena
    char digito = ' ';
    uint16_t poly_index = -1;

    //Para cada elemento de la cadena
    for(uint16_t i = 0; i < longitud; i++){

        //Si encontramos el primer digito a 1
        if(cadena_de_bits[i] == '1'){

            //Calculamos el grado del polinomio
            this->grado = longitud - 1 - i;

            //Guardamos el indice del primer elemento a 1
            poly_index = i;

            //Finalizamos la busqueda
            i = longitud;
        }
    }

    //Si no hay ningun 1
    if(poly_index == -1){

        //El grado es 0
        this->grado = 0;

        //Solo tiene un espacio
        this->bitwise_poly = new uint8_t[1];

        //Ponemos a 0 su primer coeficiente
        this->bitwise_poly[0] = 0;

        //Regresamos
        return;
    }

    //Dedicamos un booleano a cada elemento del polinomio
    this->bitwise_poly = new uint8_t[this->grado + 1];

    //Para los elementos que queda, metemos los digitos en el vector
    for(uint16_t i = 0; i <= this->grado; i++){
        if(cadena_de_bits[longitud - 1 - i] == '1'){
            this->bitwise_poly[i] = 1;
        }else{
            this->bitwise_poly[i] = 0;
        }
    }

}

//Pre: True
//Post: Copia el objeto al completo
Z2_poly::Z2_poly(Z2_poly& original){

    //Metemos el grado del original
    this->grado = original.grado;

    //Dedicamos memoria para el nuevo polinomio
    this->bitwise_poly = new uint8_t[original.grado + 1];

    //Copiamos elemento por elemento el polinomio original al nuevo
    for(uint16_t i = 0; i <= original.grado; i++){
        this->bitwise_poly[i] = original.bitwise_poly[i];
    }
}

//Pre: True
//Post: Mueve el objeto de un lado a otro
Z2_poly::Z2_poly (Z2_poly&& original){

    //Metemos el grado del original
    this->grado = original.grado;

    //Le cedemos el puntero del original al nuevo
    this->bitwise_poly = original.bitwise_poly;

    //Anulamos el puntero del original
    original.bitwise_poly = nullptr;
}

//Pre: True
//Post: Libera la memoria del objeto al ser destruido
Z2_poly::~Z2_poly(){
    //Si el vector apunta hacia algun lugar
    if(bitwise_poly != nullptr){

        //Liberamos la memoria dedicada en ese punto
        delete bitwise_poly;

        //Anulamos el puntero
        bitwise_poly = nullptr;
    }
}

//Pre: True
//Post: Devuelve la suma de dos polinomios en Z2_Z
Z2_poly Z2_poly::operator+(Z2_poly& sumando){

    //Declaramos un booleano que nos diga quien tiene el grado mayor
    bool this_has_max_degree;

    //Declaramos dos constantes, grado minimo y grado maximo
    uint16_t grado_min, grado_max;

    //Si nuestro objeto posee menor grado que el sumando
    if(this->grado < sumando.grado){

        //Colocamos los grados como corresponden
        grado_min = this->grado;
        grado_max = sumando.grado;

        //Indicamos que nuestro objeto no tiene grado maximo
        this_has_max_degree = false;
    }else{

        //De lo contrario colocamos los grados como corresponden
        grado_min = sumando.grado;
        grado_max = this->grado;

        //Indicamos que nuestro objeto si tiene grado maximo
        this_has_max_degree = true;
    }

    //Declaramos un lugar donde almacenar el resultado que pueda almacenar un objeto de grado maximo
    Z2_poly resultado(grado_max, true);

    //Declaramos un indice con estado
    uint16_t index = 0;

    //Para todos los elementos que comparten ambos polinomios
    for(; index <= grado_min; index++){

        //Calculamos la suma con una xor dado que en Z2_Z la suma es equivalente a la xor
        resultado.bitwise_poly[index] = this->bitwise_poly[index] ^ sumando.bitwise_poly[index];
    }

    //Si nuestro objeto tiene el grado maximo
    if(this_has_max_degree){

        //Para los elementos que resten de nuestro objeto
        for(; index <= grado_max; index++){

            //Los colocamos en el nuevo polinomio
            resultado.bitwise_poly[index] = this->bitwise_poly[index];
        }

    //Si es el sumando quien tiene el grado maximo
    }else{

        //Para los elementos que resten del sumando
        for(; index <= grado_max; index++){

            //Los colocamos en el nuevo polinomio
            resultado.bitwise_poly[index] = sumando.bitwise_poly[index];
        }
    }
    
    //Devolvemos el resultado de la operacion
    return resultado;

}

//Pre: True
//Post: Devuelve el producto de dos polinomios en Z2_Z
Z2_poly Z2_poly::operator*(Z2_poly& factor){

    //Caso en el que alguno sea cero
    if(this->grado == 0 && this->bitwise_poly[0] == 0){
        return Z2_poly(0);
    }else if(factor.grado == 0 && factor.bitwise_poly[0] == 0){
        return Z2_poly(0);
    }

    //Declaramos un lugar donde almacenar el resultado que pueda almacenar un objeto de grado maximo
    Z2_poly resultado(this->grado + factor.grado, true);

    //Para cada elemento del polinomio de este objeto
    for(uint16_t i = 0; i <= this->grado; i++){

        //Si tenemos un 1 en esa posicion
        if(this->bitwise_poly[i]){

            //Sumamos desplazando un desfase de i el factor
            for(uint16_t j = 0; j <= factor.grado; j++){
                resultado.bitwise_poly[i + j] ^= factor.bitwise_poly[j];
            }
        }
    }

    //Devolvemos el resultado
    return resultado;
}

//Pre: divisor != 0
//Post: Devuelve la division entera de un polinomio por el divisor
Z2_poly Z2_poly::operator/(Z2_poly& divisor){
    if(divisor.grado == 0 && divisor.bitwise_poly[0] == 0){
        throw std::logic_error("Division by zero\n");
    }

    return Z2_poly();
}

//Pre: True
//Post: Devuelve el grado del polinomio
uint16_t Z2_poly::poli_grado(){
    return this->grado;
}

//Pre: True
//Post: Devuelve el polinomio en formato a_0 + a_1*x + a_2*x^2... en una cadena
std::string Z2_poly::to_string(){
    
    //Cadena donde almacenaremos el polinomio
    std::string polinomio = "";

    //Indica si es el primer elemento a escribir
    bool first_element = true;

    //Para cada elemento del vector
    for(uint16_t g = 0; g <= this->grado; g++){

        //Si esta a 1 el coeficiente
        if(this->bitwise_poly[g] == 1){

            //Si es el primer elemento
            if(first_element){

                //Si su grado es 0 escribimos 1
                if(g == 0){
                    polinomio += (std::string)"1";

                //Si no escribimos x^g con g el grado en esa posicion
                }else{
                    if(g != 1){
                        polinomio += (std::string)"x^" + std::to_string(g);
                    }else{
                        polinomio += (std::string)"x";
                    }
                    
                }

                //Ya no es el primer elemento
                first_element = false;

            //Si no es el primer elemento
            }else{

                //Escribimos x^g con g el grado asi como la suma correspondiente
                if(g != 1){
                    polinomio += (std::string)" + x^" + std::to_string(g);
                }else{
                    polinomio += (std::string)" + x";
                }
            }
        }
    }

    //Si el polinomio es nulo devolvemos la cadena 0
    if(polinomio == ""){
        return "0";
    }

    //Devolvemos la cadena
    return polinomio;
}

//Pre: True
//Post: Devuelve el polinomio como una cadena binaria de 1s y 0s
std::string Z2_poly::to_binary_representation(){

    //Cadena donde almacenaremos el polinomio
    std::string polinomio = "";

    //Para cada elemento del polinomio leido de mayor a menor grado
    for(int32_t i = this->grado; i >= 0; i--){
        if(this->bitwise_poly[i] == 0){
            polinomio += "0";
        }else{
            polinomio += "1";
        }
    }

    //Devolvemos el polinomio en formato numero
    return polinomio;
}