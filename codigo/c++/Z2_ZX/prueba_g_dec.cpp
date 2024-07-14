#include "source/Z2_guided_decarrier.hpp"

int main(){
    G_Decarrier d("1110111");
    std::string foo;

    while(d.nextDecarry(foo)){
        std::cout << foo << std::endl;
    }

    G_Decarrier dd("1110111");
    for(uint32_t i = 0; i < 20; i++){
        G_Decarrier rama = dd.branch();
        while(rama.nextDecarry(foo)){
            std::cout << "Rama " << i << " --> " << foo << std::endl;
        }
    }

    return 0;
}