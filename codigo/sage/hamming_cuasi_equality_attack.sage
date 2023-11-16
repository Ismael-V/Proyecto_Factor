#Clave publica a atacar
CLAVE_PUBLICA = 57669915693513710416159704256743204264182266173000458848559014940270309408921

#Longitud en numero de bits de uno de sus factores
LONGITUD_FACTOR = 128

#Declaramos un cuerpo finito de orden 5 para poder deshacer los carrys
P.<x> = GF(5)[]

#Devuelve todas las interpretaciones posibles del numero como un polinomio
def reinterpret_num_as_Z5_poly(num):

    #Escribimos el numero como una cadena binaria
    bin_num = str(num.binary())

    #Este es el grado del termino de mayor peso
    bin_num_degree_index = len(bin_num) - 1

    #Este es el indice del polinomio donde sucede el carry
    bin_num_carry_index = (len(bin_num)/2).floor()

    #Declaramos un polinomio nulo
    p = 0*x

    #Declaramos una lista con todas las posiciones donde puede haber fold
    fold_indexes = []

    #Declaramos un booleano para reconocer el patron 10
    reconoce_1 = False

    #Para cada caracter en la cadena
    for c in bin_num:

        #Si el caracter es 1
        if(c == '1'):

            if(reconoce_1):
                fold_indexes.append(bin_num_degree_index)

            #Sumamos la potencia de x correspondiente a dicha posicion
            p += x^(bin_num_degree_index)
            reconoce_1 = True

        elif(reconoce_1):
            fold_indexes.append(bin_num_degree_index)
            reconoce_1 = False

        #Reducimos en 1 el grado con cada iteracion
        bin_num_degree_index -= 1

    #Declaramos un vector donde iran todos los posibles folds
    possible_interpretations = []

    #Para cada exponente en el que puedas realizar un fold
    for e in fold_indexes:
        possible_interpretations.append(p - x^(e + 1) + 2*x^e)

    #Devolvemos el polinomio obtenido
    return possible_interpretations

#Devuelve el polinomio interpretado como un numero, indiferentemente de si tiene o no elementos que no sean 0 o 1
def reinterpret_Z5_poly_as_num(poly):

    #Definimos una cadena donde almacenar los digitos binarios
    num_str = ""

    #Definimos un indice para rellenar de ceros los huecos que hagan falta
    index = 0

    #Para cada exponente de cada termino que compone el polinomio
    for exp in sorted(poly.exponents()):

        #Mientras que el indice sea menor al exponente actual
        while index < exp:

            #Rellenamos de 0s la cadena
            num_str += "0"

            #Contamos los huecos rellenados
            index += 1

        #Colocamos el 1 donde corresponde
        num_str += "1"

        #Contamos los huecos rellenados
        index += 1

    #Si la cadena es vacia
    if(num_str == ""):
        #Devolvemos el numero 0
        return 0

    #Si no, revertimos el orden de los digitos de la cadena
    rev_num_str = ""
    num_str_len = len(num_str)

    #Para cada caracter en la cadena
    for i in range (0, num_str_len):

        #Colocamos cada caracter en donde corresponde
        rev_num_str += num_str[num_str_len - 1 - i]

    #Devolvemos la cadena interpretada como un numero binario
    return ZZ("0b" + rev_num_str)

#Genera la lista de factores irreducibles del polinomio en Z5_Z
def generate_factor_list(poly):

    #Declaramos un indice y una lista
    index = 0
    factor_list = {}

    #Factorizamos en irreducibles el polinomio
    factores = factor(poly)

    #Para cada factor en factores
    for p in factores:

        #Mientras que el contador sea menor a la multiplicidad del factor
        for mul in range(0, p[1]):

            #Anyadimos tantas veces el factor a la lista como multiplicidad tenga
            factor_list[index] = p[0]
            index += 1

    #Devolvemos la lista de factores
    return factor_list


#Devuelve, de encontrarlo, uno de los factores del numero num introducido o 0 de lo contrario, dada una guess y una lista de factores
def solve_factor_recursive(num, target_deg, guess, factors, initial_index):

    #Definimos el factor previo
    previous_factor = x^0

    #Para cada elemento en rango de los que quedan
    for index in range(initial_index, len(factors)):

        #Si el factor previo no coincide con el actual
        if(previous_factor != factors[index]):

            previous_factor = factors[index]

            #Construimos nuestra guess con el siguiente factor
            next_guess = guess * factors[index]

            #El nuevo grado de nuestra guess es la suma de la anterior y del siguiente
            next_guess_deg = guess.degree() + factors[index].degree()

            #Si coincide el grado de nuestra guess con el grado target
            if(next_guess_deg == target_deg):

                #Interpretamos la guess como un numero
                p = reinterpret_Z5_poly_as_num(next_guess)

                #Si la guess divide al numero a factorizar
                if(num % p == 0):

                    #Devolvemos el factor encontrado
                    return p
                
                #Si no, nos hemos equivocado, toca retroceder
                return 0
            #Si no, si el grado supera al grado target
            elif(next_guess_deg > target_deg):

                #Nos hemos equivocado, toca retroceder
                return 0
            
            #Si no seguimos anyadiendo terminos a nuestra solucion
            solucion = solve_factor_recursive(num, target_deg, next_guess, factors, index + 1)

            #Si la solucion no es nula la devolvemos
            if(solucion != 0):
                return solucion
            
            #Si no, toca seguir buscando
    
    #Si nos quedamos sin factores, nos hemos equivocado, toca retroceder
    return 0

#Devuelve, de encontrarlo, uno de los factores del numero num introducido o 0 de lo contrario
def solve_factor(num, target_deg):

    #Reinterpretamos el numero como un polinomio
    poly_num_interpretations = reinterpret_num_as_Z5_poly(num)

    #Probamos para cada posible interpretacion del mismo
    for poly_num in poly_num_interpretations:

        #Obtenemos su lista de factores
        factor_list = generate_factor_list(poly_num)

        #Devolvemos el resultado de no ser nulo
        solucion = solve_factor_recursive(num, target_deg, x^0, factor_list, 0)

        if(solucion != 0):
            return solucion
    
    #Si fallamos devolvemos 0
    return 0

#Este es el grado target del polinomio buscado
GRADO_TARGET = LONGITUD_FACTOR - 1

#Buscamos la solucion con la estrategia propuesta
print(solve_factor(CLAVE_PUBLICA, GRADO_TARGET))
