#ifndef Z2_Z_DECARRIER_HPP
#define Z2_Z_DECARRIER_HPP

#include <iostream>
#include <cinttypes>
#include <string>

class Decarrier{
    private:

    uint16_t size;
    uint16_t carrys;

    uint16_t** carry_guesses;
    uint16_t* indexes;
    int16_t anidamiento = 0;

    std::string next_poly = "";

    void polyInterpretation(uint16_t* poly);

    public:

    //Pre: La cadena esta compuesta de 1s y 0s
    //Post: Declara un deacarreador para un polinomio en Z2
    Decarrier(std::string poly, uint16_t carrys);

    ~Decarrier();

    //Pre: True
    //Post: Devuelve el siguiente polinomio deacarreado
    bool nextDecarry(std::string& next);

};

#endif