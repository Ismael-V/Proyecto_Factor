#include "source/Z2_guided_decarrier.hpp"

const char key[] = "110010110101";

int main(){
    G_Decarrier d(key);
    std::string foo;

    while(d.nextDecarry(foo)){
        std::cout << foo << std::endl;
    }

    G_Decarrier dd(key);

    std::cout << "---Exploracion por ramas---\n";

    dd.nextDecarry(foo);
    //std::cout << foo << std::endl;

    for(uint32_t i = 0; i < 20; i++){
        G_Decarrier rama = dd.branch();

        if(!rama.existsGuess()) break;

        std::cout << "Rama " << i << " --> " << std::endl;
        while(rama.nextDecarry(foo)){
            //std::cout << foo << std::endl;
        }
    }

    return 0;
}