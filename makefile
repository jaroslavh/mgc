CC=mpic++
CPPFLAGS=-fopenmp -O3 -std=c++11
mgc: 
	$(CC) $(CPPFLAGS) mgc.cpp -o mgc 

run:
	mpirun ./mgc data/mhr_20_10_5.txt

clean:
	rm mgc
