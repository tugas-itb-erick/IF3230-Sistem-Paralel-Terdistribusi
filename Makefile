all: src/bitonic_sort.c
	gcc -g -Wall -o bitonic_sort src/bitonic_sort.c -fopenmp -lm

clean:
	rm -rf bitonic_sort
	rm -rf bitonic_sort.exe