all: src/bitonic_sort.c
	gcc -g -Wall -o bitonic_sort src/bitonic_sort.c -fopenmp -lm

run:
	./bitonic_sort 1024 3

clean:
	rm -rf bitonic_sort
	rm -rf bitonic_sort.exe