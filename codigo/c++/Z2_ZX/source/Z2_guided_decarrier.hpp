#ifndef Z2_Z_GDECARRIER_HPP
#define Z2_Z_GDECARRIER_HPP

#include <iostream>
#include <cinttypes>
#include <string>

class G_Decarrier{
    private:

    uint32_t size;
    uint8_t target_carry = false;
    int32_t max_carrys = -1;
    int32_t carrys = 0;
    uint32_t* convolution_guess = nullptr;
    uint32_t* constraint_vector = nullptr;

    uint32_t* indexes = nullptr;
    uint32_t* times = nullptr;

    int32_t meta_index = 0;
    uint8_t exists_guess = true;

    uint8_t first = true;

    std::string next_poly = "";

    bool isDecarrable(uint32_t index) const;
    bool isValid() const;
    void polyInterpretation();
    uint32_t nextDecarryPos(uint32_t index);

    public:

    //Pre: True
    //Post: Declara un deacarreador guiado
    G_Decarrier(std::string base);

    //Pre: True
    //Post: Declara un deacarreador guiado con un maximo numero de deacarreos
    G_Decarrier(std::string base, uint32_t max_c, uint8_t target = false);

    //Pre: True
    //Post: Copia el deacarreador
    G_Decarrier(G_Decarrier& other);

    //Pre: True
    //Post: Mueve el deacarreador
    G_Decarrier(G_Decarrier&& other);

    ~G_Decarrier();

    //Pre: True
    //Post: Devuelve la paridad del siguiente vector de deacarreo de haberlo
    bool nextDecarry(std::string& next);

    //Pre: True
    //Post: Devuelve si quedan en la rama posibles guesses
    bool existsGuess();

    //Pre: True
    //Post: Delega la rama actual que se explora a otro deacarreador y continua con la siguiente de ser posible
    G_Decarrier branch();
};

#endif