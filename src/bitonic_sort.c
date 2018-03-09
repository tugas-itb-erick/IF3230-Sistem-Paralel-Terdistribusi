/*
 bitonic.c 

 This file contains two different implementations of the bitonic sort
        recursive  version 
        imperative version :  impBitonicSort() 
 

 The bitonic sort is also known as Batcher Sort. 
 For a reference of the algorithm, see the article titled 
 Sorting networks and their applications by K. E. Batcher in 1968 


 The following codes take references to the codes avaiable at 

 http://www.cag.lcs.mit.edu/streamit/results/bitonic/code/c/bitonic.c

 http://www.tools-of-computing.com/tc/CS/Sorts/bitonic_sort.htm

 http://www.iti.fh-flensburg.de/lang/algorithmen/sortieren/bitonic/bitonicen.htm 
 */

/*
------- ---------------------- 
  Nikos Pitsianis, Duke CS 
-----------------------------
*/

/* 
  modified by: Erick Wijaya / 131515057
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <omp.h>

struct timeval startwtime, endwtime;
double seq_time;
FILE *output_file;
FILE *log_file;

int N;     // data array size
int *arr;  // arr data array
int num_thread;
int test_amount;

const int ASCENDING  = 1;
const int DESCENDING = 0;

void init(void);
void print(void);
void sort(void);
void test(void);
static inline void exchange(int i, int j);
void compare(int i, int j, int dir);
void bitonicMerge(int lo, int cnt, int dir);
void recBitonicSort(int lo, int cnt, int dir);
void impBitonicSort(void);
void parBitonicSort(void);


/** the main program **/ 
int main(int argc, char **argv) {

  if (argc == 2) {
    test_amount = 3;
  } else if (argc == 3 && atoi(argv[2]) > 0) {
    test_amount = atoi(argv[2]);
  } else {
    printf("Usage: %s n x\n  where n is problem size\n", argv[0]);
    printf("  where x is test amount (optional with default=3, must be greater than 0)\n");
    exit(1);
  }

  N = atoi(argv[1]);
  arr = (int *) malloc(N * sizeof(int));
  num_thread = omp_get_max_threads();

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
  fprintf(log_file, "-----------------------------------------\n");
  fprintf(log_file, "%s", asctime(timeinfo));
  fprintf(log_file, "Problem Size: %d\n", N);
  fprintf(log_file, "-----------------------------------------\n");

  int i;
  for (i = 0; i < test_amount; i++) {
    init();

    // [Start Time]
    gettimeofday (&startwtime, NULL);
    impBitonicSort();
    gettimeofday (&endwtime, NULL);
    // [End Time]

    seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
            + endwtime.tv_sec - startwtime.tv_sec);
    printf("[%d] Imperative wall clock time = %f\n", i, seq_time);
    fprintf(log_file, "[%d] Imperative wall clock time = %f\n", i, seq_time);
    
    test();
  }

  printf("-----------------------------------------\n");
  fprintf(log_file, "-----------------------------------------\n");
  for (i = 0; i < test_amount; i++) {
    init();

    // [Start Time]
    gettimeofday (&startwtime, NULL);
    parBitonicSort();
    gettimeofday (&endwtime, NULL);
    // [End Time]

    seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
            + endwtime.tv_sec - startwtime.tv_sec);
    printf("[%d] Parallel wall clock time = %f\n", i, seq_time);
    fprintf(log_file, "[%d] Parallel wall clock time = %f\n", i, seq_time);
    
    test();

    if (i < test_amount - 1) {
      output_file = fopen("output/output.txt", "w");
      if (output_file != NULL) {
        int j;
        for (j = 0; j < N; j++)
          fprintf(output_file, "%d\n", arr[j]);
        fclose(output_file);
      }
    }
    
  }
  printf("-----------------------------------------\n");
  fprintf(log_file, "-----------------------------------------\n\n\n");
  fclose(log_file);

  return 0;
}

/** -------------- SUB-PROCEDURES  ----------------- **/ 

/** procedure test() : verify sort results **/
void test() {
  int pass = 1;
  int i;
  for (i = 1; i < N; i++) {
    pass &= (arr[i-1] <= arr[i]);
  }

  printf("    TEST %s\n",(pass) ? "PASSED" : "FAILED");
  fprintf(log_file, "    TEST %s\n",(pass) ? "PASSED" : "FAILED");
}


/** procedure init() : initialize array "arr" with data **/
void init() {
  srand(0);
  int i;
  for (i = 0; i < N; i++) {
    arr[i] = rand() % N; // (N - i);
    // a[i] = (N - i);
  }
}


/** procedure  print() : print arr array elements **/
void print() {
  int i;
  for (i = 0; i < N; i++) {
    printf("%d ", arr[i]);
  }
}



/** INLINE procedure exchange() : pair swap **/
inline void exchange(int i, int j) {
  int t;
  t = arr[i];
  arr[i] = arr[j];
  arr[j] = t;
}



/** procedure compare() 
   The parameter dir indicates the sorting direction, ASCENDING 
   or DESCENDING; if (a[i] > a[j]) agrees with the direction, 
   then a[i] and a[j] are interchanged.
**/
void compare(int i, int j, int dir) {
  if (dir==(arr[i]>arr[j])) 
    exchange(i,j);
}




/** Procedure bitonicMerge() 
   It recursively sorts a bitonic sequence in ascending order, 
   if dir = ASCENDING, and in descending order otherwise. 
   The sequence to be sorted starts at index position lo,
   the parameter cbt is the number of elements to be sorted. 
 **/
void bitonicMerge(int lo, int cnt, int dir) {
  if (cnt>1) {
    int k=cnt/2;
    int i;
    for (i=lo; i<lo+k; i++)
      compare(i, i+k, dir);
    bitonicMerge(lo, k, dir);
    bitonicMerge(lo+k, k, dir);
  }
}



/** function recBitonicSort() 
    first produces a bitonic sequence by recursively sorting 
    its two halves in opposite sorting orders, and then
    calls bitonicMerge to make them in the same order 
 **/
void recBitonicSort(int lo, int cnt, int dir) {
  if (cnt>1) {
    int k=cnt/2;
    recBitonicSort(lo, k, ASCENDING);
    recBitonicSort(lo+k, k, DESCENDING);
    bitonicMerge(lo, cnt, dir);
  }
}


/** function sort() 
   Caller of recBitonicSort for sorting the entire array of length N 
   in ASCENDING order
 **/
void sort() {
  recBitonicSort(0, N, ASCENDING);
}



/*
  imperative version of bitonic sort
*/
void impBitonicSort() {
  int i,j,k;
  
  for (k=2; k<=N; k=2*k) {
    for (j=k>>1; j>0; j=j>>1) {
      for (i=0; i<N; i++) {
        int ij=i^j;
        if ((ij)>i) {
          if ((i&k)==0 && arr[i] > arr[ij]) 
            exchange(i,ij);
          if ((i&k)!=0 && arr[i] < arr[ij])
            exchange(i,ij);
        }
      }
    }
  }
}

void parBitonicSort() {
  
}