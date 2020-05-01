CC=g++
CPPFLAGS=-fopenmp -std=c++11 -O3
mgc: 
	$(CC) $(CPPFLAGS) mgc.cpp -o mgc_task

run:
	./mgc data/mhr_20_10_5.txt

clean:
	rm mgc
