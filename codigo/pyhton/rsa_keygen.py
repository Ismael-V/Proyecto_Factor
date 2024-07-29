import rsa
import base64
import re
import argparse

from pyasn1.codec.der import decoder
from pyasn1_modules.rfc2437 import RSAPrivateKey


# Pre: bits >= 16
# Post: Genera una clave publica n y devuelve tanto n como los factores p, q que lo componen
def getRSAKey(bits):

    # Generamos la clave publica y privada quedandonos solo con la privada
    _, privkey = rsa.newkeys(bits)

    # Parseamos la entrada para quedarnos solo con el texto en base64
    clave = privkey.save_pkcs1('PEM').decode('UTF-8')
    clave_limpia = re.sub("\n","", re.sub(r"-.*-", "", clave, flags=re.MULTILINE), flags=re.MULTILINE)
    clave_decodificada = base64.b64decode(clave_limpia)

    # Decodificar la estructura ASN.1
    private_key_asn1, _ = decoder.decode(clave_decodificada, asn1Spec=RSAPrivateKey())

    # Sacamos los campos que nos interesan
    n = private_key_asn1.getComponentByName('modulus')
    p = private_key_asn1.getComponentByName('prime1')
    q = private_key_asn1.getComponentByName('prime2')

    # Devolvemos el resultado
    return n, p, q


# Crear el parser
parser = argparse.ArgumentParser(description='Genera claves publicas con sus factores correspondientes')

# Añadir los argumentos obligatorios
parser.add_argument('--bits', type=int, required=True, help='Numero de bits de los factores')
parser.add_argument('--iter', type=int, required=True, help='Numero de iteraciones del algoritmo')

# Parsear los argumentos
args = parser.parse_args()

# Conjunto de claves publicas
n_set = set()

# Para todas las iteraciones designadas 
for i in range (0, args.iter):

    # Generamos una clave publica con sus factores
    n, p, q = getRSAKey(args.bits)

    # Mientras que este repetida seguimos generando claves
    while n in n_set:
        n, p, q = getRSAKey(args.bits)
    
    # Añadimos la clave a nuestro conjunto para no repetir
    n_set.add(n)

    # En funcion de que primo es mas grande escribimos primero uno y despues otro o viceversa
    if (p < q):
        print(f"{n}, {p}, {q}")
    else:
        print(f"{n}, {q}, {p}")
