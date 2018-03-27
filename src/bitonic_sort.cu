/*
  Erick Wijaya / 13515057
  source: https://www2.cs.duke.edu/courses/fall08/cps196.1/Pthreads/bitonic.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <math.h>

const int ASCENDING  = 1;
const int DESCENDING = 0;
const int MAX_INT = 2147483647;
const int THREADS = 4;

void init(int* arr, int n);
void rng(int* arr, int n);
void print(int* arr);
void test(int* arr, int n);
void swap(int* a, int* b);
void compare(int* arr, int i, int j, int dir);
void bitonicSortSeq(int* arr, int n);

int nearestPowerOfTwo(int x);
void writeToFile(char* filename, int* arr, int n);

/** the main program **/ 
int main(int argc, char **argv) {
  int* arr;
  int i, j;
  int N, fakeN;
  int num_thread;

  struct timeval startwtime, endwtime;
  double seq_time;
  FILE* log_file;

  double sum_parallel = 0;

  if (argc == 2) {
    // pass
    num_thread = THREADS;
  }
  else if (argc == 3) {
    num_thread = atoi(argv[2]);
  }
  else {
    printf("Usage: %s n x\n  where n is problem size and p is thread count\n", argv[0]);
    return 0;
  }
  
 
  // Initialize arr
  N = atoi(argv[1]);
  fakeN = nearestPowerOfTwo(N);
  arr = (int *) malloc(fakeN * sizeof(int));
  init(arr, fakeN);
  rng(arr, N);

  writeToFile("data/input.txt", arr, N);
  log_file = fopen("output/log.txt", "a");
  if (log_file == NULL) {
    printf("Error: can't open/create file");
    exit(1);
  }

  time_t rawtime;
  struct tm* timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  printf("-----------------------------------------\n");
  printf("%s", asctime(timeinfo));
  printf("Problem Size: %d\n", N);
  printf("Process: %d\n", num_thread);
  fprintf(log_file, "-----------------------------------------\n");
  fprintf(log_file, "%s", asctime(timeinfo));
  fprintf(log_file, "Problem Size: %d\n", N);
  fprintf(log_file, "Process: %d\n", num_thread);

  int* newArr;
  newArr = (int *) malloc(fakeN * sizeof(int));
  memcpy(newArr, arr, fakeN);

  // [Start Time]
  gettimeofday (&startwtime, NULL);

  bitonicSortSeq(newArr, fakeN);

  gettimeofday (&endwtime, NULL);
  // [End Time]


  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
          + endwtime.tv_sec - startwtime.tv_sec);
  seq_time *= 1000000;

  printf("Parallel wall clock time (microseconds) = %f\n", seq_time);
  fprintf(log_file, "Parallel wall clock time (microseconds) = %f\n", seq_time);

  writeToFile("data/output.txt", newArr, N);
      
  test(newArr, fakeN);
  sum_parallel += seq_time;
  free(newArr);
  

  printf("-----------------------------------------\n");
  fprintf(log_file, "-----------------------------------------\n\n\n");
  fclose(log_file);
  

  return 0;
}

/** -------------- SUB-PROCEDURES  ----------------- **/ 
void init(int* arr, int n) {
  int i = 0;
  for (i = 0; i < n; i++) {
    arr[i] = MAX_INT;
  }
}

void writeToFile(char* filename, int* arr, int n) {
  FILE* write = fopen(filename, "w");
  if (write != NULL) {
    int j;
    for (j = 0; j < n; j++)
      fprintf(write, "%d\n", arr[j]);
    fclose(write);
  }
}

void test(int* arr, int n) {
  int pass = 1;
  int i;
  for (i = 1; i < n; i++) {
    pass &= (arr[i-1] <= arr[i]);
  }

  printf("TEST %s\n",(pass) ? "PASSED" : "FAILED");
}

void rng(int* arr, int n) {
  int seed = 13515057;
  srand(seed);
  int i;
  for (i = 0; i < n; i++) {
    arr[i] = (int)rand();
  }
}

void swap(int* a, int* b) {
  int t;
  t = *a;
  *a = *b;
  *b = t;
}

void compare(int* arr, int i, int j, int dir) {
  if (dir == (arr[i] > arr[j])) 
    swap(&(arr[i]), &(arr[j]));
}

void bitonicSortSeq(int* arr, int n) {
  int i,j,k;
  
  for (k=2; k<=n; k=2*k) {
    for (j=k>>1; j>0; j=j>>1) {
      for (i=0; i<n; i++) {
	      int ij=i^j;
        if ((ij) > i) {
          if ((i&k) == 0 && arr[i] > arr[ij]) 
            swap(&(arr[i]), &(arr[ij]));
          if ((i&k) != 0 && arr[i] < arr[ij])
            swap(&(arr[i]), &(arr[ij]));
        }
      }
    }
  }
}

int nearestPowerOfTwo(int x) {
  int i = 2;
  while (i < x) {
    i *= 2;
  }

  return i;
}
