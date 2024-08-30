#!/bin/bash

fichero=$1

while read clave
do
	echo $clave
	mpirun -n 8 ./bin/factorizacion_impar_mpi $(echo $clave | awk '{print $1;}') < /dev/null
done < $fichero

