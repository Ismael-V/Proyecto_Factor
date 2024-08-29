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

constexpr uint32_t CARRY_THRESHOLD = 1;

constexpr uint8_t VERBOSE = 1;

MPI_Datatype MPI_REMOTE_CALL;

void initRemoteCalls(){
    MPI_Type_contiguous(2, MPI_UINT32_T, &MPI_REMOTE_CALL);
    MPI_Type_commit(&MPI_REMOTE_CALL);
}

//Pre: True
//Post: Envia de forma bloqueante una orden al nodo MPI designado
static inline
void sendOrder(uint32_t const order[2], int dest){
    MPI_Ssend(order, 1, MPI_REMOTE_CALL, dest, COMMAND_CHANNEL, MPI_COMM_WORLD);
}

//Pre: True
//Post: Recibe de forma bloqueante una orden del nodo MPI designado
static inline
void recvOrder(uint32_t order[2], int src){
    MPI_Recv(order, 1, MPI_REMOTE_CALL, src, COMMAND_CHANNEL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

//Pre: True
//Post: Rutina que se encarga de realizar las tareas pertinentes que se le manden
void worker_client_routine(atomic<uint32_t>& order, atomic<bool>& requestPend, uint32_t id, std::string key){

    //Declaramos la variable de comandos que enviaremos al master
    uint32_t command[2] = {id, ORD_NULL};

    //Declaramos la variable terminadora
    uint8_t active = 1;

    //Declaramos los numeros
    mpz_t clave_publica;
    mpz_t p;

    //Los inicializamos
    mpz_inits(clave_publica, p, NULL);

    //Colocamos el valor de la clave
    mpz_set_str(clave_publica, key.c_str(), 10);

    //Mientras que la orden recibida no sea de terminacion
    while(active){
        
        if(requestPend){
            //Si la orden es de trabajo
            if(order == ORD_WORK){

		        if(VERBOSE) std::cout << "Worker " + std::to_string(id) + " recibio orden de trabajo\n";

                //Declaramos el deacarreador que recibiremos del nodo master tambien
                G_Decarrier d = G_Decarrier::MPI_Recv_GDecarrier(MASTER_NODE);

                //Indicamos que ya hemos gestionado la peticion
                requestPend = false;

                std::string next_guess;

                if(VERBOSE) std::cout << "Trabajo encomendado al Worker" + std::to_string(id) + "\n";

                //Mientras no se indique terminacion y queden elementos por explorar
                while((order != ORD_TERMINATE) && d.nextDecarry(next_guess)){
            
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
                        
			            if(VERBOSE) std::cout << "Worker " + std::to_string(id) + " encontro solucion, enviando orden\n";

                        //Enviamos la orden de solucion encontrada
                        command[1] = ORD_SOL_FOUND;
                        sendOrder(command, MASTER_NODE);

			            if(VERBOSE) std::cout << "Worker " + std::to_string(id) + " esta enviando la solucion\n";

                        //Enviamos la solucion
                        MPI_Send_KeyValue(std::string(resultado), MASTER_NODE);

			            if(VERBOSE) std::cout << "Worker " + std::to_string(id) + " solucion enviada\n";

                        //Salimos del bucle de intentos
                        break;
                    }
                }

                //Si hemos salido y no ha sido por terminacion o por encontrar solucion
                if((order != ORD_TERMINATE) && (command[1] != ORD_SOL_FOUND)){

		            if(VERBOSE) std::cout << "Worker " + std::to_string(id) + " se quedo sin trabajo\n";

                    //Nos hemos quedado sin trabajo, asi que nos colocamos en pendientes
                    command[1] = ORD_END_WORK;
                    sendOrder(command, MASTER_NODE);

                }

                //Reiniciamos el comando
                command[1] = ORD_NULL;
            }
        }

        //Si la orden es terminar ponemos activo a 0
        if(order == ORD_TERMINATE){    
            active = 0;
        }
    }

    if(VERBOSE) std::cout << "Worker " + std::to_string(id) + " va ha enviar terminacion\n";

    //Enviamos el comando terminar al nodo master
    command[1] = ORD_TERMINATE;
    sendOrder(command, MASTER_NODE);

    //Finalizamos los numeros
    mpz_clears(clave_publica, p, NULL);
}

//Pre: True
//Post: Rutina que se dedica a ofrecer servicio por parte del cliente
static inline
void worker_server_routine(uint32_t id, std::string key){

    //Declaramos un codigo y la variable que gestionara la orden a seguir
    uint32_t code;
    uint32_t command[2];
    atomic<uint32_t> order = ORD_NULL;
    atomic<bool> requestPend = false;

    //Lanzamos un hilo con la rutina de trabajo del cliente
    thread cliente(worker_client_routine, std::ref(order), std::ref(requestPend), id, key);

    if(VERBOSE) std::cout << "Worker " + std::to_string(id) + " funcionando con cliente\n";

    //Mientras que la orden recibida no sea de terminacion
    while(order != ORD_TERMINATE){
        //Escuchamos peticion y hacemos visible la nueva orden
        recvOrder(command, MASTER_NODE);
        order.store(command[1]);

        //Si la orden no es de terminacion, es una peticion a atender por el cliente
        if(order != ORD_TERMINATE) requestPend = true;
        
        if(VERBOSE) std::cout << "Worker " + std::to_string(id) + " recibio comando\n";

        while(requestPend) sched_yield();
    }

    //Terminamos, esperando a que el otro hilo termine
    cliente.join();
}

void master_client_routine(atomic<uint32_t> order[2], atomic<bool>& requestPend, atomic<bool>& activeWorkers, uint32_t numOfWorkers, std::string key, uint32_t max_carrys, bool targetCarry){

    //Declaramos un codigo y la variable que gestionara la orden a seguir
    uint32_t command[2] = {MASTER_NODE, ORD_NULL};

    uint8_t* workers = nullptr;

    if(activeWorkers){
        //Creamos un array de trabajadores y los ponemos con el estado pendiente de orden
        workers = new uint8_t[numOfWorkers];

        for(uint32_t i = 0; i < numOfWorkers; i++){
	        workers[i] = STATUS_PENDING;
	    }
    }

    //Aqui ira el resultado
    std::string factor = "";

    //Este booleano indica si hemos encontrado la solucion
    uint8_t solutionFound = 0;

    //------------> Trabajo previo del master

    //Declaramos los numeros
    mpz_t clave_publica;
    mpz_t p;
    mpz_t zero;

    //Los inicializamos
    mpz_inits(clave_publica, p, zero, NULL);
    mpz_set_ui(zero, 0);

    //Colocamos el valor de la clave
    mpz_set_str(clave_publica, key.c_str(), 10);

    //Declaramos un array donde almacenaremos la representacion binaria
    char binary_representation[KEY_SIZE] = {};

    //Obtenemos la representacion binaria
    mpz_get_str(binary_representation, 2, clave_publica);

    //Declaramos la rama maestra
    G_Decarrier masterBranch(binary_representation, max_carrys, targetCarry);
    
    //Profundizamos en el grafo de deacarreos una cantidad para realizar las ramas
    std::string next_guess = "";

    //Declaramos el numero de acarreos a partir del cual haremos ramas
    uint32_t branch_carry = 0;

    //Si el maximo numero de acarreos es mayor al threshold podemos calcular el punto donde haremos ramas
    if(max_carrys > CARRY_THRESHOLD) branch_carry = max_carrys - CARRY_THRESHOLD;

    //Comenzamos a tomar el tiempo aqui
    std::chrono::time_point<std::chrono::high_resolution_clock> comienzo = std::chrono::high_resolution_clock::now();

    //Mientras no alacancemos una profundidad adecuada y queden elementos por explorar
    while((!activeWorkers || masterBranch.getCarrys() < branch_carry) && masterBranch.nextDecarry(next_guess)){

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
            for(uint32_t i = 0; (i < numOfWorkers) && !solutionFound; i++){
                if(workers[i] != STATUS_OFFLINE){

		            if(VERBOSE) std::cout << "Master finaliza worker" + std::to_string(i + 1) + "\n";

                    command[1] = ORD_TERMINATE;
                    sendOrder(command, i + 1);

                }
            }

            //Hemos encontrado la solucion
            solutionFound = 1;

            //Salimos del bucle de intentos
            break;
        }
    }

    //Miramos que trabajadores estan idle o apagados
    uint8_t areWorking = 1;

    //Mientras que haya algun trabajador activo
    while(activeWorkers){

	//if(VERBOSE) std::cout << "Master esta en rama principal\n";

        //Miramos si hay peticiones pendientes y de haberlas las atendemos
        if(requestPend){
	        if(VERBOSE) std::cout << "Master tiene peticion pendiente\n";

            switch(order[1]){

                //Si el nodo termina su trabajo lo ponemos en espera
                case ORD_END_WORK:
                    workers[order[0] - 1] = STATUS_PENDING;
                    break;

                //Si ha enviado la orden de terminacion lo marcamos como inactivo
                case ORD_TERMINATE:
                    {
                        workers[order[0] - 1] = STATUS_OFFLINE;
                        uint8_t isActive = 0;

                        if(VERBOSE) std::cout << "Master recibe terminacion de worker " + std::to_string(order[0]) + " \n";

                        //Recalculamos activeWorkers
                        for(uint32_t i = 0; i < numOfWorkers && !isActive; i++){
                            if(workers[i] != STATUS_OFFLINE){
                                    isActive = 1;
                            }
                        }
                        activeWorkers = isActive;
                        break;
                    }
                //Si ha encontrado la solucion
                case ORD_SOL_FOUND:

		            if(VERBOSE) std::cout << "Master se entera de que hay solucion\n";

                    //Tomamos la solucion
                    factor = MPI_Recv_KeyValue(order[0]);

		            if(VERBOSE) std::cout << "Master ha recibido solucion\n";

                    //Enviamos el comando de terminacion a todos los nodos si aun no lo hemos hecho
                    for(uint32_t i = 0; (i < numOfWorkers) && !solutionFound; i++){
                        if(workers[i] != STATUS_OFFLINE){

			                if(VERBOSE) std::cout << "Master termina worker " + std::to_string(i + 1) + "\n";

                            command[1] = ORD_TERMINATE;
                            sendOrder(command, i + 1);

                        }
                    }

                    //Indicamos que hemos encontrado la solucion
                    solutionFound = 1;
                    
                    break;
            }

            //Indicamos que hemos atendido la peticion
            requestPend = false;
        }

        //Si hay un trabajador libre le enjaretamos una rama para que trabaje de no haber encontrado aun solucion
        for(uint32_t i = 0; (i < numOfWorkers) && !solutionFound && masterBranch.existsGuess(); i++){

            //Mientras no alacancemos una profundidad adecuada y queden elementos por explorar
            while((masterBranch.getCarrys() < branch_carry) && masterBranch.nextDecarry(next_guess)){

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
                    for(uint32_t i = 0; (i < numOfWorkers) && !solutionFound; i++){
                        if(workers[i] != STATUS_OFFLINE){

                            if(VERBOSE) std::cout << "Master finaliza worker" + std::to_string(i + 1) + "\n";

                            command[1] = ORD_TERMINATE;
                            sendOrder(command, i + 1);

                        }
                    }

                    //Hemos encontrado la solucion
                    solutionFound = 1;

                    //Salimos del bucle de intentos
                    break;
                }
            }

            //Si no encontramos la solucion y hemos profundizado lo necesario
            if(workers[i] == STATUS_PENDING && !solutionFound){

                //Generamos una rama de la rama principal
                G_Decarrier branch(masterBranch.branch());

                //Enviamos la orden de trabajo
                command[1] = ORD_WORK;
		        if(VERBOSE) std::cout << "Master ordena trabajo al worker " + std::to_string(i + 1) + " \n";
                sendOrder(command, i + 1);

			    if(VERBOSE) std::cout << "Master envia deacarreador al worker " + std::to_string(i + 1) + " \n";
                //Enviamos la rama
                G_Decarrier::MPI_Send_GDecarrier(branch, i + 1);

                //Lo ponemos en activo
                workers[i] = STATUS_ACTIVE;
            }
        }

        //Si no existen mas guesses en la rama principal no se ha encontrado solucion y aun trabajan
        if(!masterBranch.existsGuess() && !solutionFound && areWorking){
            
            //Decimos que no hay workers trabajando hasta que se pruebe lo contrario
            areWorking = 0;

            //Miramos si alguno trabaja
            for(uint32_t i = 0; i < numOfWorkers && !areWorking; i++){
                if(workers[i] == STATUS_ACTIVE){
                        areWorking = 1;
                }
            }

            //Si no estan trabajando
            if(!areWorking){

                //Finalizamos los procesos e indicamos que no hemos encontrado el factor
                factor = "No encontrado";

                //Enviamos el comando de terminacion a todos los nodos si aun no lo hemos hecho
                for(uint32_t i = 0; (i < numOfWorkers) && !solutionFound; i++){
                    if(workers[i] != STATUS_OFFLINE){

                        if(VERBOSE) std::cout << "Master termina worker " + std::to_string(i + 1) + "\n";

                        command[1] = ORD_TERMINATE;
                        sendOrder(command, i + 1);

                    }
                }
            }
        }
    }

    //Terminamos de tomar el tiempo aqui
    std::chrono::time_point<std::chrono::high_resolution_clock> final = std::chrono::high_resolution_clock::now();

    //Quitamos el vector de workers y el deacarreador
    delete[] workers;
    workers = nullptr;

    //Finalizamos los numeros
    mpz_clears(clave_publica, p, zero, NULL);

    //Calculamos el tiempo transcurrido
    uint64_t nanosec_ellapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(final - comienzo).count();

    //Escribimos el resultado final
    std::cout << "Clave: " + key + "\nFactor: " + factor + "\nTime ellapsed: " + std::to_string((double)nanosec_ellapsed/std::nano::den) + " s, " + std::to_string(nanosec_ellapsed) + " ns\n";
}

void master_server_routine(uint32_t numOfWorkers, std::string key, uint32_t max_carrys, bool targetCarry){
    //Declaramos un codigo y la variable que gestionara la orden a seguir
    uint32_t command[2] = {MASTER_NODE, ORD_NULL};
    atomic<uint32_t> order[2] = {0, ORD_NULL};
    atomic<bool> requestPend = false;

    //Declaramos el numero de trabajadores activos
    atomic<bool> activeWorkers = (numOfWorkers != 0);

    //Lanzamos un hilo con la rutina de trabajo del cliente
    thread cliente(master_client_routine, order, std::ref(requestPend), std::ref(activeWorkers), numOfWorkers, key, max_carrys, targetCarry);

    //Mientras que haya algun trabajador
    while(activeWorkers){

        //Escuchamos peticion y hacemos visible la nueva orden
        recvOrder(command, MPI_ANY_SOURCE);
	    //MPI_Recv(command, 1, MPI_REMOTE_CALL, MPI_ANY_SOURCE, COMMAND_CHANNEL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	    if(VERBOSE) std::cout << "Server master recibe peticion\n";

        //Guardamos la nueva orden
        order[0] = command[0];
        order[1] = command[1];

        //Hacemos notorio que hay una peticion pendiente
        requestPend = true;

        //Si hay una peticion pendiente dormimos
        while(requestPend) sched_yield();
    }

    //Terminamos, esperando a que el otro hilo termine
    cliente.join();
}

int main(int argc, char** argv){

    //Si el numero de argumentos es 3
    if(argc == 4){
        
        //Declaramos el ierr numero de procesos y mi id
        int ierr, num_procs, my_id;

        //Iniciamos MPI en modo hilo
        int provisto;
        ierr = MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provisto);

        //Si el nivel de threading provisto es insuficiente abortamos la ejecucion.
        if(provisto < MPI_THREAD_MULTIPLE){
            
            ierr = MPI_Finalize();
            std::cout << "Error, el MPI con el que se ha compilado no soporta MPI_THREAD_MULTIPLE\n";
            return 1;
        }

        //Iniciamos realizar target
        bool targetCarry = false;

        //Maximo numero de deacarreos
        uint32_t max_carrys = std::stoul(std::string(argv[2]));

        //Si indicamos que si
        if(std::string(argv[3]) == std::string("true")){

            //Activamos la funcionalidad
            targetCarry = true;

            if(VERBOSE) std::cout << "Targeting carry\n";
        }

        //Iniciamos los numeros
        ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
        ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

        //Iniciamos las comunicaciones
        init_MPI();
        initRemoteCalls();

        //En funcion del tipo de proceso que sea inicio como master o worker
        if(my_id == MASTER_NODE){
            master_server_routine(num_procs - 1, std::string(argv[1]), max_carrys, targetCarry);
        }else{
            worker_server_routine(my_id, std::string(argv[1]));
        }
        
        //Finalizamos MPI
        ierr = MPI_Finalize();
    }else{
        std::cout << "Utilizacion: " << argv[0] << " <clave_publica> <max_deacarreos> <target_carry[true]>\n";
    }

    return 0;
}
