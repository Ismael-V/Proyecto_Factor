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

    //Valor de los simbolos
    Z2_poly<I> symbols;

    public:

    //Inicializamos la fila de la matriz con la fila y los simbolos
    Z2_matrix_row(Z2_poly<I> fila, Z2_poly<I> simbolos) : row(fila), symbols(simbolos) {}

    //Pre: True
    //Post: Realiza la suma de dos filas
    Z2_matrix_row<I> operator+(const Z2_matrix_row<I>& sumando) const{
        return Z2_matrix_row(this->row + sumando.row, this->symbols + sumando.symbols);
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

    //Pre: True
    //Post: Devuelve el simbolo en la posicion indicada
    bool operator[](std::size_t index) const{
        return symbols[index];
    }

    //Pre: True
    //Post: Devuelve si la fila de la matriz es nula
    bool isZero() const{
        return this->row.isZero();
    }

    //Pre: True
    //Post: Devuelve si la fila contiene simbolos
    bool hasSymbols() const{
        return !this->symbols.isZero();
    }

    //Pre: True
    //Post: Sustituye el contenido de la fila asi como de los simbolos que contenga
    void substitute_row(const Z2_poly<I>& fila, const Z2_poly<I>& simbolo){
        this->row = fila;
        this->symbols = simbolo;
    }

    //Pre: True
    //Post: Devuelve la informacion relevante de la fila
    std::string to_string() const{
        return this->row.to_string() + " => " + this->symbols.to_string();
    }

    //Pre: True
    //Post: Devuelve el "grado" de la fila
    uint32_t poli_grado() const{
        return row.poli_grado();
    }

    //Pre: True
    //Post: Devuelve la posicion del primer simbolo
    uint32_t first_symbol() const{
        return symbols.poli_grado();
    }

    //Pre: True
    //Post: Devuelve el "minimo grado" de la fila
    uint32_t min_grado() const{
        return row.min_grado();
    }

};

template <std::unsigned_integral I>
class Z2_Linear_Solver{
    std::vector<Z2_matrix_row<I>> matriz;

    Z2_poly<I> generador;

    public:

    Z2_Linear_Solver(Z2_poly<I> generator);

    void printMatrix() const;

    //Pre: El polinomio con el que fue generado debe ser libre de cuadrados
    //Post: Devuelve, de ser reducible, un factor no trivial del polinomio, de lo contrario devuelve 1
    Z2_poly<I> solve_linear();
};

template <std::unsigned_integral I>
void Z2_Linear_Solver<I>::printMatrix() const{

    std::cout << "- - - - - - -" << std::endl;

    for(Z2_matrix_row<I> e : this->matriz){
        std::cout << e.to_string() << std::endl;
    }

    std::cout << "- - - - - - -" << std::endl;
}

template <std::unsigned_integral I>
Z2_Linear_Solver<I>::Z2_Linear_Solver(Z2_poly<I> generator) : generador(generator) {

    //std::cout << generator.to_string() << std::endl;

    //Sacamos el grado del polinomio
    uint32_t grado = generator.poli_grado();

    //Generamos un vector de polinomios denominado potencias y una unidad
    std::vector<Z2_poly<I>> potencias;
    Z2_poly<I> unidad("1");

    //Para cada elemento de grado inferior al del polinomio generador
    for(int32_t i = (grado - 1); i >= 0; i--){

        //std::cout << (unidad.productByMonomial(i << 1) % generator).to_string() << std::endl;

        //Calculamos las potencias del monomio (x^i)^2 modulo el generador
        potencias.push_back((unidad.productByMonomial(i << 1) % generator));
        
    }

    //Declaramos una cadena de caracteres de tamaño grado
    std::vector<std::string> matrizBinaria(grado, "");
 
    //Declaramos la posicion de la identidad
    int32_t identity = grado - 1;

    //Para cada una de las potencias generadas
    for(Z2_poly<I> p : potencias){

        //std::cout << p.to_string() << std::endl;

        //Para cada uno de los monomios no nulos del polinomio
        for(int32_t g = (grado - 1), g_inv = 0; g >= 0; g--, g_inv++){

            //Si el grado no esta en la diagonal
            if(g != identity){

                //Si hay un elemento no nulo
                if(p[g]){

                    //Escribimos un 1
                    matrizBinaria[g_inv] += '1';

                //Si no
                }else{

                    //Escribimos un 0
                    matrizBinaria[g_inv] += '0';
                }
            //De lo contrario habra que sumar 1 al elemento y por tanto
            }else{

                //Si hay un elemento no nulo
                if(p[g]){

                    //Escribimos un 0
                    matrizBinaria[g_inv] += "0";

                //Si no
                }else{

                    //Escribimos un 1
                    matrizBinaria[g_inv] += "1";
                }
            }
        }

        //Restamos uno a la identidad para comprobar la diagonal
        identity--;

    }

    //Para cada fila de la matriz
    for(std::string filaMatriz : matrizBinaria){

        //Introducimos el elemento en nuestra matriz
        this->matriz.push_back(Z2_matrix_row(Z2_poly<I>(filaMatriz), Z2_poly<I>("0")));
    }
}

template <std::unsigned_integral I>
Z2_poly<I> Z2_Linear_Solver<I>::solve_linear(){

    //this->printMatrix();

    Profiler ("Solver");

    //Declaramos un iterador y el iterador siguiente
    typename std::vector<Z2_matrix_row<I>>::iterator iter = this->matriz.begin();
    typename std::vector<Z2_matrix_row<I>>::iterator next_iter;

    //Declaramos un vector de elementos que faltan para resolver el sistema
    std::vector<uint32_t> restantes;

    //Declaramos el grado maximo del sistema
    int32_t grados = this->matriz.size() - 1;

    //Mientras que no lleguemos al final de la matriz
    while(iter != this->matriz.end()){

        //Ordena por grado las filas de la misma
        std::sort(iter, this->matriz.end(), [](Z2_matrix_row<I> a, Z2_matrix_row<I> b) {return a > b;});

        //this->printMatrix();
        //std::cout << "Grado del polinomio apuntado: " << iter->poli_grado() << std::endl;
        //std::cout << "Grado target: " << grados << std::endl;

        //Mientras que el grado del polinomio ahora apuntado sea menor a grados
        while(grados > (int32_t)iter->poli_grado()){
            //std::cout << "Falta grado -> " << grados << std::endl;

            //Significa que falta el grado indicado por grados, asi que lo añadimos
            restantes.push_back(grados);

            //Restamos en 1 grados
            grados--;
        }

        //Si la fila esta a cero
        if(iter->isZero()){

            //Declaramos una unidad y el simbolo 0
            Z2_poly<I> unidad("1");
            uint32_t simbolo = 0;

            //Para los grados que nos faltan por colocar
            for(uint32_t g : restantes){

                //std::cout << "Introducido grado -> " << g << std::endl;

                //Introducimos los grados restantes
                iter->substitute_row(unidad.productByMonomial(g), unidad.productByMonomial(simbolo));

                //this->printMatrix();

                //Aumentamos el numero de simbolo y avanzamos el iterador
                simbolo++;
                ++iter;
            }

            //Si aun no se ha alcanzado el final de la matriz es que nos falta añadir el elemento de grado 0
            if(iter != this->matriz.end()){

                //Añadimos el elemento de grado 0, la unidad
                iter->substitute_row(unidad, unidad.productByMonomial(simbolo));

                //this->printMatrix();
            }

        //Si aun no se ha alcanzado el polinomio nulo
        }else{

            //Colocamos el siguiente iterador en la siguiente posicion
            next_iter = std::next(iter);

            //Mientras que queden filas por debajo de la actual
            while(next_iter != this->matriz.end()){

                //Si su grado coincide
                if(next_iter->poli_grado() == iter->poli_grado()){

                    //Sumalas para reducir el sistema
                    *next_iter = *next_iter + *iter;

                    //Avanza el iterador
                    ++next_iter;

                //Sino hemos acabado este paso
                }else{

                    //Ponemos el iterador al final de la matriz
                    next_iter = this->matriz.end();
                }    
            }

            //Restamos en 1 grados y avanzamos el iterador
            if(grados > 0) grados--;
            ++iter;

            //this->printMatrix();
        }

        
    }

    //Ordenamos de menor a mayor grado la matriz para diagonalizar
    std::sort(this->matriz.begin(), this->matriz.end(), [](Z2_matrix_row<I> a, Z2_matrix_row<I> b) {return a < b;});

    //this->printMatrix();

    //Devolvemos el iterador al inicio de la matriz y recalculamos el grado maximo del sistema
    iter = this->matriz.begin();
    grados = this->matriz.size() - 1;

    //Mientras que no se acaben las filas de la matriz
    while(iter != this->matriz.end()){

        //Apuntamos el otro iterador a la fila siguiente
        next_iter = std::next(iter);

        //Para el resto de filas apuntadas
        for(;next_iter != this->matriz.end(); ++next_iter){

            //Si el monomio de menor grado coincide con el de la fila actual
            if(next_iter->min_grado() == iter->poli_grado()){

                //Suma la fila
                *next_iter = *next_iter + *iter;

            }
        }

        //this->printMatrix();

        //Avanza el iterador
        ++iter;
    }

    //Tras esto reordenamos de mayor a menor y quedara la matriz identidad (Realmente bastaria con invertir el orden y ni eso es necesario)
    std::sort(this->matriz.begin(), this->matriz.end(), [](Z2_matrix_row<I> a, Z2_matrix_row<I> b) {return a > b;});

    //this->printMatrix();

    //Declaramos el simbolo seleccionado y el polinomio factorizador
    int32_t simbolo_seleccionado = -1;
    std::string polinomio_factorizador;
    polinomio_factorizador.reserve(this->matriz.size());

    //Para la lista de simbolos que queden
    for(Z2_matrix_row<I> fila : this->matriz){

        //Si la fila tiene simbolos
        if(fila.hasSymbols() && simbolo_seleccionado == -1){

            //Seleccionamos el indice del simbolo indicado
            simbolo_seleccionado = fila.first_symbol();
        }

        //Si el simbolo es valido y esta añadido en esa fila
        if(simbolo_seleccionado != -1 && fila[simbolo_seleccionado]){

            //Colocamos un 1
            polinomio_factorizador += "1";

        //Sino colocamos un cero
        }else{
            polinomio_factorizador += "0";
        }
    }

    //std::cout << Z2_poly<I>(polinomio_factorizador).to_string() << std::endl;

    //Devolvemos el maximo comun divisor del polinomio original y del polinomio factorizador
    return mcd(Z2_poly<I>(polinomio_factorizador), this->generador);
}


//Pre: True
//Post: Factoriza en irreducibles el polinomio dado
template<std::unsigned_integral I>
void berlekamp_factorize(Z2_poly<I> poly, std::vector<Z2_poly<I>>& irreducibles){

    Profiler ("Berlekamp");

    //Si el grado es 0 vuelve
    if(poly.poli_grado() == 0) return;

    //Calculamos la derivada y el maximo comun divisor de esta y el polinomio original
    Z2_poly<I> derivada = poly.formerDerivative();

    //std::cout << "Derivada: " << derivada.to_string() << std::endl;
    Z2_poly<I> squared_factors = mcd(poly, derivada);

    //std::cout << poly.to_string() << std::endl;
    //std::cout << derivada.to_string() << std::endl;
    //std::cout << squared_factors.to_string() << std::endl;

    //Si la derivada es 0
    if(derivada.isZero()){
        //std::cout << "Es cuadrado perfecto\n";

        //Es un cuadrado perfecto
        std::vector<Z2_poly<I>> factores;

        //Calculamos berlekamp para su raiz cuadrada
        berlekamp_factorize(poly.sqrt(), factores);

        //Insertamos por duplicado sus factores
        irreducibles.insert(irreducibles.end(), factores.begin(), factores.end());
        irreducibles.insert(irreducibles.end(), factores.begin(), factores.end());

        //Volvemos
        return;

    //Si el maximo comun divisor es 1
    }else if(squared_factors.poli_grado() == 0){
        //std::cout << "Es libre de cuadrados\n";

        //std::cout << poly.to_string() << std::endl;

        //Aplicamos el resolvedor al polinomio
        Z2_poly<I> non_trivial_factor = Z2_Linear_Solver<I>(poly).solve_linear();

        //std::cout << "El factor no trivial es: " << non_trivial_factor.to_string() << std::endl;

        //Si el factor no trivial es de grado 0 o es el polinomio (fixme: eso no deberia pasar)
        if(non_trivial_factor.poli_grado() == 0 || non_trivial_factor == poly){

            //Mete el polinomio a la lista de irreducibles
            irreducibles.push_back(poly);

            //Volvemos
            return;
        }
        
        //Si no, factorizamos el polinomio dividido por dicho factor y el propio factor
        berlekamp_factorize(poly / non_trivial_factor, irreducibles);
        berlekamp_factorize(non_trivial_factor, irreducibles);

        //Volvemos
        return;
    }

    //std::cout << "Se rompe en 2\n";

    //std::cout << (poly / squared_factors).to_string() << std::endl;
    //std::cout << (squared_factors).to_string() << std::endl;

    //Si no, es que comparten factores, simplemente factorizamos el polinomio sin factores repetidos y el libre de cuadrados
    berlekamp_factorize(poly / squared_factors, irreducibles);
    berlekamp_factorize(squared_factors, irreducibles);

    //Volvemos
    return;
}

#endif