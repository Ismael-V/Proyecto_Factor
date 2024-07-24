#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <atomic>
#include "source/Z2_guided_mpi_decarrier.hpp"
#include "source/Solve_Factor.hpp"

using namespace std;

#define U_TYPE uint64_t

vector<string> claves;

constexpr int COMMAND_CHANNEL = 3;

constexpr uint32_t WORKER_FINISH = 0;

constexpr uint32_t MASTER_NODE = 0;

constexpr uint32_t ORD_NULL = 0;
constexpr uint32_t ORD_TERMINATE = 1;
constexpr uint32_t ORD_WORK = 2;
constexpr uint32_t ORD_END_WORK = 4;
constexpr uint32_t ORD_SOL_FOUND = 8;

constexpr uint8_t STATUS_OFFLINE = 0;
constexpr uint8_t STATUS_PENDING = 1;
constexpr uint8_t STATUS_ACTIVE = 2;

constexpr uint32_t CARRY_THRESHOLD = 4;

MPI_Datatype MPI_REMOTE_CALL;

void initRemoteCalls(){
    MPI_Type_contiguous(2, MPI_UINT32_T, &MPI_REMOTE_CALL);
    MPI_Type_commit(&MPI_REMOTE_CALL);
}

//Pre: True
//Post: Envia de forma bloqueante una orden al nodo MPI designado
static inline
void sendOrder(uint32_t const order[2], int dest){
    MPI_Send(order, 1, MPI_REMOTE_CALL, dest, COMMAND_CHANNEL, MPI_COMM_WORLD);
}

//Pre: True
//Post: Recibe de forma bloqueante una orden del nodo MPI designado
static inline
void recvOrder(uint32_t order[2], int src){
    uint32_t order[2] = {0};
    MPI_Recv(order, 1, MPI_REMOTE_CALL, src, COMMAND_CHANNEL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

//Pre: True
//Post: Rutina que se dedica a ofrecer servicio por parte del cliente
static inline
void worker_server_routine(uint32_t id, std::string key){

    //Declaramos un codigo y la variable que gestionara la orden a seguir
    uint32_t code;
    uint32_t command[2];
    atomic<uint32_t> order = ORD_NULL;

    //Lanzamos un hilo con la rutina de trabajo del cliente
    thread cliente(worker_client_routine, std::ref(order), id, key);

    //Mientras que la orden recibida no sea de terminacion
    while(!(order & ORD_TERMINATE)){
        //Escuchamos peticion y hacemos visible la nueva orden
        recvOrder(command, MASTER_NODE);
        order.store(command[1]);
    }

    //Terminamos, esperando a que el otro hilo termine
    cliente.join();
}

//Pre: True
//Post: Rutina que se encarga de realizar las tareas pertinentes que se le manden
void worker_client_routine(atomic<uint32_t>& order, uint32_t id, std::string key){

    //Declaramos la variable de comandos que enviaremos al master
    uint32_t command[2] = {id, ORD_NULL};

    //Declaramos los numeros
    mpz_t clave_publica;
    mpz_t p;

    //Los inicializamos
    mpz_inits(clave_publica, p, NULL);

    //Colocamos el valor de la clave
    mpz_set_str(clave_publica, key.c_str(), 10);

    //Mientras que la orden recibida no sea de terminacion
    while(!(order & ORD_TERMINATE)){
        
        //Si la orden es de trabajo
        if(order & ORD_WORK){

            //Declaramos el deacarreador que recibiremos del nodo master tambien
            G_Decarrier d = G_Decarrier::MPI_Recv_GDecarrier(MASTER_NODE);
            std::string next_guess;

            //Quitamos la mascara de trabajo
            order &= !ORD_WORK;

            //Mientras no se indique terminacion y queden elementos por explorar
            while(!(order & ORD_TERMINATE) && d.nextDecarry(next_guess)){
        
                //Declaramos un polinomio con esa representación
                Z2_poly<U_TYPE> clave_polinomica(next_guess);

                //Declaramos un vector de factores
                std::vector<Z2_poly<U_TYPE>> factores = {};

                //Aplicamos el algoritmo de Berlekamp para factorizar el polinomio representación
                berlekamp_factorize(clave_polinomica, factores);

                //Ordenamos los factores
                std::sort(factores.begin(), factores.end());

                //Buscamos la clave
                solve_factor_blind(factores, clave_publica, p);

                //De encontrarla se la enviamos al nodo master
                if(mpz_cmp_ui(p, 0) != 0){

                    //Declaramos un array donde almacenaremos el numero
                    char resultado[KEY_SIZE] = {};
                    mpz_get_str(resultado, 10, p);
                    
                    //Enviamos la orden de solucion encontrada
                    command[1] = ORD_SOL_FOUND;
                    sendOrder(command, MASTER_NODE);

                    //Enviamos la solucion
                    MPI_Send_KeyValue(std::string(resultado), MASTER_NODE);

                    //Salimos del bucle de intentos
                    break;
                }
            }

            //Si hemos salido y no ha sido por terminacion o por encontrar solucion
            if(!(order & ORD_TERMINATE) && !(command[1] & ORD_SOL_FOUND)){

                //Nos hemos quedado sin trabajo, asi que nos colocamos en pendientes
                command[1] = ORD_END_WORK;
                sendOrder(command, MASTER_NODE);
            }

            //Reiniciamos el comando
            command[1] = ORD_NULL;
        }
    }

    //Enviamos el comando terminar al nodo master
    command[1] = ORD_TERMINATE;
    sendOrder(command, MASTER_NODE);

    //Finalizamos los numeros
    mpz_clears(clave_publica, p, NULL);
}

void master_server_routine(uint32_t numOfWorkers, std::string key){
    //Declaramos un codigo y la variable que gestionara la orden a seguir
    uint32_t command[2] = {MASTER_NODE, ORD_NULL};
    atomic<uint32_t> order[2] = {0, ORD_NULL};
    atomic<bool> requestPend = false;

    //Declaramos el numero de trabajadores activos
    atomic<bool> activeWorkers = true;

    //Lanzamos un hilo con la rutina de trabajo del cliente
    thread cliente(master_client_routine, order, std::ref(requestPend), std::ref(activeWorkers), numOfWorkers, key);

    //Mientras que haya algun trabajador
    while(activeWorkers){

        //Escuchamos peticion y hacemos visible la nueva orden
        recvOrder(command, MPI_ANY_SOURCE);

        //Guardamos la nueva orden
        order[0] = command[0];
        order[1] = command[1];

        //Hacemos notorio que hay una peticion pendiente
        requestPend.store(true);

        //Si hay una peticion pendiente dormimos
        while(requestPend) sched_yield();
    }

    //Terminamos, esperando a que el otro hilo termine
    cliente.join();
}

void master_client_routine(atomic<uint32_t> order[2], atomic<bool>& requestPend, atomic<bool>& activeWorkers, uint32_t numOfWorkers, std::string key){

    //Declaramos un codigo y la variable que gestionara la orden a seguir
    uint32_t command[2] = {MASTER_NODE, ORD_NULL};

    //Creamos un array de trabajadores y los ponemos con el estado pendiente de orden
    uint8_t* workers = new uint8_t[activeWorkers](STATUS_PENDING);

    //Aqui ira el resultado
    std::string factor = "";

    //Este booleano indica si hemos encontrado la solucion
    uint8_t solutionFound = 0;

    //------------> Trabajo previo del master

    //Declaramos los numeros
    mpz_t clave_publica;
    mpz_t p;

    //Los inicializamos
    mpz_inits(clave_publica, p, NULL);

    //Colocamos el valor de la clave
    mpz_set_str(clave_publica, key.c_str(), 10);

    //Declaramos un array donde almacenaremos la representacion binaria
    char binary_representation[KEY_SIZE] = {};

    //Obtenemos la representacion binaria
    mpz_get_str(binary_representation, 2, clave_publica);

    //Profundizamos en el grafo de deacarreos una cantidad para realizar las ramas
    G_Decarrier masterBranch(binary_representation);
    std::string next_guess;

    //Si no hay trabajadores lo indicamos
    if(numOfWorkers == 0){
        activeWorkers = false;
    }

    //Mientras no alacancemos una profundidad adecuada y queden elementos por explorar
    while(!activeWorkers || (masterBranch.getCarrys() < CARRY_THRESHOLD && masterBranch.nextDecarry(next_guess))){

        //Declaramos un polinomio con esa representación
        Z2_poly<U_TYPE> clave_polinomica(next_guess);

        //Declaramos un vector de factores
        std::vector<Z2_poly<U_TYPE>> factores = {};

        //Aplicamos el algoritmo de Berlekamp para factorizar el polinomio representación
        berlekamp_factorize(clave_polinomica, factores);

        //Ordenamos los factores
        std::sort(factores.begin(), factores.end());

        //Buscamos la clave
        solve_factor_blind(factores, clave_publica, p);

        //De encontrarla la almacenamos en factor
        if(mpz_cmp_ui(p, 0) != 0){

            //Declaramos un array donde almacenaremos el numero
            char resultado[KEY_SIZE] = {};
            mpz_get_str(resultado, 10, p);
            
            factor = std::string(resultado);

            //Enviamos el comando de terminacion a todos los nodos si aun no lo hemos hecho
            for(uint32_t i = 1; i < numOfWorkers + 1 && !solutionFound; i++){
                if(workers[i] != STATUS_OFFLINE){

                    command[1] = ORD_TERMINATE;
                    sendOrder(command, i);

                }
            }

            //Hemos encontrado la solucion
            solutionFound = 1;

            //Salimos del bucle de intentos
            break;
        }
    }

    //Mientras que haya algun trabajador activo
    while(activeWorkers){

        //Miramos si hay peticiones pendientes y de haberlas las atendemos
        if(requestPend){
            switch(order[1]){

                //Si el nodo termina su trabajo lo ponemos en espera
                case ORD_END_WORK:
                    workers[order[0]] = STATUS_PENDING;
                    break;

                //Si ha enviado la orden de terminacion lo marcamos como inactivo
                case ORD_TERMINATE:
                    workers[order[0]] = STATUS_OFFLINE;
                    uint8_t isActive = 0;

                    //Recalculamos activeWorkers
                    for(uint32_t i = 0; i < numOfWorkers && !isActive; i++){
                        if(workers[i] != STATUS_OFFLINE){
                            isActive = 1;
                        }
                    }

                    activeWorkers.store(isActive);
                    break;

                //Si ha encontrado la solucion
                case ORD_SOL_FOUND:

                    //Tomamos la solucion
                    factor = MPI_Recv_KeyValue(order[0]);

                    //Enviamos el comando de terminacion a todos los nodos si aun no lo hemos hecho
                    for(uint32_t i = 1; i < numOfWorkers + 1 && !solutionFound; i++){
                        if(workers[i] != STATUS_OFFLINE){

                            command[1] = ORD_TERMINATE;
                            sendOrder(command, i);

                        }
                    }

                    //Indicamos que hemos encontrado la solucion
                    solutionFound = true;
                    
                    break;
            }

            //Indicamos que hemos atendido la peticion
            requestPend = false;
        }

        //Si hay un trabajador libre le enjaretamos una rama para que trabaje de no haber encontrado aun solucion
        for(uint32_t i = 0; i < numOfWorkers && !solutionFound; i++){
            if(workers[i] == STATUS_PENDING){
                G_Decarrier branch(masterBranch.branch());

                //Enviamos la orden de trabajo
                command[1] = ORD_WORK;
                sendOrder(command, i + 1);

                //Enviamos la rama
                G_Decarrier::MPI_Send_GDecarrier(branch, i + 1);

                //Lo ponemos en activo
                workers[i] = STATUS_ACTIVE;
            }
        }
    }

    //Quitamos el vector de workers
    delete workers;

    //Finalizamos los numeros
    mpz_clears(clave_publica, p, NULL);

    std::cout << "Clave: " << key << "\nFactor: " << factor << std::endl;
}

int main(int argc, char** argv){

    //Si el numero de argumentos es 2
    if(argc == 2){
        
        //Declaramos el ierr numero de procesos y mi id
        int ierr, num_procs, my_id;

        //Iniciamos MPI
        ierr = MPI_Init(&argc, &argv);

        //Iniciamos los numeros
        ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
        ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

        //Iniciamos las comunicaciones
        init_MPI();
        initRemoteCalls();

        //En funcion del tipo de proceso que sea inicio como master o worker
        if(my_id == MASTER_NODE){
            master_server_routine(num_procs - 1, std::string(argv[1]));
        }else{
            worker_server_routine(my_id, std::string(argv[1]));
        }
        
        //Finalizamos MPI
        ierr = MPI_Finalize();
    }else{

        std::cout << "Utilizacion: " << argv[0] << " <clave_publica>\n";
    }

    return 0;
}
