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
    uint32_t* convolution_guess;
    uint32_t* constraint_vector;

    uint32_t* indexes;
    uint32_t* times;

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

    ~G_Decarrier();

    bool nextDecarry(std::string& next);
};

//Pre: True
//Post: Decide en funcion del indice al que apunta si se puede proceder al deacarreo
bool G_Decarrier::isDecarrable(uint32_t index) const{
    
    //Para cada una de las componentes contiguo al indice apuntado
    for(uint32_t i = index + 1; i < this->size; i++){

        //Si la componente es inferior entonces se puede acomodar el deacarreo
        if(convolution_guess[i] < constraint_vector[i]){
            return true;
        
        //Si la componente es superior entonces es imposible acomodar el deacarreo
        }else if(convolution_guess[i] > constraint_vector[i]){
            return false;
        }

        //Si son iguales comprobamos el resto
    }

    //Si coincide con el vector restriccion, no se puede acomodar
    return false;
}

//Pre: True
//Post: Decide si el vector de convolucion es uno valido
bool G_Decarrier::isValid() const{

    //Para cada elemento del vector de deacarreo testado
    for(uint32_t i = 0; i < this->size; i++){

        //Si alguna de sus componentes excede el maximo teorico que puede devolver una convolucion
        if(convolution_guess[i] > constraint_vector[i] + 1){

            //Entonces no es valido
            return false;
        }
    }

    //Si no, es una posibilidad
    return true;
}

void G_Decarrier::polyInterpretation(){

    next_poly.clear();

    for(uint32_t i = 0; i < size; i++){
        if(convolution_guess[i] & 1){
            next_poly += '1';
        }else{
            next_poly += '0';
        }
    }

    //std::cout << this->next_poly << std::endl;
    
}

//Pre: True
//Post: Devuelve la primera posicion deacarreable comenzando desde index
uint32_t G_Decarrier::nextDecarryPos(uint32_t index){

    for(uint32_t first_index = index; first_index < size;){
        if(convolution_guess[first_index] != 0){
            return first_index;
        }else{
            first_index++;
        }
    }

    return size;
}

G_Decarrier::G_Decarrier(std::string base){
    uint32_t tamanyo = base.length();
    
    uint8_t begin_base = false;
    uint32_t index;

    for(index = 0; index < tamanyo && !begin_base;){
        if(base[index] != '0'){
            begin_base = true;
        }else{
            index++;
        }
    }

    size = tamanyo - index;

    next_poly.reserve(size);

    convolution_guess = new uint32_t[size];
    constraint_vector = new uint32_t[size];
    indexes = new uint32_t[size + 1](0);
    times = new uint32_t[size + 1](0);

    for(uint32_t j = 0; index < tamanyo; index++, j++){
        if(base[index] == '0'){
            convolution_guess[j] = 0;
        }else{
            convolution_guess[j] = 1;
        }
    }

    uint32_t constraint_limit = (size + 1) >> 1;

    for(uint32_t k = 0; k < constraint_limit; k++){
        constraint_vector[k] = k;
        constraint_vector[size - k - 1] = k;
    }
}

G_Decarrier::G_Decarrier(std::string base, uint32_t max_c, uint8_t target) : G_Decarrier(base){
    max_carrys = max_c;
    target_carry = target;
}

G_Decarrier::~G_Decarrier(){

    //std::cout << "Me inmolo\n";
    if(convolution_guess != nullptr){
        delete convolution_guess;
        //std::cout << "Boom_1!!\n";
    }

    if(constraint_vector != nullptr){
        delete constraint_vector;
        //std::cout << "Boom_2!!\n";
    }

    if(indexes != nullptr){
        delete indexes;
        //std::cout << "Boom_3!!\n";
    }

    if(times != nullptr){
        delete times;
        //std::cout << "Boom_4!!\n";
    }

    convolution_guess = nullptr;
    constraint_vector = nullptr;
    indexes = nullptr;
    times = nullptr;

    //std::cout << "Boom!!\n";
}

//Pre: True
//Post: Devuelve la paridad del siguiente vector de deacarreo de haberlo
bool G_Decarrier::nextDecarry(std::string& next){

    //Si es el primer deacarreo
    if(first){

        //Ya no lo sera
        first = false;

        //Interpretamos la guess
        polyInterpretation();

        //Devolvemos la guess
        next = next_poly;

        //Indicamos exito en el encuentro de una nueva guess
        return true;
    }

    //Mientras que queden guesses
    while(exists_guess){

        //Pillamos el primer indice a deacarrear
        uint32_t first_index = indexes[meta_index];
        
        //Si dicho indice esta en el vector y el numero de acarreos no excede el maximo
        if(first_index < size && (max_carrys == -1 || carrys <= max_carrys)){

            //Comprobamos que sea deacarreable
            if(isDecarrable(first_index)){

                //Realizamos el deacarreo
                convolution_guess[first_index] -= 1;
                convolution_guess[first_index + 1] += 2;

                //Indicamos que hemos hecho un deacarreo para el indice indexes[meta_index]
                times[meta_index]++;

                //Anyadimos un deacarreo
                carrys++;

                //Si es valido devolvemos una guess
                if(isValid() && (!target_carry || carrys == max_carrys)){

                    uint8_t first = true;

                    /*

                    for(uint32_t e = 0; e < size; e++ ){    
                        if(first){
                            std::cout << convolution_guess[e];
                            first = false;
                        }else{
                            std::cout << " | " << convolution_guess[e];
                        }
                    }

                    std::cout << std::endl;

                    */

                    //Interpretamos la guess
                    polyInterpretation();

                    //Devolvemos la guess
                    next = next_poly;

                    //Indicamos exito en el encuentro de una nueva guess
                    return true;

                }

            //Si no se puede deacarrear probamos con el siguiente
            }else{
                indexes[meta_index + 1] = nextDecarryPos(indexes[meta_index] + 1);
                times[meta_index + 1] = 0;
                meta_index++;
            }

        //Si no esta o nos hemos pasado con los acarreos
        }else{

            uint8_t keep_backtracking = true;

            //Mientras que sigas retrocediendo
            while(keep_backtracking){

                //Deshacemos un deacarreo, de poder hacerlo y probamos con el siguiente
                if(times[meta_index] > 0){

                    //Descontamos un deacarreo
                    times[meta_index]--;

                    //Descontamos del total de deacarreos
                    carrys--;

                    //Recuperamos el anterior indice
                    first_index = indexes[meta_index];

                    //Deshacemos el deacarreo
                    convolution_guess[first_index] += 1;
                    convolution_guess[first_index + 1] -= 2;

                    //Probamos con el siguiente
                    indexes[meta_index + 1] = nextDecarryPos(first_index + 1);
                    times[meta_index + 1] = 0;
                    meta_index++;

                    keep_backtracking = false;

                //Si no podemos, retrocederemos al nivel anterior
                }else{
                    meta_index--;

                    //Si no hay nivel anterior
                    if(meta_index < 0){

                        //No hay mas guesses
                        exists_guess = false;
                        return false;
                    }
                }
            }
        }
    }

    //No hay mas guesses
    return false;
}

#endif