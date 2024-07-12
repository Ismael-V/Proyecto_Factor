#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include "mpi.h"

struct{
	uint32_t id;
	char nombre[32];
} paquete;

int main(int argc, char** argv){
	int ierr, num_procs, my_id;

	int tag = 0;

	ierr = MPI_Init(&argc, &argv);

	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	printf("Hola mundo! Soy el proceso %i de %i procesos.\n", my_id, num_procs);

	if(my_id == 0){

		uint32_t r0, result;
		for(uint8_t i = 0; i < num_procs - 1; i++){
			MPI_Recv(&r0, 1, MPI_UINT32_T, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			printf("Llamada a funcion realizada por %i.\nRespondiendo...\n", r0);
			result = r0*r0;
			MPI_Send(&result, 1, MPI_UINT32_T, r0, tag, MPI_COMM_WORLD);
		}

	}else{
		uint32_t x = my_id, y;
		MPI_Sendrecv(&x, 1, MPI_UINT32_T, 0, tag, &y, 1, MPI_UINT32_T, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("Mi id %i al cuadrado es %i.\n", x, y);
	}

	ierr = MPI_Finalize();

	return 0;
}
