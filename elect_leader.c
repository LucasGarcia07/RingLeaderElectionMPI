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

	/**
	 * Unused right now
	 */
	if (argc < 2) {
		printf("Error, correct usage: <PRIME NUMBER>");
		exit(1);
	} else {
		pnum = atoi(argv[1]);
	}

	rank = (world_rank + 1) * pnum % world_size;
	/*
	 * unused right now
	 */

	// these wont work :(
//		int clockwise = ((rank == (world_size - 1)) ? 0 : rank + 1);
//		int counter = ((rank == 0) ? (world_size - 1) : rank - 1);

		int clockwise = ((world_rank == (world_size - 1)) ? 0 : world_rank + 1);
		int counter = ((world_rank == 0) ? (world_size - 1) : world_rank - 1);

	printf("Rank Real-%d, Modified-%d. Neighbours: left-%d, right-%d\n",
			world_rank, rank, counter, clockwise);

	//============================== LEADER ELECTION =================================

	/**
	 * Initialize variables for leader election
	 */
	int token;
	int i_am_leader = 0;
	int leader_found = 0;

	// Receive from the lower process and send to the higher process. Take care
	// of the special case when you are the first process to prevent deadlock.
	if (rank != 0) {
		MPI_Recv(&token, 1, MPI_INT, counter, 0, MPI_COMM_WORLD,
				MPI_STATUS_IGNORE);
		printf("Process %d received token %d from process %d\n", rank,
				token, counter);
	} else {
		// Set the token's value if you are process 0
		token = rank;
	}
	MPI_Send(&token, 1, MPI_INT, clockwise, 0, MPI_COMM_WORLD);
	// Now process 0 can receive from the last process. This makes sure that at
	// least one MPI_Send is initialized before all MPI_Recvs (again, to prevent
	// deadlock)
	if (rank == 0) {
		MPI_Recv(&token, 1, MPI_INT, counter, 0, MPI_COMM_WORLD,
				MPI_STATUS_IGNORE);
		printf("Process %d received token %d from process %d\n", rank,
				token, counter);
	}
	while (!leader_found) {
		leader_found = 1;
	}

	MPI_Finalize();

}
