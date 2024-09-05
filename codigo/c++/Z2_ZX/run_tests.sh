#!/bin/bash

fichero=$1

while read clave
do
	echo $clave
	mpirun -n 8 ./bin/generic_method_mpi $(echo $clave | awk '{print $1;}') 4 false < /dev/null
done < $fichero

