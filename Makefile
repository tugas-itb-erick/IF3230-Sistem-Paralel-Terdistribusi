all: src/bitonic_sort.cu src/bitonic_sort_serial.c
	nvcc src/bitonic_sort.cu -o bitonic_sort -lm
	gcc src/bitonic_sort_serial.c -o bitonic_sort_serial -lm

clean:
	rm -rf bitonic_sort
	rm -rf bitonic_sort.exe
	rm -rf bitonic_sort_serial
	rm -rf bitonic_sort_serial.exe