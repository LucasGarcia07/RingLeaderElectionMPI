EXECS=electleader ring
MPICC?=mpicc

all: ${EXECS}

electleader: elect_leader.c
	${MPICC} -o electleader elect_leader.c
	
ring: ring.c
	${MPICC} -o ring ring.c

clean:
	rm ${EXECS}
