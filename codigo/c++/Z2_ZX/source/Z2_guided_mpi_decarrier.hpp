#ifndef Z2_Z_GDECARRIER_HPP
#define Z2_Z_GDECARRIER_HPP

#include <iostream>
#include <cinttypes>
#include <string>
#include "mpi.h"

constexpr uint32_t KEY_SIZE = 2048;

static uint8_t isInitializedMPI = 0;
static MPI_Datatype MPI_WORK_PACKET;
static constexpr WPACKET_ELEMS = 13;

//Paquete de trabajo para el deacarreador guiado
static struct {
    uint32_t size; 
    int32_t max_carrys;
    int32_t carrys;
    int32_t meta_index;
    int32_t last_meta_index;

    uint32_t convolution_guess[KEY_SIZE];
    uint32_t constraint_vector[KEY_SIZE];
    uint32_t indexes[KEY_SIZE + 1];
    uint32_t times[KEY_SIZE + 1];

    uint8_t target_carry = false;
    uint8_t exists_guess = true;
    uint8_t first = true;

    char next_poly[KEY_SIZE];
} work_packet;

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
    int32_t last_meta_index = 0;
    uint8_t exists_guess = true;

    uint8_t first = true;

    std::string next_poly = "";

    bool isDecarrable(uint32_t index) const;
    bool isValid() const;
    void polyInterpretation();
    uint32_t nextDecarryPos(uint32_t index);

    //Pre: True
    //Post: Constructor vacio
    G_Decarrier();

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

    bool nextDecarry(std::string& next);

    //Pre: True
    //Post: Delega la rama actual que se explora a otro deacarreador y continua con la siguiente de ser posible
    G_Decarrier branch();

};

//----> Rutinas MPI <----

//Pre: True
//Post: Inicializa el tipo de datos, bloque de trabajo para poder enviar este objeto a traves de MPI
int init_MPI();

//Pre: Se debe haber ejecutado previamente init_MPI();
//Post: Envia, de forma bloqueante, el deacarreador al nodo indicado
void MPI_Send_GDecarrier(const G_Decarrier &d, int32_t dest);

//Pre: True
//Post: Recibe, de forma bloqueante, un deacarreador del nodo indicado
void MPI_RECV_GDecarrier(G_Decarrier &d, int32_t src);

#endif