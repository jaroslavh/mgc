CC=mpic++
CPPFLAGS=-fopenmp -O3 -std=c++11
mgc: 
	$(CC) $(CPPFLAGS) mgc.cpp -o mgc_mpi 

run:
	mpirun ./mgc_pi data/mhr_20_10_5.txt

clean:
	rm mgc_mpi
