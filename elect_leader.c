/*
 * elect_leader.c
 *
 *  Created on: Mar 17, 2014
 *      Author: Samiam
 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ELECTED_TAG 2

int main(int argc, char** argv) {

	//============================== INITIALIZATION =================================

	/**
	 * First initilize the MPI environment
	 */
	int world_rank;
	int world_size;
	int identifier;
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

	identifier = (world_rank + 1) * pnum % world_size;
	/*
	 * unused right now
	 */

	// these wont work :(
	//		int clockwise = ((rank == (world_size - 1)) ? 0 : rank + 1);
	//		int counter = ((rank == 0) ? (world_size - 1) : rank - 1);

	int clockwise = ((world_rank == (world_size - 1)) ? 0 : world_rank + 1);
	int counter = ((world_rank == 0) ? (world_size - 1) : world_rank - 1);

	printf("Rank Real-%d, Identifier-%d. Neighbours: left-%d, right-%d\n",
			world_rank, identifier, counter, clockwise);

	//============================== LEADER ELECTION =================================

	int to_pass;
	int i_am_leader = 0;
	int leader_found = 0;
	int participant = 0;
	MPI_Status status;
	int status_val;
	int leader;

	if (identifier == 0) {
		// be the first to send
		to_pass = identifier;
		MPI_Send(&to_pass, 1, MPI_INT, clockwise, 0, MPI_COMM_WORLD);

	}
	while (!leader_found) {
		MPI_Recv(&to_pass, 1, MPI_INT, counter, MPI_ANY_TAG, MPI_COMM_WORLD,
				&status);
		printf("Process %d received token %d from process %d\n", identifier,
				to_pass, counter);

		status_val = status.MPI_TAG;

		if (status_val == ELECTED_TAG) { /* leader is found! */
			leader_found = 1;
			participant = 0;
			leader = to_pass;
		} else {

			if (to_pass < identifier) {
				to_pass = identifier;
			} else if (to_pass == identifier) { /* i am the leader! */
				i_am_leader = 1;
				status_val = ELECTED_TAG;
			} else {
				//do nothing...
			}
		}

		MPI_Send(&to_pass, 1, MPI_INT, clockwise, status_val, MPI_COMM_WORLD);

		//		leader_found = 1;
	}
	printf(
			"Process with rank %d, leader status %d and identifier %d reporting the leader is: %d\n",
			world_rank, i_am_leader, identifier, leader);

	MPI_Finalize();

}
