import rsa
import base64
import re
from cryptography import x509

pubkey,privkey = rsa.newkeys(16)
texto = pubkey.save_pkcs1('PEM').decode('UTF-8')
text_re = re.sub("\n","", re.sub(r"-.*-", "", texto, flags=re.MULTILINE), flags=re.MULTILINE)
cert = x509.load_der_x509_certificate(bytes(text_re, 'UTF-8'))
print(cert.public_key().public_numbers().n)
#print(privkey.save_pkcs1('PEM').decode('UTF-8'))