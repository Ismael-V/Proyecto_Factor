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

void Decarrier::polyInterpretation(uint16_t* poly){

    this->next_poly.clear();

    for(uint16_t i = 0; i < this->size; i++){
        if(poly[i] & 1){
            this->next_poly += '1';
        }else{
            this->next_poly += '0';
        }
    }

    //std::cout << this->next_poly << std::endl;
    
}

//Pre: La cadena esta compuesta de 1s y 0s
//Post: Declara un deacarreador para un polinomio en Z2
Decarrier::Decarrier(std::string poly, uint16_t carrys) : carrys(carrys){
    uint16_t size = poly.size();
    uint8_t begin_poly = 0;

    uint16_t index;
    for(index = 0; index < size && !begin_poly; index++){
        if(poly[index] != '0'){
            begin_poly = 1;
        }
    }

    if(index == size || index == 0){
        this->size = 0;
        return;
    }

    index--;

    this->size = size - index;

    this->indexes = new uint16_t[carrys + 1](0);
    this->carry_guesses = new uint16_t*[carrys + 1];
    for(uint16_t i = 0; i < carrys + 1; i++){
        this->carry_guesses[i] = new uint16_t[this->size](0);
    }

    this->next_poly.reserve(this->size);

    for(uint16_t u = 0; index <= size; index++, u++){
        if(poly[index] == '1'){
            this->carry_guesses[0][u] = 1;
        }else{
            this->carry_guesses[0][u] = 0;
        }
    }

    /*
    for(uint16_t i = 0; i < this->size; i++){
        std::cout << this->carry_guesses[0][i] << "-";
    }

    std::cout << std::endl;
    */
}

Decarrier::~Decarrier(){

    if(this->indexes != nullptr){
        delete this->indexes;
    }

    if(this->carry_guesses != nullptr){
        delete[] this->carry_guesses;
    }

    this->indexes = nullptr;
    this->carry_guesses = nullptr;
}

bool Decarrier::nextDecarry(std::string& next){

    //Si el numero de deacarreos es 0
    if(this->carrys == 0 && this->anidamiento >= 0){

        //Devolvemos la interpretacion del polinomio original
        polyInterpretation(this->carry_guesses[this->anidamiento]);

        //Lo devolvemos
        next = this->next_poly;

        //Restamos en 1 el nivel de anidamiento
        this->anidamiento--;

        //Devolvemos que ha habido siguiente elemento
        return true;
    }

    //Mientras que el anidamiento sea mayor a 0
    while(this->anidamiento >= 0){

        //Si quedan indices apuntando a candidatos a ser deacarreados
        if(this->indexes[anidamiento] < this->size - 1){

            //Para cada elemento del vector actual
            for(uint16_t i = 0; i < this->size; i++){

                //Si el indice coincide con el apuntado
                if(this->indexes[anidamiento] == i){

                    //Ponemos en esta posicion de ser posible el deacarreo
                    if(this->carry_guesses[anidamiento][i] > 0){
                        this->carry_guesses[anidamiento + 1][i] = this->carry_guesses[anidamiento][i] - 1;
                        this->carry_guesses[anidamiento + 1][i + 1] = this->carry_guesses[anidamiento][i + 1] + 2;

                        //Aumentamos en 1 extra el elemento apuntado porque ya ha sido copiado
                        i++;

                    //Sino aumentamos el valor de indexes dentro del rango permitido y copiamos el elemento como toca
                    }else{
                        this->carry_guesses[anidamiento + 1][i] = this->carry_guesses[anidamiento][i];
                        if(this->indexes[anidamiento]++ >= this->size - 1) this->indexes[anidamiento] = this->size - 2;
                    }

                //De lo contrario copiamos como siempre el elemento que toca    
                }else{
                    this->carry_guesses[anidamiento + 1][i] = this->carry_guesses[anidamiento][i];
                }

            }

            /*
                for(uint16_t i = 0; i < this->size; i++){
                    std::cout << this->carry_guesses[anidamiento + 1][i] << "-";
                }

                std::cout << "--- Anidamiento: " << this->anidamiento << " --- indice: " << this->indexes[anidamiento] << " --- target: " << this->carrys << std::endl;
                std::cout << std::endl;
            */
            
            //Avanzamos el indice apuntado
            this->indexes[anidamiento]++;

            //Si hemos alcanzado el objetivo de deacarreos
            if(this->anidamiento + 1 == this->carrys){

                //Interpretamos el resultado como un polinomio en Z2
                polyInterpretation(this->carry_guesses[this->anidamiento + 1]);

                //Lo devolvemos
                next = this->next_poly;

                //Devolvemos que ha habido siguiente elemento
                return true;
            }else{

                //Si no hemos alcanzado el objetivo aumentamos el anidamiento
                this->anidamiento++;
            }

        //Si nos hemos quedado sin posibilidades para deacarrear
        }else{

            //Reseteamos el puntero de indices al inicio
            this->indexes[anidamiento] = 0;

            //Reducimos en 1 el nivel de anidamiento
            this->anidamiento--;
        }
    }

    //Si no quedan posibilidades pues se acabo
    return false;
}

#endif