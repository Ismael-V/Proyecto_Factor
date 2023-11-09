#ifndef Z2_Z_POLYNOMIALS
#define Z2_Z_POLYNOMIALS

#include <inttypes.h>
#include <string>

class Z2_poly{
    private:
    uint8_t* bitwise_poly; //Almacena la informacion del polinomio
    uint16_t grado; //Es el grado del polinomio actual

    //Pre: True
    //Post: Genera un polinomio vacio de grado indicado
    Z2_poly(uint16_t grado, bool is_empty);

    public:

    //Pre: True
    //Post: Genera el polinomio 0
    Z2_poly();

    //Pre: True
    //Post: Genera un monomio de grado indicado
    Z2_poly(uint16_t grado);

    //Pre: La cadena esta compuesta de 1s y 0s
    //Post: Genera un polinomio con los correspondientes elementos.
    Z2_poly(std::string cadena_de_bits);

    //Pre: True
    //Post: Copia el objeto al completo
    Z2_poly(Z2_poly& original);

    //Pre: True
    //Post: Mueve el objeto de un lado a otro
    Z2_poly (Z2_poly&& original);

    //Pre: True
    //Post: Libera la memoria del objeto al ser destruido
    ~Z2_poly();

    //Pre: True
    //Post: Devuelve la suma de dos polinomios en Z2_Z
    Z2_poly operator+(Z2_poly& sumando);

    //Pre: True
    //Post: Devuelve la resta de dos polinomios en Z2_Z
    Z2_poly operator-(Z2_poly& sumando){
        return *this + sumando;
    }

    //Pre: True
    //Post: Devuelve el producto de dos polinomios en Z2_Z
    Z2_poly operator*(Z2_poly& factor);

    //Pre: divisor != 0
    //Post: Devuelve la division entera de un polinomio por el divisor
    Z2_poly operator/(Z2_poly& divisor);

    //Pre: divisor != 0
    //Post: Devuelve el resto de dividir un polinomio por el divisor
    Z2_poly operator%(Z2_poly& divisor);

    //Pre: True
    //Post: Devuelve el grado del polinomio
    uint16_t poli_grado();

    //Pre: True
    //Post: Devuelve el polinomio en formato a_0 + a_1*x + a_2*x^2... en una cadena
    std::string to_string();

    //Pre: True
    //Post: Devuelve el polinomio como una cadena binaria de 1s y 0s
    std::string to_binary_representation();
};

#endif