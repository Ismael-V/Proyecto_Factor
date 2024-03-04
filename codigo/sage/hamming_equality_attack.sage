import os

# Set the number of threads to 1
os.environ["OMP_NUM_THREADS"] = "1"

#Clave publica a atacar
CLAVE_PUBLICA = [
    803469022129496566413832787617400811301815554554649653438361,
    6455624695367578199454821262392757575390333697927097565235108436845886183891812477262923580543007926644351019495797658417,
    2593447234415158150880561759103712937719035952138904312295269762787651828920346118692012314486480674433331797011170307454308621532932644248696937123376305647601073835281257670796849,
    853505847408621347174758406861792240861042398140499291717193475474824769393417320362647557977823694603531621600606151332030034668234021157582824004509785207324386394493413822591403231216715524473407168578525428020258331483383953419137912635373,
    89884656743115795386465259539451236680898848947115328636715040578866337902750481566354238661203768010560056939935696678829394884407208311246423945664124328274421597392891154851259652058805873851245969775641083292269319271006402181322880012243214961934234908953700164107933415187770613461286001336894523179153,
    89884656743115795386465259539451236680898848947238333669346023329529379793821935691190576553606718228943622646692787649533396472437575100331643021452356799270624548666821618684530456538819237381161921526926730215518397251842884136508158600364010339632773140265833335871757206095545873422627664335643611047869,
    359538626972463181545937173441671075415267175255535079644558347735467119057617110613994049314107928149804605744470838482720720128924948666126951368045435618886973105091364723032067678843703378594563440148856361548681362392739186663226884957937817394230632924905285484312704871823549413298764914802394260700157
]

#Longitud en numero de bits de uno de sus factores
LONGITUD_FACTOR = [100, 201, 301, 404, 512, 512, 513]

import time

#Declaramos un cuerpo finito de orden 2
P.<x> = GF(2)[]

#Devuelve el numero interpretado como un polinomio
def reinterpret_num_as_Z2_poly(num):

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

#Devuelve el polinomio interpretado como un numero
def reinterpret_Z2_poly_as_num(poly):

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

#Genera la lista de factores irreducibles del polinomio en Z2_Z
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
                p = reinterpret_Z2_poly_as_num(next_guess)

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
    poly_num = reinterpret_num_as_Z2_poly(num)

    #Obtenemos su lista de factores
    factor_list = generate_factor_list(poly_num)

    #Devolvemos el resultado de resolver un factor con estos parametros
    return solve_factor_recursive(num, target_deg, x^0, factor_list, 0)

#Buscamos la solucion con la estrategia propuesta
def test():

    for i in range(7):
        resultado = solve_factor(CLAVE_PUBLICA[i], LONGITUD_FACTOR[i] - 1)
        print("Resultado: ", resultado)

start_time = time.time()
test()
end_time = time.time()

print("Time elapsed: ", end_time)
