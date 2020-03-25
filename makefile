CC=g++
CPPFLAGS=-fopenmp -O3
mgc: 
	$(CC) $(CPPFLAGS) mgc.cpp -o mgc 

run:
	./mgc data/mhr_20_10_5.txt

clean:
	rm mgc mgc.o
