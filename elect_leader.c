/*
 * elect_leader.c
 *
 *  Created on: Mar 17, 2014
 *      Author: Samiam
 *
 *      Once a leader is elected, the program should have each node print its rank,
 *      identifier value, leader status, number of messages received, and number of
 *      messages sent according to the specification:
 *      	rank=%d, id=%d, leader=0, mrcvd=%d, msent=%d
 *
 *      Once every process has printed the information described above, there should
 *      be one more messaging round to send the total messages sent and total messages
 *      received by every node to the leader. The leader, finally, prints the following.
 *      	rank=%d, id=%d, trcvd=%d, tsent=%d
 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ELECTED_TAG 2
#define PUBLISH_MSGS 3

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

	MPI_Status status;
	int status_val;

	int to_pass;
	int senders_id;

	int i_am_leader = 0;
	int leader_found = 0;
	int leader;

	/* info to print */
	int no_sent = 0;
	int no_recvd = 0;

	if (world_rank == 0) {
		// be the first to send
		to_pass = identifier;
		MPI_Send(&to_pass, 1, MPI_INT, clockwise, 0, MPI_COMM_WORLD);
		no_sent = no_sent + 1;

	}
	while (!leader_found) {
		MPI_Recv(&to_pass, 1, MPI_INT, counter, MPI_ANY_TAG, MPI_COMM_WORLD,
				&status);
		no_recvd = no_recvd + 1;

		senders_id = (counter + 1) * pnum % world_size;
		printf(
				"Identifier %d received token %d from process %d (with identifier %d)\n",
				identifier, to_pass, counter, senders_id);

		status_val = status.MPI_TAG;

		if (status_val == ELECTED_TAG) { /* leader is found! */
			leader_found = 1;
			leader = to_pass;
		} else {

			if (to_pass < identifier) {
				to_pass = identifier;
			} else if (to_pass == identifier) { /* i am the leader! */
				i_am_leader = 1;
				printf(
						"Just got the 'i am leader' message, world rank %d, id %d\n",
						world_rank, identifier);
				status_val = ELECTED_TAG;
			} else {
				//do nothing...
			}
		}

		MPI_Send(&to_pass, 1, MPI_INT, clockwise, status_val, MPI_COMM_WORLD);
		no_sent = no_sent + 1;
	}
	//	senders_id = (counter + 1) * pnum % world_size;
	printf(
			"Process with rank %d, leader status %d and identifier %d reporting the leader is: %d (with process #%d and calculated id %d)\n",
			world_rank, i_am_leader, identifier, leader, counter, senders_id);
	printf("rank=%d, id=%d, leader=%d, mrcvd=%d, msent=%d\n", world_rank, identifier, i_am_leader, no_recvd, no_sent);

//	if(i_am_leader){
//		//then recv all the messages from everyone
//		int i = world_size - 1;
//		while(i < 0){
//			MPI_Recv(&to_pass, 1, MPI_INT, MPI_ANY_SOURCE, PUBLISH_MSGS, MPI_COMM_WORLD,
//							&status);
//			no_recvd = no_recvd + to_pass;
//		}
//	}

	MPI_Finalize();

}
