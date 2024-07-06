#include "Z2_polynomial.hpp"

//Pre: True
//Post: Envia un mensaje de control
void log_message(std::string message){
    std::cout << message << "\n";
    std::cout.flush();
}

//Pre: True
//Post: Devuelve el grado de un polinomio escrito como cadena binaria
uint32_t degreeOfBinaryPolinomialStr(std::string poli){

    //Calculamos la longitud de la cadena
    uint32_t longitud = poli.length();

    //Para cada elemento de la cadena
    for(uint32_t i = 0; i < longitud; i++){

        //Si encontramos el primer 1
        if(poli[i] == '1'){

            //El grado es la longitud menos el indice i menos 1
            return longitud - i - 1;
        }
    }

    //Si no hay 1s el grado es 0
    return 0;
}