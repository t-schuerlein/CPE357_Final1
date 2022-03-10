all: 
	g++ ./mpi.cpp -o mpi -lrt -Wall
	g++ ./sorter.cpp -o sorter -lrt -Wall