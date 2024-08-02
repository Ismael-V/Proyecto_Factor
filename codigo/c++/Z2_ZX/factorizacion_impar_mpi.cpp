#include "gmp.h"
#include "mpi.h"
#include <thread>
#include <atomic>
#include <iostream>

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

void master_client_routine(std::atomic<uint32_t> orden[2], std::atomic<bool> peticion_pendiente, std::atomic<bool> trabajadores_activos, uint32_t num_workers, std::string key){

    //Si hay trabajadores activos estaremos pendiente de lo que hagan
    if(trabajadores_activos){

    //Sino nos encargamos nosotros mismos de encontrar la solucion
    }else{

    }

}

void master_server_routine(uint32_t num_workers, std::string key){

    //Declaramos un codigo y la variable que gestionara la orden a seguir
    uint32_t comando[2] = {MASTER_NODE, ORD_NULL};
    std::atomic<uint32_t> orden[2] = {0, ORD_NULL};
    std::atomic<bool> peticion_pendiente = false;

    //Declaramos el numero de trabajadores activos
    std::atomic<bool> trabajadores_activos = (num_workers != 0);

    //Lanzamos un hilo con la rutina de trabajo del cliente
    std::thread cliente(master_client_routine, orden, std::ref(peticion_pendiente), std::ref(trabajadores_activos), num_workers, key);

    //TODO: Logica de recepcion de mensajes
}