CC=g++
CFLAGS=-fopenmp
mgc: mgc.o
	$(CC) $(CFLAGS) -o mgc mgc.o

run:
	./mgc data/mhr_20_10_5.txt

