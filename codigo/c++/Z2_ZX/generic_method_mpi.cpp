#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include "source/Z2_guided_mpi_decarrier.hpp"
#include "source/Solve_Factor.hpp"

using namespace std;

#define U_TYPE uint64_t

vector<string> claves;

//Pre: True
//Post: Lee las claves del fichero indicado y las almacena en el vector de claves
bool readKeys(string fichero){

    ifstream entrada(fichero);
    if(entrada.is_open()){
        string clave;
        getline(entrada, clave);

        while(!entrada.eof()){
            claves.push_back(clave);
            getline(entrada, clave);
        }

        entrada.close();
    }else{
        return false;
    }

    return true;
}

void worker_server_routine(){

}

void worker_client_routine(){

}

void working_routine(){

    //Declaramos los numeros
    mpz_t clave_publica;
    mpz_t p;

    //Los inicializamos
    mpz_inits(clave_publica, p, NULL);

    //Declaramos la clave que recibiremos del nodo master
    std::string key = MPI_Recv_KeyValue(0);

    //Colocamos el valor de la clave
    mpz_set_str(clave_publica, key.c_str(), 10);

    //Declaramos el deacarreador que recibiremos del nodo master tambien
    G_Decarrier d = G_Decarrier::MPI_Recv_GDecarrier(0);
    std::string next_guess;

    while(d.nextDecarry(next_guess)){
 
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

        //Escribimos el resultado de no ser 0
        if(mpz_cmp_ui(p, 0) != 0){
            std::cout << "Resultado: ";
            mpz_out_str(stdout, 10, p);
            std::cout << std::endl;

            std::cout << next_guess << std::endl;

            //Salimos del bucle de intentos
            break;
        }
    }
}

int main(int argc, char** argv){

    if(argc == 2){
        readKeys(argv[1]);
        uint32_t i = 0;
        int ierr, num_procs, my_id;

        int tag = 0;

        ierr = MPI_Init(&argc, &argv);

        ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
        ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

        init_MPI();

        if(my_id == 0){
            for(string s : claves){
                G_Decarrier d(s);
                for(uint32_t i = 1; i < num_procs; i++){
                    G_Decarrier rama = d.branch();
                    G_Decarrier::MPI_Send_GDecarrier(rama, i);
                }

                break;
            }
        }else{
            G_Decarrier mi_rama = G_Decarrier::MPI_Recv_GDecarrier(0);
            std::string next_guess;
            while(mi_rama.nextDecarry(next_guess)){
                std::cout << "Id --> " << my_id << " : " << next_guess << std::endl;
            }
        }
        
        ierr = MPI_Finalize();
    }else{

    }

    return 0;
}