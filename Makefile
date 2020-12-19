

all: GameOfLifeMPI

GameOfLifeMPI: GameOfLifeMPI.c
	mpicc -o GameOfLifeMPI GameOfLifeMPI.c -lm
	#Please type mpirun -np 2 ./GameOfLifeMPI


Clean:
	rm -rf GameOfLifeMPI 