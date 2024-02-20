#ifndef Z2_Z_LINEAR_SOLVER
#define Z2_Z_LINEAR_SOLVER

#include <vector>
#include <set>
#include <algorithm>
#include "Z2_polynomial.hpp"

template <std::unsigned_integral I>
class Z2_matrix_row{

    //Valor de la fila
    Z2_poly<I> row;

    //Indice del valor i
    uint32_t a_i = 0;

    //Valor de los simbolos
    Z2_poly<I> symbols;

    public:

    //Inicializamos la fila de la matriz con la fila, la id y los simbolos
    Z2_matrix_row(Z2_poly<I> fila, uint32_t id, Z2_poly<I> simbolos) : row(fila), a_i(id), symbols(simbolos) {}

    //Pre: True
    //Post: Realiza la suma de dos filas
    Z2_matrix_row<I> operator+(const Z2_matrix_row<I>& sumando) const{
        return Z2_matrix_row(this->row + sumando.row, this->a_i, this->symbols + sumando.symbols);
    }

    //Pre: True
    //Post: Devuelve si nuestra fila es mayor a la otra fila
    bool operator>(const Z2_matrix_row<I>& otro){
        return this->row > otro.row;
    }

    //Pre: True
    //Post: Devuelve si nuestra fila es menor a la otra fila
    bool operator<(const Z2_matrix_row<I>& otro){
        return this->row < otro.row;
    }

    void substitute_row(const Z2_poly<I>& fila, const Z2_poly<I>& simbolo){
        this->row = fila;
        this->symbols = simbolo;
    }

    //Pre: True
    //Post: Devuelve la informacion relevante de la fila
    std::string to_string() const{
        return this->row.to_string() + " - a_" + std::to_string(a_i) + " => " + this->symbols.to_string();
    }

    //Pre: True
    //Post: Devuelve el "grado" de la fila
    uint32_t poli_grado() const{
        return row.poli_grado();
    }

};

template <std::unsigned_integral I>
class Z2_Linear_Solver{
    std::vector<Z2_matrix_row<I>> matriz;

    public:

    Z2_Linear_Solver(Z2_poly<I> generator);

    void solve_linear();
};

template <std::unsigned_integral I>
Z2_Linear_Solver<I>::Z2_Linear_Solver(Z2_poly<I> generator){

    std::cout << generator.to_string() << std::endl;

    uint32_t grado = generator.poli_grado();

    std::vector<Z2_poly<I>> potencias;
    Z2_poly<I> unidad("1");

    for(int32_t i = (grado - 1); i >= 0; i--){

        //std::cout << (unidad.productByMonomial(i << 1) % generator).to_string() << std::endl;
        potencias.push_back((unidad.productByMonomial(i << 1) % generator));
        
    }

    std::vector<std::string> matrizBinaria(grado, "");
 
    uint32_t identity = grado - 1;

    for(Z2_poly<I> p : potencias){

        //std::cout << p.to_string() << std::endl;

        for(int32_t g = (grado - 1); g >= 0; g--){

            if(g != identity){
                if(p[g]){
                    matrizBinaria[grado - 1 - g] += "1";
                }else{
                    matrizBinaria[grado - 1 - g] += "0";
                }
            }else{
                if(p[g]){
                    matrizBinaria[grado - 1 - g] += "0";
                }else{
                    matrizBinaria[grado - 1 - g] += "1";
                }
            }
        }

        identity--;
    }

    uint32_t var_id = grado - 1;

    for(std::string s : matrizBinaria){
        this->matriz.push_back(Z2_matrix_row(Z2_poly<I>(s), var_id, Z2_poly<I>("0")));
        var_id--;
    }

    //std::sort(this->matriz.begin(), this->matriz.end(), [](Z2_matrix_row<I> a, Z2_matrix_row<I> b) {return a < b;});

}

template <std::unsigned_integral I>
void Z2_Linear_Solver<I>::solve_linear(){
    typename std::vector<Z2_matrix_row<I>>::iterator iter = this->matriz.begin();
    typename std::vector<Z2_matrix_row<I>>::iterator next_iter;

    std::vector<uint32_t> restantes;
    uint32_t grados = this->matriz.size() - 1;

    while(iter != this->matriz.end()){
        std::sort(iter, this->matriz.end(), [](Z2_matrix_row<I> a, Z2_matrix_row<I> b) {return a > b;});

        std::cout << "Grado del polinomio apuntado: " << iter->poli_grado() << std::endl;

        while(grados > iter->poli_grado()){
            std::cout << "Falta grado -> " << grados << std::endl;
            restantes.push_back(grados);
            grados--;
        }

        if(iter->poli_grado() == 0){
            Z2_poly<I> unidad("1");
            uint32_t simbolo = 0;
            for(uint32_t g : restantes){

                std::cout << "Introducido grado -> " << g << std::endl;
                iter->substitute_row(unidad.productByMonomial(g), unidad.productByMonomial(simbolo));
                simbolo++;
                ++iter;
            }

            if(iter != this->matriz.end()){
                iter->substitute_row(unidad, unidad.productByMonomial(simbolo));
                ++iter;
            }

            for(Z2_matrix_row<I> e : this->matriz){
                std::cout << e.to_string() << std::endl;
            }

            std::cout << "- - - - - - -" << std::endl;

        }else{

            grados--;

            next_iter = std::next(iter);

            for(Z2_matrix_row<I> e : this->matriz){
                std::cout << e.to_string() << std::endl;
            }

            std::cout << "- - - - - - -" << std::endl;

            while(next_iter != this->matriz.end()){
                if(next_iter->poli_grado() == iter->poli_grado()){
                    *next_iter = *next_iter + *iter;
                    ++next_iter;
                }else{
                    next_iter = this->matriz.end();
                }    
            }

            ++iter;

            for(Z2_matrix_row<I> e : this->matriz){
                std::cout << e.to_string() << std::endl;
            }

            std::cout << "- - - - - - -" << std::endl;
        }
    }

    std::sort(this->matriz.begin(), this->matriz.end(), [](Z2_matrix_row<I> a, Z2_matrix_row<I> b) {return a < b;});

    iter = this->matriz.begin();
    grados = this->matriz.size() - 1;

    while(iter != this->matriz.end()){
        ++iter;
    }

    for(Z2_matrix_row<I> e : this->matriz){
        std::cout << e.to_string() << std::endl;
    }
    
}

#endif