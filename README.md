Para compilar el código de la parte de C++ se requiere de la librería gmp (The GNU Multiple Precision Arithmetic Library) así como de make.
Además, si se desea ejecutar las versiones mpi, se necesita de un sistema configurado para usar Open MPI (Message Passing Interface)

1º) Se accede al directorio codigo/c++/Z2_ZX con el comando cd:
```
cd codigo/c++/Z2_ZX
```
2º) Se ejecuta el comando make para uno de los siguientes targets:
```
  make specific_method
  make generic_method
  make generic_method_mpi
  make factorizacion_impar_mpi
```
3º) Los binarios generados se encuentran en la carpeta bin:
Ejemplo de uso:
```
  ./bin/specific_method 2394965672548873
```
```
  ./bin/generic_method 2394965672548873 0 true
```
Para ejecutar en MPI, se indica con el siguiente comando donde -n indica el número de nodos MPI
```
mpirun -n 8 ./bin/generic_method_mpi 2394965672548873 0 true
```
```
mpirun -n 8 ./bin/factorizacion_impar_mpi 2394965672548873
```
Si se necesita ayuda sobre el uso de los binarios con ejecutarlos sin parametros indica una descripción de uso.
```
  ./bin/generic_method
```
Utilizacion: ./bin/generic_method <clave_publica> <max_deacarreos> <target_carry[true]>

Donde <clave_publica> es el número a factorizar, <max_deacarreos> es el número máximo de deacarreos a realizar y 
<target_carry[true]> es un parametro que indica si se va a probar solamente con los números con exactamente <max_deacarreos> deacarreos.
