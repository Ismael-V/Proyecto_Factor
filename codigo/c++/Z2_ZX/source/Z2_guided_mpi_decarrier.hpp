#ifndef Z2_Z_GDECARRIER_HPP
#define Z2_Z_GDECARRIER_HPP

#include <iostream>
#include <cinttypes>
#include <cstring>
#include <string>
#include "mpi.h"

constexpr uint32_t KEY_SIZE = 2048;

constexpr int WORK_CHANNEL = 0;
constexpr int KEY_VAL_CHANNEL = 1;

static uint8_t isInitializedMPI = 0;
static MPI_Datatype MPI_WORK_PACKET;
static MPI_Datatype MPI_STR_KEY_VALUE;
static constexpr uint8_t WPACKET_ELEMS = 12;

//Paquete de trabajo para el deacarreador guiado
struct work_packet{
    uint32_t size; 
    int32_t max_carrys;
    int32_t carrys;
    int32_t meta_index;

    uint32_t convolution_guess[KEY_SIZE];
    uint32_t constraint_vector[KEY_SIZE];
    uint32_t indexes[KEY_SIZE + 1];
    uint32_t times[KEY_SIZE + 1];

    uint8_t target_carry;
    uint8_t exists_guess;
    uint8_t first;

    char next_poly[KEY_SIZE];
};

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

    //Pre: True
    //Post: Constructor vacio
    G_Decarrier(uint32_t tamanyo);

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

    //Pre: True
    //Post: Devuelve el numero de deacarreos acaecidos
    int32_t getCarrys();

    //----> Rutinas MPI <----

    //Pre: Se debe haber ejecutado previamente init_MPI();
    //Post: Envia, de forma bloqueante, el deacarreador al nodo indicado
    static void MPI_Send_GDecarrier(const G_Decarrier &d, int32_t dest);

    //Pre: Se debe haber ejecutado previamente init_MPI();
    //Post: Recibe, de forma bloqueante, un deacarreador del nodo indicado
    static G_Decarrier MPI_Recv_GDecarrier(int32_t src);

};

//----> Rutinas MPI <----

//Pre: True
//Post: Inicializa el tipo de datos, bloque de trabajo para poder enviar este objeto a traves de MPI
int init_MPI();

//Pre: Se debe haber ejecutado previamente init_MPI() y key.length() <= KEY_SIZE
//Post: Envia, de forma bloqueante, la clave o valor al nodo indicado
void MPI_Send_KeyValue(const std::string& key, int32_t dest);

//Pre: Se debe haber ejecutado previamente init_MPI()
//Post: Recibe, de forma bloqueante, la clave o valor del nodo indicado
std::string MPI_Recv_KeyValue(int32_t src);

#endif
