#Clave publica a atacar
CLAVE_PUBLICA = 1034144194281952233622876054037438462740289438733787436844034931470023906038705977954345957663367489774483940064361406739522192706994885508829376563064106727001019313473249788398561309440827507023349459678665905449836058164660666145688401634593420169647719305882911584967385985887463378837967842295754296034334493057980649506804491245259270600114547559533676369796592139230422530160879640925684737888799716336883435900676876519786754073675529738603179230459075905502942958529405947654948001899004924958733579727655935832693278040978668418665672926331169253713626855587785486628280415849149246859939803619382794084286461

#Longitud en numero de bits de uno de sus factores
LONGITUD_FACTOR = 1028

#Numero de carrys que ocurrieron
N_CARRY = 0

#Declaramos un cuerpo finito de orden 1 + 2*N_CARRY para poder deshacer los carrys
P.<x> = GF(Primes().next(1 + 2*N_CARRY))[]

#Devuelve una lista con todas las posibilidades de polinomios tras deshacer un carry en la lista de polinomios introducida
def unCarry(list_of_poly):
    #Declaramos la lista de polinomios posibles con un acarreo menos
    list_of_unCarry_polys = []

    #Para cada polinomio de la lista
    for p in list_of_poly:

        #Para cada elemento del polinomio de grado mayor a 1
        for i in range (1, p.degree() + 1):
            
            #Si el coeficiente es mayor a 1
            if p[i] > 0:

                #Anyadimos a la lista el polinomio resultante de deshacer el carry en esa posicion
                list_of_unCarry_polys.append(p - x^i + 2*x^(i - 1))
    
    #Devolvemos la lista de polinomios sin acarreo
    return list_of_unCarry_polys


#Devuelve la interpretacion del numero como un polinomio en un cuerpo finito
def reinterpret_num_as_finite_poly(num):

    #Escribimos el numero como una cadena binaria
    bin_num = str(num.binary())

    #Este es el grado del termino de mayor peso
    bin_num_degree_index = len(bin_num) - 1

    #Declaramos un polinomio nulo
    p = 0*x

    #Para cada caracter en la cadena
    for c in bin_num:

        #Si el caracter es 1
        if(c == '1'):

            #Sumamos la potencia de x correspondiente a dicha posicion
            p += x^(bin_num_degree_index)

        #Reducimos en 1 el grado con cada iteracion
        bin_num_degree_index -= 1

    #Devolvemos el polinomio obtenido
    return p

#Devuelve el polinomio interpretado como un numero binario, si algun digito no es 0 o 1 devuelve 0
def reinterpret_finite_poly_as_num(poly):

    #Definimos una cadena donde almacenar los digitos binarios
    num_str = ""

    #Definimos el tamanyo del polinomio
    poly_size = poly.degree() + 1

    #Para cada monomio del polinomio
    for i in range(0, poly_size):

        #Si el digito es 0 anyadimos un 0
        if(poly[poly_size - 1 - i] == 0):
            num_str += '0'

        #Si el digito es 1 anyadimos un 1
        elif(poly[poly_size - 1 - i] == 1):
            num_str += '1'

        #Si no es ni 0 ni 1, no es candidato asi que devolvemos 0
        else:
            return 0

    #Si la cadena es vacia devolvemos un 0
    if(num_str == ""):
        return 0
    
    #Devolvemos la cadena interpretada como un numero binario
    return ZZ("0b" + num_str)

#Genera la lista de factores irreducibles del polinomio en el grupo finito
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

            #Indicamos que el factor previo es el actual
            previous_factor = factors[index]

            #Construimos nuestra guess con el siguiente factor
            next_guess = guess * factors[index]

            #El nuevo grado de nuestra guess es la suma de la anterior y del siguiente
            next_guess_deg = guess.degree() + factors[index].degree()

            #Si coincide el grado de nuestra guess con el grado target
            if(next_guess_deg == target_deg):

                #Interpretamos la guess como un numero
                p = reinterpret_finite_poly_as_num(next_guess)

                #Si la guess divide al numero a factorizar
                if(p != 0 and num % p == 0):

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
    poly_num_interpretations = [reinterpret_num_as_finite_poly(num)]

    #Sacamos todas las posibilidades tras deshacer n carrys
    for i in range (0, N_CARRY):
        poly_num_interpretations = unCarry(poly_num_interpretations)

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
