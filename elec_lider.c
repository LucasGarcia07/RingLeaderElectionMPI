#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#define ELECTED_TAG 2

int main(int argc, char** argv) {
	
	//Inicializando o ambiente do MPI
	
	int world_rank;
	int world_size;
	int ID;
	time_t t;
    srand(t);

	int pnum = rand();
	MPI_Init(&argc, &argv); 
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	ID = abs((world_rank + 1) * pnum % (world_size * 10));

	int sentido_horario = ((world_rank == (world_size - 1)) ? 0 : world_rank + 1);
	int contador = ((world_rank == 0) ? (world_size - 1) : world_rank - 1);

	printf("MPI_RANK: %d, ID: %d Vizinho esquerdo: %d, Vizinho direito: %d\n",
			world_rank, ID, contador, sentido_horario);

	MPI_Barrier(MPI_COMM_WORLD);

	MPI_Status status;
	int status_valor;
	int passar;
	int sou_lider = 0;
	int lider_encontrado = 0;
	int lider;

	if (world_rank == 0) {
		passar = ID;
		MPI_Send(&passar, 1, MPI_INT, sentido_horario, 0, MPI_COMM_WORLD);
	}
	while (!lider_encontrado) {
		MPI_Recv(&passar, 1, MPI_INT, contador, MPI_ANY_TAG, MPI_COMM_WORLD,
				&status);

		ID_Remetente = (contador + 1) * pnum % world_size;
		
		printf("ID: %d token recebido: %d do processo: %d\n\n", ID, passar, contador);
		
		
		status_valor = status.MPI_TAG;

		if (status_valor == ELECTED_TAG) { /* lider encontrado */
			lider_encontrado = 1;
			lider = passar;
		} else {

			if (passar < ID) {
				passar = ID;
			} else if (passar == ID) { /* sou o lider */
				sou_lider = 1;
				printf("Acabei de receber a mensagem 'sou líder', MPI_RANK: %d, ID: %d\n\n",world_rank, ID);
				status_valor = ELECTED_TAG;
			}
		}

		MPI_Send(&passar, 1, MPI_INT, sentido_horario, status_valor, MPI_COMM_WORLD);

		printf("ID: %d token enviado: %d do processo: %d\n\n", ID, passar, contador);
	}

	printf("Processo com MPI_RANK: %d, status de lider: %d ID: %d dizendo que o lider é: %d (processo #%d)\n",world_rank, sou_lider, ID, lider, contador);
	MPI_Barrier(MPI_COMM_WORLD);

	printf("\n");
	if(sou_lider == 0 ){
		printf("\nMPI_RANK: %d, ID: %d, LIDER: FALSE\n", world_rank, ID);
	} else if (sou_lider == 1){
		
		printf("\nMPI_RANK: %d, ID: %d, LIDER: TRUE\n", world_rank, ID);
		

	}

	MPI_Finalize();
	
	printf("\n");
}