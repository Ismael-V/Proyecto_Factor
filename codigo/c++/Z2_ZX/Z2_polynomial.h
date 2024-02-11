#ifndef Z2_Z_POLYNOMIALS
#define Z2_Z_POLYNOMIALS

#include <inttypes.h>
#include <iostream>
#include <string>
#include <deque>
#include <bitset>

/*
 * Los polinomios internamente se representan en bloques de bits que van del mas significativo hacia
 * el menos significativo "00000001", "00100001" = x^8 + x^5 + 1
 * 
 */

template <std::integral I>
class Z2_poly{
    private:
    std::deque<I> polinomio;
    uint32_t grado = 0;

    //Pre: True
    //Post: Genera un polinomio vacio
    Z2_poly();

    public:

    //Pre: True
    //Post: Devuelve los bloques internos y el grado
    std::string blocks();

    //Pre: La cadena esta compuesta de 1s y 0s
    //Post: Genera un polinomio con los correspondientes elementos.
    Z2_poly(std::string cadena_de_bits);

    //Pre: True
    //Post: Multiplica el polinomio por x^n
    Z2_poly<I> productByMonomial(uint16_t n) const;

    //Pre: True
    //Post: Divide el polinomio por x^n
    Z2_poly<I> divisionByMonomial(uint16_t n) const;

    //Pre: True
    //Post: Devuelve la suma de dos polinomios en Z2_Z
    Z2_poly<I> operator+(const Z2_poly& sumando) const;

    //Pre: True
    //Post: Devuelve la resta de dos polinomios en Z2_Z
    Z2_poly<I> operator-(const Z2_poly& sumando) const{
        return *this + sumando;
    }

    //Pre: True
    //Post: Devuelve el producto de dos polinomios en Z2_Z
    Z2_poly<I> operator*(const Z2_poly& factor) const;

    //Pre: divisor != 0
    //Post: Devuelve la division entera de un polinomio por el divisor
    Z2_poly<I> operator/(const Z2_poly& divisor) const;

    //Pre: divisor != 0
    //Post: Devuelve el resto de dividir un polinomio por el divisor
    Z2_poly<I> operator%(const Z2_poly& divisor) const;

    //Pre: True
    //Post: Comprueba si los polinomios son iguales
    bool operator==(const Z2_poly& otro) const;

    //Pre: True
    //Post: Comprueba si los polinomios son distintos
    bool operator!=(const Z2_poly& otro) const;

    //Pre: True
    //Post: Comprueba si este polinomio es mayor que el comprobado
    bool operator>(const Z2_poly& otro) const;

    //Pre: True
    //Post: Comprueba si este polinomio es mayor o igual que el comprobado
    bool operator>=(const Z2_poly& otro) const;

    //Pre: True
    //Post: Comprueba si este polinomio es menor que el comprobado
    bool operator<(const Z2_poly& otro) const;

    //Pre: True
    //Post: Comprueba si este polinomio es menor o igual que el comprobado
    bool operator<=(const Z2_poly& otro) const;

    //Pre: True
    //Post: Devuelve la derivada formal del polinomio
    Z2_poly<I> formerDerivative() const;

    //Pre: True
    //Post: Devuelve la raiz cuadrada entera del polinomio
    Z2_poly<I> sqrt() const;

    //Pre: True
    //Post: Devuelve el grado del polinomio
    uint32_t poli_grado() const;

    //Pre: True
    //Post: Devuelve si el polinomio es 0
    uint8_t isZero() const;

    //Pre: True
    //Post: Devuelve el polinomio en formato a_0 + a_1*x + a_2*x^2... en una cadena
    std::string to_string() const;

    //Pre: True
    //Post: Devuelve el polinomio como una cadena binaria de 1s y 0s
    std::string to_binary_representation() const;
};

//Pre: True
//Post: Envia un mensaje de control
void log_message(std::string message){
    std::cout << message << "\n";
    std::cout.flush();
}

//Pre: True
//Post: Devuelve la posicion del bit mas significativo
template<std::integral I>
uint8_t leadingPosition(I num){

    //La posicion mas significativa
    uint8_t leadingPos = 0;

    //Mientras que al dividir por 2 no se haga cero
    while(num >>= 1){

        //Suma 1 al valor
        leadingPos++;
    }

    //Devuelve la posicion del bit mas significativo
    return leadingPos;
}

//Pre: True
//Post: Devuelve el grado de un polinomio escrito como cadena binaria
uint32_t degreeOfBinaryPolinomialStr(std::string poli){

    //Calculamos la longitud de la cadena
    uint32_t longitud = poli.length();

    //Para cada elemento de la cadena
    for(uint32_t i = 0; i < longitud; i++){

        //Si encontramos el primer 1
        if(poli[i] == '1'){

            //El grado es la longitud menos el indice i menos 1
            return longitud - i - 1;
        }
    }

    //Si no hay 1s el grado es 0
    return 0;
}

//Pre: True
//Post: Devuelve el numero en su representacion binaria
template<std::integral I>
std::string numToBinaryString(I num){

    //Devolvemos el numero convertido en un bitset del tamanyo del tipo de dato
    return std::bitset<sizeof(I)*8>(num).to_string();
}

//Pre: bin_string.length() <= sizeof(I)*8
//Post: Devuelve el numero representado por la cadena binaria dada
template<std::integral I>
I binaryStringToNum(std::string bin_string){

    //Devuelve el numero que representa una cadena de bits
    return static_cast<I>(std::bitset<sizeof(I)*8>(bin_string).to_ullong());
}

//Pre: True
//Post: Devuelve el grado del polinomio
template <std::integral I>
uint32_t Z2_poly<I>::poli_grado() const{
    return this->grado;
}

//Pre: True
//Post: Devuelve si el polinomio es 0
template <std::integral I>
uint8_t Z2_poly<I>::isZero() const{
    return (this->grado == 0 && this->polinomio[0] == 0);
}

//Pre: True
//Post: Devuelve el maximo comun divisor de dos polinomios
template <std::integral I>
Z2_poly<I> mcd(Z2_poly<I> p, Z2_poly<I> q){

    //Declaramos tres polinomios
    Z2_poly<I> x("0"), y("0"), z("0");

    //Los colocamos de tal forma que en x quede el de mayor grado y en y el de menor grado
    if(p.poli_grado() >= q.poli_grado()){
        x = p;
        y = q;
    }else{
        x = q;
        y = p;
    }

    //Calculamos iterativamente mediante la identidad mcd(p, q) = mcd(q, p % q)
    while(y.poli_grado() != 0){
        z = x;
        x = y;
        y = z % y;
    }

    //Si el polinomio es 0 entonces el mcd es el resultado en x
    if(y.isZero()){
        return x;
    }

    //Sino el resultado sera 1, coprimos.
    return y;
}

//Pre: True
//Post: Genera el polinomio 0
template <std::integral I>
Z2_poly<I>::Z2_poly(){

    //El grado de este polinomio es 0
    this->grado = 0;

}

//Pre: True
//Post: Devuelve los bloques internos y el grado
template <std::integral I>
std::string Z2_poly<I>::blocks(){
    std::string blocks = "";
    uint8_t first = 1;

    for(I b : this->polinomio){
        if(first){
            blocks += numToBinaryString(b);
            first = 0;
        }else{
            blocks += " : " + numToBinaryString(b);
        }
    }

    blocks += " | grado --> " + std::to_string(this->grado) + "\n";

    return blocks;
}

//Pre: True
//Post: Devuelve el polinomio en formato a_0 + a_1*x + a_2*x^2... en una cadena
template <std::integral I>
std::string Z2_poly<I>::to_string() const{

    //Declaramos la representacion
    std::string representacion = "";
   
    //Declaramos un token
    std::string token = "";

    //Declaramos el inicio del patron
    uint8_t inicio = 1;

    //Alineamos el grado para sacar los monomios
    uint32_t aligned_degree = this->grado + sizeof(I)*8 - (this->grado % (sizeof(I)*8)) - 1;

    //Para cada bloque de nuestro polinomio
    for(I block : this->polinomio){

        //Sacamos el token
        token = numToBinaryString(block);

        //Para cada elemento del token
        for(uint8_t i = 0; i < sizeof(I)*8; i++){

            //Si vale 1 formateamos el polinomio como corresponda
            if(token[i] == '1'){

                if(inicio){

                    if(aligned_degree > 1){
                        representacion += "x^" + std::to_string(aligned_degree);
                    }else if(aligned_degree == 1){
                        representacion += "x";
                    }else{
                        representacion += "1";
                    }
                    inicio = 0;
                }else{
                    if(aligned_degree > 1){
                        representacion += " + x^" + std::to_string(aligned_degree);
                    }else if(aligned_degree == 1){
                        representacion += " + x";
                    }else{
                        representacion += " + 1";
                    }
                    
                }
            }

            //Vamos restando el grado alineado
            aligned_degree--;
        }
    }

    //Si la representacion esta vacia, es el polinomio vacio
    if(representacion == ""){
        return "0";
    }

    //Devolvemos la representacion
    return representacion;
}

//Pre: True
//Post: Devuelve el polinomio como una cadena binaria de 1s y 0s
template <std::integral I>
std::string Z2_poly<I>::to_binary_representation() const{

    //Generamos un buffer para reconstruir la cadena
    std::string polinomio_binario = "";

    //Si esta vacio el polinomio
    if(this->polinomio.size() == 0){
        return polinomio_binario;
    }

    //Para cada elemento en el vector
    //I block : this->polinomio
    for (typename std::deque<I>::const_iterator iter = this->polinomio.begin(); iter != this->polinomio.end(); ++iter ){

        //std::cout << numToBinaryString(*iter) + " ";

        //Añadimos el bloque a la cadena correspondiente
        polinomio_binario += numToBinaryString(*iter);
    }

    //std::cout << std::endl;

    //std::cout << polinomio_binario << std::endl;

    //Sacamos el polinomio quitando los 0s sobrantes de delante
    return polinomio_binario.substr(sizeof(I)*8 - leadingPosition(*this->polinomio.begin()) - 1, this->grado + 1); 
}

//Pre: True
//Post: Multiplica el polinomio por x^n
template <std::integral I>
Z2_poly<I> Z2_poly<I>::productByMonomial(uint16_t n) const{

    //Si el monomio es simplemente 1 o el polinomio es el 0 devolvemos el mismo polinomio
    if(n == 0 || (this->grado == 0 && this->polinomio[0] == 0)){
        return *this;
    }

    //Declaramos un hueco
    I hueco = 0;

    //Declaramos el numero de bloques nulos a añadir
    uint32_t blocks = n / (sizeof(I)*8);

    //Declaramos el shift que no es trivial
    uint8_t non_trivial_shift = n % (sizeof(I)*8);

    //Generamos un resultado y dos iteradores, uno de ellos mira una posicion delante del otro
    Z2_poly<I> resultado;
    typename std::deque<I>::const_iterator iter = this->polinomio.begin();
    typename std::deque<I>::const_iterator next_iter = std::next(iter);

    //Mientras que haya un shift no trivial
    if(non_trivial_shift != 0){

        //El hueco es como haber realizado el desplazamiento sizeof(I)*8 - non_trivial_shift
        hueco = *iter >> (sizeof(I)*8 - non_trivial_shift);

        //Si el hueco no es nulo
        if(hueco != 0){

            //Lo añadimos al resultado
            resultado.polinomio.push_back(hueco);
        }

        //Para el resto de elementos
        for(; iter != this->polinomio.end(); ++iter, ++next_iter){

            //Hacemos un shift a izquierdas como uno espera
            hueco = *iter << non_trivial_shift;

            //En caso de que haya elemento siguiente
            if(next_iter != this->polinomio.end()){

                //Rellenamos lo que falta de la misma forma que en el anterior caso
                hueco |= *next_iter >> (sizeof(I)*8 - non_trivial_shift);
            }

            //Lo añadimos al resultado
            resultado.polinomio.push_back(hueco);
        }

    //Si el shift es trivial
    }else{
        //Clonamos el polinomio al otro lado
        resultado.polinomio = this->polinomio;
    }

    //Para los desplazamientos triviales añadimos el numero de bloques que toque
    for(uint32_t b = 0; b < blocks; b++){
        resultado.polinomio.push_back(0);
    }

    //El grado es el original mas el del monomio añadido
    resultado.grado = this->grado + n;

    //Devolvemos el resultado
    return resultado;
}

//Pre: True
//Post: Divide el polinomio por x^n
template <std::integral I>
Z2_poly<I> Z2_poly<I>::divisionByMonomial(uint16_t n) const{

    //Si el monomio es simplemente 1 o el polinomio es el 0 devolvemos el mismo polinomio
    if(n == 0 || (this->grado == 0 && this->polinomio[0] == 0)){
        return *this;
    }

    //Calculamos el tamaño de nuestro polinomio en bloques
    uint32_t size_poli = this->polinomio.size();

    //Declaramos el numero de bloques a borrar
    uint32_t blocks = n / (sizeof(I)*8);

    //Declaramos el shift que no es trivial
    uint8_t non_trivial_shift = n % (sizeof(I)*8);

    //Generamos un resultado
    Z2_poly<I> resultado = *this;

    //std::cout << resultado.blocks();

    //Si quedan bloques por borrar
    while(blocks > 0){

        //Si no se han acabado los bloques
        if(size_poli > 1){

            //Borramos el ultimo elemento
            resultado.polinomio.pop_back();

            //std::cout << resultado.blocks();

            //Eliminamos 1 al numero de bloques del polinomio
            size_poli--;
        }else{

            //Si nos quedamos sin bloques, es el polinomio 0
            return Z2_poly("0");
        }

        //Eliminamos 1 al numero de bloques borrados
        blocks--;
        
    }

    //Si hay un shift no trivial
    if(non_trivial_shift != 0){

        //Generamos dos iteradores reversos, uno que apunte a la siguiente posicion
        typename std::deque<I>::reverse_iterator iter = resultado.polinomio.rbegin();
        typename std::deque<I>::reverse_iterator next_iter = std::next(iter);

        //Para cada bloque restante de nuestro polinomio
        for(; iter != resultado.polinomio.rend(); ++iter, ++next_iter){

            //Aplicamos el shift a derechas
            *iter = *iter >> non_trivial_shift;

            //std::cout << resultado.blocks();

            //Si hay un elemento en la siguiente posicion
            if(next_iter != resultado.polinomio.rend()){

                //Realizamos el shift contrario y hacemos una o logica sobre el iterador
                *iter |= *next_iter << (sizeof(I)*8 - non_trivial_shift);

                //std::cout << resultado.blocks();
            }

        }

        //Si el size es mayor que 1 y el primer bloque se quedo a 0, lo borramos
        if(size_poli > 1 && resultado.polinomio[0] == 0){
            resultado.polinomio.pop_front();

            //std::cout << resultado.blocks();
        }

    }

    //El grado es n veces menor
    if(resultado.grado >= n){
        resultado.grado -= n;
    }else{
        resultado.grado = 0;
    }

    //std::cout << resultado.blocks();
    
    //Devolvemos el resultado
    return resultado;
}

//Pre: La cadena esta compuesta de 1s y 0s
//Post: Genera un polinomio con los correspondientes elementos.
template <std::integral I>
Z2_poly<I>::Z2_poly(std::string cadena_de_bits){

    //Calculamos el grado a partir del numero en formato de cadena binaria
    this->grado = degreeOfBinaryPolinomialStr(cadena_de_bits);

    //std::cout << this->grado << std::endl;

    //Calculamos la longitud y el sobrante dependiendo del tamaño de los bloques
    uint32_t longitud = cadena_de_bits.length();
    uint8_t sobrante = longitud % (sizeof(I)*8);

    //Declaramos un numero del tamaño concreto, un token y un detector de patron
    I num;
    std::string token;
    uint8_t pattern = 0;

    //Si sobra algo
    if(sobrante != 0){

        //Extraemos el sobrante
        token = cadena_de_bits.substr(0, sobrante);

        //std::cout << token << " ";

        //Lo convertimos a numero
        num = binaryStringToNum<I>(token);

        //Si no es nulo o el grado es 0 --> es el polinomio 0
        if(num != 0 || this->grado == 0){

            //Introducimos el elemento con un bloque asignado para el solo
            this->polinomio.push_back(num);

            //El resto que queda es polinomio
            pattern = 1;
        }
        
    }

    //Para el resto de la cadena
    for(uint32_t i = sobrante; i < longitud; i += sizeof(I)*8){

        //Tomamos un token del tamaño de bloque en cuestion
        token = cadena_de_bits.substr(i, sizeof(I)*8);

        //std::cout << token << " ";

        //Lo convertimos a numero
        num = binaryStringToNum<I>(token);

        //Si pertenece al patron o no es nulo
        if(pattern || num != 0 ){
            
            //Introducimos el elemento
            this->polinomio.push_back(num);

            //El resto que queda es polinomio
            pattern = 1;
        }
    }

    //std::cout << std::endl;   
}

//Pre: True
//Post: Devuelve la suma de dos polinomios en Z2_Z
template<std::integral I>
Z2_poly<I> Z2_poly<I>::operator+(const Z2_poly<I>& sumando) const{

    //Calculamos la cantidad de elementos bloque que posee cada polinomio
    uint32_t size_x = this->polinomio.size();
    uint32_t size_y = sumando.polinomio.size();

    //Generamos dos iteradores sobre ambos polinomios
    typename std::deque<I>::const_iterator iter_x = this->polinomio.begin();
    typename std::deque<I>::const_iterator iter_y = sumando.polinomio.begin();

    //Generamos un resultado
    Z2_poly<I> resultado;

    //std::cout << size_x << ", " << size_y << std::endl;

    //Si hay mas elementos en nuestro polinomio que en el sumando
    if(size_x > size_y){

        //El grado es el de nuestro polinomio
        resultado.grado = this->grado;

        //Mientras el numero de elementos de nuestro polinomio exceda los del sumando
        while(size_x > size_y){

            //Añadimos cada elemento de nuestro polinomio
            resultado.polinomio.push_back(*iter_x);

            //Avanzamos nuestro iterador
            ++iter_x;

            //Restamos un elemento
            size_x--;
        }

        //Mientras que queden elementos por sumar
        for(; iter_x != this->polinomio.end(); ++iter_x, ++iter_y){

            //Realizamos la suma con una XOR
            resultado.polinomio.push_back((*iter_x) ^ (*iter_y));
        }

    //Si hay mas elementos en el sumando que en nuestro polinomio    
    }else if(size_y > size_x){

        //El grado es el del sumando
        resultado.grado = sumando.grado;

        //Mientras el numero de elementos del sumando exceda el de nuestro polinomio
        while(size_y > size_x){

            //Añadimos cada elemento del sumando
            resultado.polinomio.push_back(*iter_y);

            //Avanzamos el iterador del sumando
            ++iter_y;

            //Restamos un elemento
            size_y--;
        }

        //Mientras que queden elementos por sumar
        for(; iter_y != sumando.polinomio.end(); ++iter_x, ++iter_y){

            //Realizamos la suma con una XOR
            resultado.polinomio.push_back((*iter_y) ^ (*iter_x));
        }
    
    //Si tienen el mismo numero de elementos
    }else{

        //Mientras que queden elementos por sumar
        for(; iter_x != this->polinomio.end(); ++iter_x, ++iter_y){

            //std::cout << numToBinaryString<I>((*iter_x)) << " xor " << numToBinaryString<I>((*iter_y)) << " --> " << numToBinaryString<I>((*iter_x) ^ (*iter_y));
            //std::cout << " - ";

            //Realizamos la suma con una XOR
            resultado.polinomio.push_back((*iter_x) ^ (*iter_y));
        }

        //std::cout << resultado.to_binary_representation() << std::endl;

        //Sacamos un iterador al principio del resultado
        typename std::deque<I>::iterator iter_r = resultado.polinomio.begin();

        //Mientras queden bloque vacios
        while(*iter_r == 0 && size_x > 1){

            //std::cout << numToBinaryString(*iter_x) + " ";

            //Avanzamos el iterador
            ++iter_r;

            //Eliminamos el elemento vacio
            resultado.polinomio.pop_front();

            //Reducimos en 1 el numero de bloques restantes
            size_x--;
        }

        //El grado es la suma del coeficiente director mas la posicion del coeficiente mas significativo
        resultado.grado = leadingPosition(*iter_r) + (size_x - 1)*sizeof(I)*8;

    }

    //Devolvemos el resultado
    return resultado;
}

//Pre: True
//Post: Devuelve el producto de dos polinomios en Z2_Z
template<std::integral I>
Z2_poly<I> Z2_poly<I>::operator*(const Z2_poly& factor) const{

    //Generamos el polinomio 0
    Z2_poly<I> product("0");

    //Declaramos un iterador a nuestro polinomio
    typename std::deque<I>::const_iterator iter = this->polinomio.begin();

    //Declaramos un token
    std::string token = "";

    //Alineamos el grado para realizar el producto
    uint32_t aligned_degree = this->grado + sizeof(I)*8 - (this->grado % (sizeof(I)*8)) - 1;

    //Para cada elemento de nuestro polinomio
    for(; iter != this->polinomio.end(); ++iter){

        //Convertimos el elemento en una cadena de bits
        token = numToBinaryString(*iter);

        //std::cout << token << "\n";

        //Para cada elemento de la cadena de bits
        for(uint8_t i = 0; i < sizeof(I)*8; i++){

            //std::cout << aligned_degree << std::endl;

            //Si encontramos un bit a 1
            if (token[i] == '1'){
                //std::cout << factor.productByMonomial(aligned_degree).to_binary_representation() << std::endl;

                //Sumamos el factor multiplicado por el monomio adecuado
                product = product + factor.productByMonomial(aligned_degree);

                //std::cout << factor.productByMonomial(aligned_degree).to_binary_representation() << std::endl;
                
                //std::cout << product.to_binary_representation() << std::endl;
            }

            //Reducimos en 1 el grado
            aligned_degree--;
        }
    }

    //El grado del nuevo polinomio es la suma de grados
    product.grado = this->grado + factor.grado;
    
    //Devolvemos el resultado
    return product;
}

//Pre: divisor != 0
//Post: Devuelve la division entera de un polinomio por el divisor
template<std::integral I>
Z2_poly<I> Z2_poly<I>::operator/(const Z2_poly& divisor) const{

    //Declaramos un dividendo, una unidad y un resultado
    Z2_poly<I> dividendo = *this;
    Z2_poly<I> unidad("1");
    Z2_poly<I> resultado("0");

    //Declaramos el grado del dividendo y del divisor
    uint32_t grado_dividendo = dividendo.grado;
    uint32_t grado_divisor = divisor.grado;

    //Mientras que el grado del dividendo sea mayor al del divisor
    while(grado_dividendo >= grado_divisor){

        //Restamos lo que podamos con el polinomio pequeño
        dividendo = dividendo - divisor.productByMonomial(grado_dividendo - grado_divisor);

        //Añadimos al cociente la cantidad adecuada
        resultado = resultado + unidad.productByMonomial(grado_dividendo - grado_divisor);

        //Calculamos el nuevo grado de nuestro polinomio
        grado_dividendo = dividendo.grado;
    }

    //Devolvemos el cociente
    return resultado;
}

//Pre: divisor != 0
//Post: Devuelve el resto de dividir un polinomio por el divisor
template <std::integral I>
Z2_poly<I> Z2_poly<I>::operator%(const Z2_poly& divisor) const{
    //Declaramos un dividendo, una unidad y un resultado
    Z2_poly<I> dividendo = *this;

    //Declaramos el grado del dividendo y del divisor
    uint32_t grado_dividendo = dividendo.grado;
    uint32_t grado_divisor = divisor.grado;

    //Mientras que el grado del dividendo sea mayor o igual al del divisor
    while(grado_dividendo >= grado_divisor){

        //Restamos lo que podamos con el polinomio pequeño
        dividendo = dividendo - divisor.productByMonomial(grado_dividendo - grado_divisor);

        //Calculamos el nuevo grado de nuestro polinomio
        grado_dividendo = dividendo.grado;
    }

    //Devolvemos el resto
    return dividendo;
}

//Pre: True
//Post: Comprueba si los polinomios son iguales
template <std::integral I>
bool Z2_poly<I>::operator==(const Z2_poly& otro) const{

    //Si el grado no coincide
    if(this->grado != otro.grado){

        //No son iguales
        return false;
    }

    //Declaramos un iterador a nuestro polinomio
    typename std::deque<I>::const_iterator iter_x = this->polinomio.begin();

    //Declaramos un iterador a otro polinomio
    typename std::deque<I>::const_iterator iter_y = otro.polinomio.begin();

    //Para cada elemento en ambos vectores
    for(; iter_x != this->polinomio.end(); ++iter_x, ++iter_y){

        //Si alguno es distinto
        if(*iter_x != *iter_y){

            //No son iguales
            return false;
        }
    }

    //Son iguales
    return true;
}

//Pre: True
//Post: Comprueba si los polinomios son distintos
template <std::integral I>
bool Z2_poly<I>::operator!=(const Z2_poly& otro) const{
    
    //Si el grado no coincide
    if(this->grado != otro.grado){

        //No son iguales
        return true;
    }

    //Declaramos un iterador a nuestro polinomio
    typename std::deque<I>::const_iterator iter_x = this->polinomio.begin();

    //Declaramos un iterador a otro polinomio
    typename std::deque<I>::const_iterator iter_y = otro.polinomio.begin();

    //Para cada elemento en ambos vectores
    for(; iter_x != this->polinomio.end(); ++iter_x, ++iter_y){

        //Si alguno es distinto
        if(*iter_x != *iter_y){

            //No son iguales
            return true;
        }
    }

    //Son iguales
    return false;

}

//Pre: True
//Post: Comprueba si este polinomio es mayor que el comprobado
template <std::integral I>
bool Z2_poly<I>::operator>(const Z2_poly& otro) const{

    //Si el grado de nuestro polinomio es mayor que el otro
    if(this->grado > otro.grado){

        //Es mayor
        return true;

    //Si el grado de nuestro polinomio es menor que el otro
    }else if(this->grado < otro.grado){

        //Es menor
        return false;
    }

    //Declaramos un iterador a nuestro polinomio
    typename std::deque<I>::const_iterator iter_x = this->polinomio.begin();

    //Declaramos un iterador a otro polinomio
    typename std::deque<I>::const_iterator iter_y = otro.polinomio.begin();

    //Para cada elemento en ambos vectores
    for(; iter_x != this->polinomio.end(); ++iter_x, ++iter_y){

        //Si el bloque de nuestro polinomio es mayor que el otro
        if(*iter_x > *iter_y){

            //Nuestro polinomio es mayor que el otro
            return true;
        
        //Si el bloque de nuestro polinomio es menor que el otro
        }else if(*iter_x < *iter_y){

            //Es menor
            return false;
        }

    }

    //Sino son iguales
    return false;
}

//Pre: True
//Post: Comprueba si este polinomio es mayor o igual que el comprobado
template <std::integral I>
bool Z2_poly<I>::operator>=(const Z2_poly& otro) const{

    //Si el grado de nuestro polinomio es mayor que el otro
    if(this->grado > otro.grado){

        //Es mayor
        return true;

    //Si el grado de nuestro polinomio es menor que el otro
    }else if(this->grado < otro.grado){

        //Es menor
        return false;
    }

    //Declaramos un iterador a nuestro polinomio
    typename std::deque<I>::const_iterator iter_x = this->polinomio.begin();

    //Declaramos un iterador a otro polinomio
    typename std::deque<I>::const_iterator iter_y = otro.polinomio.begin();

    //Para cada elemento en ambos vectores
    for(; iter_x != this->polinomio.end(); ++iter_x, ++iter_y){

        //Si el bloque de nuestro polinomio es mayor que el otro
        if(*iter_x > *iter_y){

            //Nuestro polinomio es mayor que el otro
            return true;
        
        //Si el bloque de nuestro polinomio es menor que el otro
        }else if(*iter_x < *iter_y){

            //Es menor
            return false;
        }
    }

    //Sino son iguales
    return true;
}

//Pre: True
//Post: Comprueba si este polinomio es menor que el comprobado
template <std::integral I>
bool Z2_poly<I>::operator<(const Z2_poly& otro) const{

    //Si el grado de nuestro polinomio es mayor que el otro
    if(this->grado > otro.grado){

        //Es mayor
        return false;

    //Si el grado de nuestro polinomio es menor que el otro
    }else if(this->grado < otro.grado){

        //Es menor
        return true;
    }

    //Declaramos un iterador a nuestro polinomio
    typename std::deque<I>::const_iterator iter_x = this->polinomio.begin();

    //Declaramos un iterador a otro polinomio
    typename std::deque<I>::const_iterator iter_y = otro.polinomio.begin();

    //Para cada elemento en ambos vectores
    for(; iter_x != this->polinomio.end(); ++iter_x, ++iter_y){

        //Si el bloque de nuestro polinomio es mayor que el otro
        if(*iter_x > *iter_y){

            //Nuestro polinomio es mayor que el otro
            return false;
        
        //Si el bloque de nuestro polinomio es menor que el otro
        }else if(*iter_x < *iter_y){

            //Es menor
            return true;
        }
    }

    //Sino son iguales
    return false;

}

//Pre: True
//Post: Comprueba si este polinomio es menor o igual que el comprobado
template <std::integral I>
bool Z2_poly<I>::operator<=(const Z2_poly& otro) const{

    //Si el grado de nuestro polinomio es mayor que el otro
    if(this->grado > otro.grado){

        //Es mayor
        return false;

    //Si el grado de nuestro polinomio es menor que el otro
    }else if(this->grado < otro.grado){

        //Es menor
        return true;
    }

    //Declaramos un iterador a nuestro polinomio
    typename std::deque<I>::const_iterator iter_x = this->polinomio.begin();

    //Declaramos un iterador a otro polinomio
    typename std::deque<I>::const_iterator iter_y = otro.polinomio.begin();

    //Para cada elemento en ambos vectores
    for(; iter_x != this->polinomio.end(); ++iter_x, ++iter_y){

        //Si el bloque de nuestro polinomio es mayor que el otro
        if(*iter_x > *iter_y){

            //Nuestro polinomio es mayor que el otro
            return false;
        
        //Si el bloque de nuestro polinomio es menor que el otro
        }else if(*iter_x < *iter_y){

            //Es menor
            return true;
        }
    }

    //Sino son iguales
    return true;

}

//Pre: True
//Post: Devuelve la derivada formal del polinomio
template <std::integral I>
Z2_poly<I> Z2_poly<I>::formerDerivative() const{

    //Generamos el resultado
    Z2_poly<I> derivada = this->divisionByMonomial(1);

    //std::cout << derivada.blocks();

    //Calculamos el numero de bloques
    uint32_t size_poli = derivada.polinomio.size(); 

    //Usamos un patron para borrar factores impares
    uint8_t patron = 0b01010101;
    I mascara = 0;

    //Generamos la mascara especifica
    for(uint8_t i = 0; i < sizeof(I); i++){
        mascara |= patron << (i << 3);
    }

    //Declaramos un iterador a nuestro polinomio
    typename std::deque<I>::iterator iter = derivada.polinomio.begin();

    //Aplicamos la mascara a cada elemento
    for(; iter != derivada.polinomio.end(); ++iter){
        *iter &= mascara;
        //std::cout << derivada.blocks();
    }

    //Borramos los bloques nulos
    iter = derivada.polinomio.begin();

    while(*iter == 0 && size_poli > 1){
        ++iter;
        derivada.polinomio.pop_front();
        //std::cout << derivada.blocks();
        size_poli--;
    }
    
    //El grado es la suma del coeficiente director mas la posicion del coeficiente mas significativo
    derivada.grado = leadingPosition(*iter) + (size_poli - 1)*sizeof(I)*8;
    //std::cout << derivada.blocks();

    //Devolvemos la derivada formal
    return derivada;
}

//Pre: True
//Post: Devuelve la raiz cuadrada entera del polinomio
template <std::integral I>
Z2_poly<I> Z2_poly<I>::sqrt() const{

    //Sacamos el grado alineado y el ultimo indice
    uint32_t aligned_degree = this->grado + sizeof(I)*8 - (this->grado % (sizeof(I)*8)) - 1;
    uint32_t last_index = aligned_degree;

    //Generamos una cadena vacia de 0s
    std::string binarySqrt(last_index + 1, '0');

    //Generamos un token
    std::string token = "";

    //Para cada bloque en el polinomio
    for(I b : this->polinomio){

        //Tokenizamos el bloque
        token = numToBinaryString(b);

        //Para cada elemento del token
        for(uint8_t i = 0; i < sizeof(I)*8; i++){

            //Si uno de los elementos es 1
            if(token[i] == '1'){

                //Si el grado alineado es impar
                if(aligned_degree % 2 == 1){

                    //Rellenamos de 1s hasta la mitad del grado alineado
                    for(uint32_t j = 0; j < aligned_degree/2 + 1; j++){
                        binarySqrt[last_index - j] = '1';
                    }

                    //Devolvemos el resultado
                    return Z2_poly(binarySqrt);

                //De lo contrario
                }else{

                    //Ponemos a 1 el elemento de grado mitad
                    binarySqrt[last_index - aligned_degree/2] = '1';
                }
            }

            //Decrementamos en 1 el grado alineado
            aligned_degree--;
        }
    }

    //Devolvemos el polinomio resultante
    return Z2_poly(binarySqrt);
}

#endif