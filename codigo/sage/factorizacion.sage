from itertools import combinations

#Declaramos un cuerpo finito de orden 2
P.<x> = GF(2)[]

#Colocamos el numero a factorizar
a = 359538626972463181545937173441671075415267175255535079644558347735467119057617110613994049314107928149804605744470838482720720128924948666126951368045435618886973105091364723032067678843703378594563440148856361548681362392739186663226884957937817394230632924905285484312704871823549413298764914802394260700157;

#Colocamos el grado target del factor a adivinar (longitud de uno de los factores en bits - 1)
b = 512

#Declaramos un polinomio nulo
q = 0*x;

#Medimos la longitud de la cadena
j = len(str(a.binary())) - 1

#Para cada elemento de la cadena
for i in str(a.binary()):

  #Si la cadena tiene un 1 en ese punto
  if(i == '1'):
    #Sumamos el monomio correspondiente
    q = q + x^(j)

  #Restamos uno al grado
  j -= 1

#Factorizamos el polinomio
F = factor(q)

#Declaramos un indice y una lista
i = 0
List = {}

#Para cada factor
for p in factor(q):
  n = 0

  #Mientras n sea menor al numero de factores
  while n < p[1]:

    #Anyadimos el factor de forma redundante a la lista
    List[i] = p[0]
    i += 1
    n += 1

solucion = 1 + 0*x;

#Devuelve el polinomio interpretado como un numero escrito en binario
def poly_to_bin(polinomio):
  str = ""
  j = 0;
  for i in sorted(polinomio.exponents()):
    while j < i:
      str += "0"
      j += 1
    str += "1"
    j += 1
  
  if(str == ""):
    return "0"
  
  rev_str = ""
  for i in range (0, len(str)):
    rev_str += str[len(str) - 1 - i]
  
  return ZZ("0b" + rev_str)

def solve_factor(guess, guess_deg, factors, initial_index, target_deg):
  for index in range(initial_index, len(factors)):
    next_guess = guess * factors[index]
    new_deg = guess_deg + factors[index].degree()
    if(new_deg == target_deg):
      #print(poly_to_bin(next_guess))
      if(a % poly_to_bin(next_guess) == 0):
        return poly_to_bin(next_guess)
      return 0
    elif(new_deg > target_deg):
      return 0
    solucion = solve_factor(next_guess, new_deg, factors, index + 1, target_deg)
    if(solucion != 0):
      return solucion
  return 0

print(solve_factor(1, 0, List, 0, b))
