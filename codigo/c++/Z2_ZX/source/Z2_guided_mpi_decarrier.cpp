#include "Z2_guided_mpi_decarrier.hpp"

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

G_Decarrier::G_Decarrier(uint32_t tamanyo){

    size = tamanyo;
	
    convolution_guess = new uint32_t[size];
    constraint_vector = new uint32_t[size];
    indexes = new uint32_t[size + 1](0);
    times = new uint32_t[size + 1](0);

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

//Pre: True
//Post: Copia el deacarreador
G_Decarrier::G_Decarrier(G_Decarrier& other){

    this->size = other.size;
    this->target_carry = other.target_carry;
    this->max_carrys = other.max_carrys;
    this->carrys = other.carrys;

    //Clonamos los vectores
    this->convolution_guess = new uint32_t[other.size];
    this->constraint_vector = new uint32_t[other.size];

    this->indexes = new uint32_t[other.size + 1];
    this->times = new uint32_t[other.size + 1];

    for(uint32_t i = 0; i < other.size; i++){
        this->convolution_guess[i] = other.convolution_guess[i];
        this->constraint_vector[i] = other.constraint_vector[i];

        this->indexes[i] = other.indexes[i];
        this->times[i] = other.times[i];
    }

    //Clonamos las ultimas componentes de estos vectores
    this->indexes[other.size] = other.indexes[other.size];
    this->times[other.size] = other.times[other.size];

    this->meta_index = other.meta_index;
    this->last_meta_index = other.last_meta_index;
    this->exists_guess = other.exists_guess;

    this->first = other.first;

    std::string next_poly = other.next_poly;
}

//Pre: True
//Post: Mueve el deacarreador
G_Decarrier::G_Decarrier(G_Decarrier&& other){

    this->size = other.size;
    this->target_carry = other.target_carry;
    this->max_carrys = other.max_carrys;
    this->carrys = other.carrys;

    //Movemos los vectores
    if(this->convolution_guess != nullptr){
        delete this->convolution_guess;
        //std::cout << "Boom_1!!\n";
    }

    if(this->constraint_vector != nullptr){
        delete this->constraint_vector;
        //std::cout << "Boom_2!!\n";
    }

    if(this->indexes != nullptr){
        delete this->indexes;
        //std::cout << "Boom_3!!\n";
    }

    if(this->times != nullptr){
        delete this->times;
        //std::cout << "Boom_4!!\n";
    }

    this->convolution_guess = other.convolution_guess;
    this->constraint_vector = other.constraint_vector;

    this->indexes = other.indexes;
    this->times = other.times;

    other.convolution_guess = nullptr;
    other.constraint_vector = nullptr;
    other.indexes = nullptr;
    other.times = nullptr;

    this->meta_index = other.meta_index;
    this->last_meta_index = other.last_meta_index;
    this->exists_guess = other.exists_guess;

    this->first = other.first;

    std::string next_poly = other.next_poly;
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
                    if(meta_index < last_meta_index){

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

//Pre: True
//Post: Delega la rama actual que se explora a otro deacarreador y continua con la siguiente rama
G_Decarrier G_Decarrier::branch(){

    //Clonamos el deacarreador
    G_Decarrier rama_actual(*this);

    //Si no hay mas guesses en la nueva rama paramos
    if(!this->exists_guess){
        return rama_actual;
    }

    //La nueva rama nunca contendra al primer elemento
    this->first = false;

    uint8_t keep_backtracking = true;

    //Mientras que sigas retrocediendo
    while(keep_backtracking){

        //Recuperamos el ultimo indice
        uint32_t first_index = indexes[meta_index];

        //Deshacemos todos los deacarreos de este nivel
        while(times[meta_index] > 0){

            //Descontamos un deacarreo
            times[meta_index]--;

            //Descontamos del total de deacarreos
            carrys--;

            //Deshacemos el deacarreo
            convolution_guess[first_index] += 1;
            convolution_guess[first_index + 1] -= 2;

        }

        //Probamos el siguiente
        first_index = nextDecarryPos(first_index + 1);
        std::cout << "Next start point: " << first_index << std::endl;

        //Si no quedan mas indices en este nivel viajamos al nivel anterior e iteramos de nuevo el bucle
        if(first_index >= size){
            //Si no hay nivel anterior
            if(meta_index < last_meta_index){

                //No hay mas guesses
                this->exists_guess = false;

                //Deja de retroceder
                keep_backtracking = false;
            }else{
                meta_index--;
            }
        
        //Si hemos encontrado un nuevo indice
        }else{

            //Comenzamos a deacarrear por aqui
            indexes[meta_index] = first_index;

            //Pararemos de explorar al llegar al final de esta rama
            rama_actual.last_meta_index = meta_index + 1;

            //Deja de retroceder
            keep_backtracking = false;

        }
    }

    //Devuelve la rama actual
    return rama_actual;
}

//Pre: True
//Post: Inicializa el tipo de datos, bloque de trabajo para poder enviar este objeto a traves de MPI
int init_MPI(){

    if(isInitializedMPI) return 0;
    isInitializedMPI = 1;

    //Declaramos los tamaños de cada campo
    int B[] = {
        1, //uint32_t size; 
        1, //int32_t max_carrys;
        1, //int32_t carrys;
        1, //int32_t meta_index;
        1, //int32_t last_meta_index;
        KEY_SIZE, //uint32_t convolution_guess[KEY_SIZE];
        KEY_SIZE, //uint32_t constraint_vector[KEY_SIZE];
        KEY_SIZE + 1, //uint32_t indexes[KEY_SIZE];
        KEY_SIZE + 1, //uint32_t times[KEY_SIZE];
        1, //uint8_t target_carry = false;
        1, //uint8_t exists_guess = true;
        1, //uint8_t first = true;
        KEY_SIZE //char next_poly[KEY_SIZE];
    };

    //Indicamos donde se localiza cada elemento en el struct
    MPI_Aint D[] = {
        offsetof(struct work_packet, size), //uint32_t size; 
        offsetof(struct work_packet, max_carrys), //int32_t max_carrys;
        offsetof(struct work_packet, carrys), //int32_t carrys;
        offsetof(struct work_packet, meta_index), //int32_t meta_index;
        offsetof(struct work_packet, last_meta_index), //int32_t last_meta_index;
        offsetof(struct work_packet, convolution_guess), //uint32_t convolution_guess[KEY_SIZE];
        offsetof(struct work_packet, constraint_vector), //uint32_t constraint_vector[KEY_SIZE];
        offsetof(struct work_packet, indexes), //uint32_t indexes[KEY_SIZE];
        offsetof(struct work_packet, times), //uint32_t times[KEY_SIZE];
        offsetof(struct work_packet, target_carry), //uint8_t target_carry = false;
        offsetof(struct work_packet, exists_guess), //uint8_t exists_guess = true;
        offsetof(struct work_packet, first), //uint8_t first = true;
        offsetof(struct work_packet, next_poly) //char next_poly[KEY_SIZE];
    };

    //Indicamos el tipo de cada elemento en tipos basicos de MPI
    MPI_Datatype T[] = {
        MPI_UINT32_T, //uint32_t size; 
        MPI_INT32_T, //int32_t max_carrys;
        MPI_INT32_T, //int32_t carrys;
        MPI_INT32_T, //int32_t meta_index;
        MPI_INT32_T, //int32_t last_meta_index;
        MPI_UINT32_T, //uint32_t convolution_guess[KEY_SIZE];
        MPI_UINT32_T, //uint32_t constraint_vector[KEY_SIZE];
        MPI_UINT32_T, //uint32_t indexes[KEY_SIZE];
        MPI_UINT32_T, //uint32_t times[KEY_SIZE];
        MPI_UINT8_T, //uint8_t target_carry = false;
        MPI_UINT8_T, //uint8_t exists_guess = true;
        MPI_UINT8_T, //uint8_t first = true;
        MPI_CHAR //char next_poly[KEY_SIZE];
    };

    //Generamos el tipo de datos
    MPI_Type_create_struct(WPACKET_ELEMS, B, D, T, &MPI_WORK_PACKET);

    //Generamos el tipo de datos para enviar claves y resultados
    MPI_Type_contiguous(KEY_SIZE, MPI_CHAR, &MPI_STR_KEY_VALUE);

    //Los consolidamos
    MPI_Type_commit(&MPI_WORK_PACKET);
    MPI_Type_commit(&MPI_STR_KEY_VALUE);

    return 1;
}

//Pre: Se debe haber ejecutado previamente init_MPI();
//Post: Envia, de forma bloqueante, el deacarreador al nodo indicado
void G_Decarrier::MPI_Send_GDecarrier(const G_Decarrier &d, int32_t dest){

    //Declaramos un paquete de trabajo
    work_packet paquete;

    //Escribimos cada campo del paquete de trabajo con la copia de nuestro decarrier
    paquete.size = d.size;
    paquete.max_carrys = d.max_carrys;
    paquete.carrys = d.carrys;
    paquete.meta_index = d.meta_index;
    paquete.last_meta_index = d.last_meta_index;

    for(uint32_t i = 0; i < d.size; i++){
        paquete.convolution_guess[i] = d.convolution_guess[i];
        paquete.constraint_vector[i] = d.constraint_vector[i];
        paquete.indexes[i] = d.indexes[i];
        paquete.times[i] = d.times[i];
        paquete.next_poly[i] = d.next_poly[i];
    }

    paquete.indexes[d.size] = d.indexes[d.size];
    paquete.times[d.size] = d.times[d.size];
    
    paquete.target_carry = d.target_carry;
    paquete.exists_guess = d.exists_guess;
    paquete.first = d.first;

    //Lo enviamos con la API de MPI
    MPI_Send(&paquete, 1, MPI_WORK_PACKET, dest, WORK_CHANNEL, MPI_COMM_WORLD);
}

//Pre: Se debe haber ejecutado previamente init_MPI();
//Post: Recibe, de forma bloqueante, un deacarreador del nodo indicado
G_Decarrier G_Decarrier::MPI_Recv_GDecarrier(int32_t src){

    //Declaramos un paquete de trabajo
    work_packet paquete;

    //Recibimos los datos con esta sentencia
    MPI_Recv(&paquete, 1, MPI_WORK_PACKET, src, WORK_CHANNEL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


    //Declaramos el deacarreador que construiremos
    G_Decarrier d(paquete.size);

    //Escribimos cada campo del paquete de trabajo con la copia de nuestro decarrier
    d.max_carrys = paquete.max_carrys;
    d.carrys = paquete.carrys;
    d.meta_index = paquete.meta_index;
    d.last_meta_index = paquete.last_meta_index;

    for(uint32_t i = 0; i < paquete.size; i++){
        d.convolution_guess[i] = paquete.convolution_guess[i];
        d.constraint_vector[i] = paquete.constraint_vector[i];
        d.indexes[i] = paquete.indexes[i];
        d.times[i] = paquete.times[i];
        d.next_poly[i] = paquete.next_poly[i];
    }

    d.indexes[paquete.size] = paquete.indexes[paquete.size];
    d.times[paquete.size] = paquete.times[paquete.size];
    
    d.target_carry = paquete.target_carry;
    d.exists_guess = paquete.exists_guess;
    d.first = paquete.first;

    //Devolvemos el deacarreador recibido
    return d;
}

//Pre: Se debe haber ejecutado previamente init_MPI() y key.length() <= KEY_SIZE
//Post: Envia, de forma bloqueante, la clave o valor al nodo indicado
void MPI_Send_KeyValue(const std::string& key, int32_t dest){

    //Generamos un buffer
    char clave[KEY_SIZE];

    //Si excedemos el tamanyo permitido de envio regresamos
    if(key.length() > KEY_SIZE) return;

    //Copiamos la clave o valor al buffer
    strcpy(clave, key.c_str());

    //Lo enviamos con la API de MPI
    MPI_Send(&clave, 1, MPI_STR_KEY_VALUE, dest, KEY_VAL_CHANNEL, MPI_COMM_WORLD);
}

//Pre: Se debe haber ejecutado previamente init_MPI()
//Post: Recibe, de forma bloqueante, la clave o valor del nodo indicado
std::string MPI_Recv_KeyValue(int32_t src){

    //Generamos un buffer
    char clave[KEY_SIZE];

    //Recibimos los datos con esta sentencia
    MPI_Recv(&clave, 1, MPI_STR_KEY_VALUE, src, KEY_VAL_CHANNEL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    //Devolvemos la clave como una cadena de caracteres
    return std::string(clave);
}
