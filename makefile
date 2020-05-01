CC=g++
CFLAGS=-std=c++11 -O3 -Wall -pedantic
mgc:
	$(CC) $(CFLAGS) -o mgc_seq mgc.cpp

run:
	./mgc_seq data/mhr_20_10_5.txt

