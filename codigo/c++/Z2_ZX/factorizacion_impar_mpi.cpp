#include "gmp.h"
#include "mpi.h"
#include <thread>
#include <atomic>
#include <iostream>
#include <chrono>

constexpr uint32_t KEY_SIZE = 2048;
constexpr int WORK_CHANNEL = 0;
constexpr int KEY_VAL_CHANNEL = 1;

MPI_Datatype MPI_REMOTE_CALL;
MPI_Datatype MPI_STR_KEY_VALUE;

constexpr uint32_t MASTER_NODE = 0;

constexpr uint32_t ORD_NULL = 0;
constexpr uint32_t ORD_TERMINATE = 1;
constexpr uint32_t ORD_SOL_FOUND = 2;

void initRemoteCalls(){
    MPI_Type_contiguous(2, MPI_UINT32_T, &MPI_REMOTE_CALL);
    MPI_Type_commit(&MPI_REMOTE_CALL);
}

void initKeyValueExchange(){
    MPI_Type_contiguous(KEY_SIZE, MPI_CHAR, &MPI_STR_KEY_VALUE);
    MPI_Type_commit(&MPI_STR_KEY_VALUE);
}

void worker_client_routine(std::atomic<bool>& enviando_datos, std::atomic<bool>& terminate, std::string key, uint32_t id, uint32_t num_workers){

    //Inicializamos la variable para enviar comandos
    uint32_t comando[2];

    //Indicamos el id del hilo en la primera componente del comando
    comando[0] = id;

    //Inicializamos el numero a factorizar y el limite de factorizacion
	mpz_t N, sqrt_N, guess, sup_limit;
	mpz_inits(N, sqrt_N, guess, sup_limit, NULL);

    //Inicializamos la clave publica
    mpz_set_str(N, key.c_str(), 10);

    //Calculamos su raiz cuadrada
    mpz_sqrt(sqrt_N, N);

    //Calculamos el trozo de trabajo que le toca al worker en funcion de su id
    mpz_div_ui(sup_limit, sqrt_N, num_workers);

    //Empezaremos desde el trozo multiplicado por su id - 1
    mpz_mul_ui(guess, sup_limit, id - 1);

    //Si nuestro id coincide con el numero de workers
    if(id == num_workers){

        //El limite superior coincide con el maximo a probar + 1, "por si es un cuadrado perfecto..."
        mpz_set(sup_limit, sqrt_N);
        mpz_add_ui(sup_limit, sup_limit, 1);

    //Sino probaremos desde guess hasta guess + trozo_de_trabajo
    }else{
        mpz_add(sup_limit, sup_limit, guess);
    }

    //Si la guess es par la corregimos para que sea impar
    if(mpz_even_p(guess)) mpz_add_ui(guess, guess, 1);

    //Mientras que la guess no exceda al limite superior y no se indique terminacion
    while(mpz_cmp(guess, sup_limit) <= 0 && !terminate){

        //Si la guess divide a N
        if(mpz_divisible_p(N, guess)){

            //Indicamos que vamos a enviar datos
            enviando_datos = true;

            //Si aun no se ha indicado terminacion tenemos via libre de enviar datos
            if(!terminate){

                //Declaramos un buffer para sacar la solucion
                char solucion[KEY_SIZE];
                mpz_get_str(solucion, 10, guess);

                //Indicamos el comando de solucion encontrada
                comando[1] = ORD_SOL_FOUND;

                //Enviamos el comando de solucion encontrada junto con la solucion
                MPI_Ssend(comando, 1, MPI_REMOTE_CALL, MASTER_NODE, WORK_CHANNEL, MPI_COMM_WORLD);
                MPI_Ssend(solucion, 1, MPI_STR_KEY_VALUE, MASTER_NODE, KEY_VAL_CHANNEL, MPI_COMM_WORLD);
            }

            //Indicamos que hemos terminado de enviar datos
            enviando_datos = false;

            //Salimos del bucle
            break;
        }

        //Si no divide probamos con la siguiente
        mpz_add_ui(guess, guess, 2);
    }

    //Finalizamos los elementos empleados
    mpz_clears(N, sqrt_N, guess, sup_limit, NULL);
}

void worker_server_routine(uint32_t id, std::string key, uint32_t num_workers){

    //Declaramos un lugar donde recibir comandos
    uint32_t comando[2];

    //Declaramos una variable que indica si el worker esta enviando una solucion
    std::atomic<bool> enviando_datos = false;
    std::atomic<bool> terminate = false;

    //Invocamos al cliente de nuestro worker
    std::thread cliente(worker_client_routine, std::ref(enviando_datos), std::ref(terminate), key, id, num_workers);

    //Esperamos la recepcion de una orden de parada
    MPI_Recv(&comando, 1, MPI_REMOTE_CALL, MASTER_NODE, WORK_CHANNEL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    //Si recibimos la orden de terminar, mandamos la orden
    terminate = true;

    //Mientras que estemos enviando datos no hacemos nada
    while(enviando_datos) sched_yield();

    //Confirmamos la recepción de la orden de terminacion por parte del worker
    comando[0] = id;

    //Enviamos el comando al servidor
    MPI_Ssend(&comando, 1, MPI_REMOTE_CALL, MASTER_NODE, WORK_CHANNEL, MPI_COMM_WORLD);

    //Esperamos a que el cliente finalice
    cliente.join();
}

void master_client_routine(std::atomic<uint32_t> orden[2], std::atomic<bool>& peticion_pendiente, std::atomic<bool>& trabajadores_activos, uint32_t num_workers, std::string key){

    //Declaramos el lugar donde sacaremos el factor
    std::string factor;

    //Declaramos el comando con el que trabajaremos
    const uint32_t comando[2] = {MASTER_NODE, ORD_TERMINATE};

    //Esta variable indica si hemos encontrado la solucion
    bool solucion_encontrada = false;

    //Comenzamos a tomar el tiempo aqui, (aunque los workers ya estarian a lo suyo)
    std::chrono::time_point<std::chrono::high_resolution_clock> comienzo = std::chrono::high_resolution_clock::now();

    //Si hay trabajadores activos estaremos pendiente de lo que hagan
    if(trabajadores_activos){

        //Declaramos el vector de trabajadores activos
        uint8_t* workers = new uint8_t[num_workers];

        //Para cada elemento del vector
        for(uint32_t i = 0; i < num_workers; i++){

            //Indicamos que dicho worker esta operativo
            workers[i] = 1;
        }

        //Mientras que esten los trabajadores activos
        while(trabajadores_activos){

            //Si hay una peticion pendiente
            if(peticion_pendiente){

                //Sacamos la peticion
                uint32_t peticion = orden[1];
                uint32_t nodo = orden[0];

                switch(peticion){
                    //Si se ha encontrado la solucion
                    case ORD_SOL_FOUND:

                        //Generamos un buffer donde recibiremos la clave
                        char c_factor[KEY_SIZE];

                        //Recibimos la clave del nodo indicado
                        MPI_Recv(&c_factor, 1, MPI_STR_KEY_VALUE, nodo, KEY_VAL_CHANNEL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                        //Guardamos la clave en factor
                        factor = std::string(c_factor);

                        //Enviamos una orden de terminacion a todos los worker activos
                        for(uint32_t i = 0; i < num_workers && !solucion_encontrada; i++){

                            //Si el worker esta activo
                            if(workers[i]){

                                //Le enviamos una orden de terminacion
                                MPI_Ssend(&comando, 1, MPI_REMOTE_CALL, i + 1, WORK_CHANNEL, MPI_COMM_WORLD);
                            }
                        }

                        //Indicamos que hemos encontrado solucion
                        solucion_encontrada = true;
                    break;
                    case ORD_TERMINATE:

                        //Sacamos el nodo que nos ha comunicado terminacion
                        uint32_t nodo = orden[0] - 1;

                        //Indicamos su inactividad
                        workers[nodo] = 0;

                        //Recalculamos si todos los trabajadores estan o no activos
                        uint8_t esta_activo = 0;

                        for(uint32_t i = 0; i < num_workers && !esta_activo; i++){
                            esta_activo |= workers[i];
                        }

                        //Este valor se almacenara en nuestra variable de terminacion
                        if(!esta_activo) trabajadores_activos = false;
                    break;
                }

                //Indicamos que hemos terminado de gestionar la peticion
                peticion_pendiente = false;
            }
        }

    //Sino nos encargamos nosotros mismos de encontrar la solucion
    }else{

        //Inicializamos el numero a factorizar y el limite de factorizacion
        mpz_t N, sqrt_N, guess;
        mpz_inits(N, sqrt_N, guess, NULL);

        //Inicializamos la clave publica
        mpz_set_str(N, key.c_str(), 10);

        //Calculamos su raiz cuadrada
        mpz_sqrt(sqrt_N, N);

        //Ponemos la primera guess
        mpz_set_ui(guess, 3);


        //Mientras que la guess no exceda al limite superior y no se indique terminacion
        while(mpz_cmp(guess, sqrt_N) <= 0){
            if(mpz_divisible_p(N, guess)){
                //Declaramos un buffer para sacar la solucion
                char solucion[KEY_SIZE];
                mpz_get_str(solucion, 10, guess);

                //El factor sera el encontrado por la solucion
                factor = std::string(solucion);
                break;
            }
        }

        mpz_clears(N, sqrt_N, guess, NULL);
    }

    //Terminamos de tomar el tiempo aqui
    std::chrono::time_point<std::chrono::high_resolution_clock> final = std::chrono::high_resolution_clock::now();

    //Calculamos el tiempo transcurrido
    uint64_t nanosec_ellapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(final - comienzo).count();

    //Escribimos el resultado final
    std::cout << "Clave: " + key + "\nFactor: " + factor + "\nTime ellapsed: " + std::to_string((double)nanosec_ellapsed/std::nano::den) + " s, " + std::to_string(nanosec_ellapsed) + " ns\n";
}

void master_server_routine(uint32_t num_workers, std::string key){

    //Declaramos un codigo y la variable que gestionara la orden a seguir
    uint32_t comando[2] = {MASTER_NODE, ORD_NULL};
    std::atomic<uint32_t> orden[2] = {MASTER_NODE, ORD_NULL};
    std::atomic<bool> peticion_pendiente = false;

    //Declaramos el numero de trabajadores activos
    std::atomic<bool> trabajadores_activos = (num_workers != 0);

    //Lanzamos un hilo con la rutina de trabajo del cliente
    std::thread cliente(master_client_routine, orden, std::ref(peticion_pendiente), std::ref(trabajadores_activos), num_workers, key);

    //Mientras haya trabajadores activos
    while(trabajadores_activos){

        //Recibimos sus peticiones
        MPI_Recv(&comando, 1, MPI_REMOTE_CALL, MPI_ANY_SOURCE, WORK_CHANNEL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        //Sacamos la orden en concreto
        orden[0] = comando[0];
        orden[1] = comando[1];

        //Indicamos que hay una peticion pendiente
        peticion_pendiente = true;

        //Hasta que no atendamos la peticion no proseguimos a escuchar mas
        while(peticion_pendiente) sched_yield();
    }

    //Esperamos a que el cliente finalice
    cliente.join();
}

int main(int argc, char** argv){

    //Si el numero de argumentos es 3
    if(argc == 2){
        
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

        //Iniciamos los numeros
        ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
        ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

        //Iniciamos las comunicaciones
        initRemoteCalls();
        initKeyValueExchange();

        //En funcion del tipo de proceso que sea inicio como master o worker
        if(my_id == MASTER_NODE){
            master_server_routine(num_procs - 1, std::string(argv[1]));
        }else{
            worker_server_routine(my_id, std::string(argv[1]), num_procs - 1);
        }
        
        //Finalizamos MPI
        ierr = MPI_Finalize();
    }else{
        std::cout << "Utilizacion: " << argv[0] << " <clave_publica>\n";
    }

    return 0;
}