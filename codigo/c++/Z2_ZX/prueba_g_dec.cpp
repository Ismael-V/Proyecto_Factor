#include "Z2_guided_decarrier.hpp"

int main(){
    G_Decarrier d("10101101", 4);

    std::string foo;
    while(d.nextDecarry(foo)){
        std::cout << foo << std::endl;
    };

    std::cout << "Salgo" << std::endl;

    return 0;
}