#include "source/Z2_guided_decarrier.hpp"

int main(){
    G_Decarrier d("1110111");

    std::string foo;
    while(d.nextDecarry(foo)){
        std::cout << foo << std::endl;
    };

    G_Decarrier d2("1110111");
    G_Decarrier b1 = d2.branch();

    std::cout << "Rama 1\n";

    while(b1.nextDecarry(foo)){
        std::cout << foo << std::endl;
    }

    std::cout << "\nRama 2\n";

    while(d2.nextDecarry(foo)){
        std::cout << foo << std::endl;
    }

    std::cout << "Salgo" << std::endl;

    return 0;
}