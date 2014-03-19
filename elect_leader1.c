/*
 * elect_leader.c
 *
 *  Created on: Mar 17, 2014
 *      Author: Samiam
 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char** argv) {

	//============================== INITIALIZATION =================================

	/**
	 * First initilize the MPI environment
	 */
	int world_rank;
	int world_size;
	int rank;
	int to_send;
	int to_recv;
	int pnum;

	MPI_Init(NULL, NULL); /* initializes the MPI world? */
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	if (argc < 2) {
		printf("Error, correct usage: <PRIME NUMBER>");
		exit(1);
	} else {
		pnum = atoi(argv[1]);
	}

	rank = (world_rank + 1) * pnum % world_size;

	//fake
	int clockwise = ((world_rank == (world_size - 1)) ? 0 : world_rank + 1);
	int counter = ((world_rank == 0) ? (world_size - 1) : world_rank - 1);

	to_send = ((rank == (world_size - 1)) ? 0 : rank + 1);
	to_recv = ((rank == 0) ? (world_size - 1) : rank - 1);

	printf("Rank Real-%d, Modified-%d. Neighbours: left-%d, right-%d\n",
			world_rank, rank, to_recv, to_send);

	//============================== SEND AND RECV =================================

	//everyone sends
	int token = 5;
	if (world_rank == 0) {
		printf("Process modified %d sending to %d\n", rank, to_send);
		MPI_Send(&token, 1, MPI_INT, to_send, 0, MPI_COMM_WORLD);
		printf("Process modified %d recieving from %d\n", rank, to_recv);
		MPI_Recv(&token, 1, MPI_INT, to_recv, 0, MPI_COMM_WORLD,
				MPI_STATUS_IGNORE);
		printf("Recieve successfull on process %d recieving from %d\n", rank,
				to_recv);

	} else {
		printf("Process modified %d recieving from %d\n", rank, to_recv);
		MPI_Recv(&token, 1, MPI_INT, to_recv, 0, MPI_COMM_WORLD,
				MPI_STATUS_IGNORE);
		printf("Recieve successfull on process %d recieving from %d\n", rank,
				to_recv);
		MPI_Send(&token, 1, MPI_INT, to_send, 0, MPI_COMM_WORLD);
		printf("Process modified %d sending to %d\n", rank, to_send);

	}
	//============================== LEADER ELECTION =================================

	/**
	 * Initialize variables for leader election
	 */
	// Receive from the lower process and send to the higher process. Take care
	// of the special case when you are the first process to prevent deadlock.
	if (world_rank != 0) {
		MPI_Recv(&token, 1, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD,
				MPI_STATUS_IGNORE);
		printf("Process %d received token %d from process %d\n", world_rank,
				token, world_rank - 1);
	} else {
		// Set the token's value if you are process 0
		token = -1;
	}
	MPI_Send(&token, 1, MPI_INT, (world_rank + 1) % world_size, 0,
			MPI_COMM_WORLD);
	// Now process 0 can receive from the last process. This makes sure that at
	// least one MPI_Send is initialized before all MPI_Recvs (again, to prevent
	// deadlock)
	if (world_rank == 0) {
		MPI_Recv(&token, 1, MPI_INT, world_size - 1, 0, MPI_COMM_WORLD,
				MPI_STATUS_IGNORE);
		printf("Process %d received token %d from process %d\n", world_rank,
				token, world_size - 1);
	}
	MPI_Finalize();

}
