#include <iostream>
#include <fstream>
#include <chrono>
#include "source/Z2_guided_mpi_decarrier.hpp"
#include "source/Solve_Factor.hpp"

using namespace std;

#define U_TYPE uint64_t

vector<string> claves;

//Pre: True
//Post: Lee las claves del fichero indicado las almacena en el vector de claves
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

void working_routine(){

}

void server_routine(){

}

int main(int argc, char** argv){

    if(argc == 2){
        readKeys(argv[1]);
        uint32_t i = 0;
        for(string s : claves){
            cout << "Clave " << i << " --> " << s << endl;
            i++;
        }
    }else{

    }

    return 0;
}