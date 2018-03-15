/*
  Erick Wijaya / 13515057
  source: https://www2.cs.duke.edu/courses/fall08/cps196.1/Pthreads/bitonic.c
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <omp.h>
#include <math.h>

struct timeval startwtime, endwtime;
double seq_time;
FILE *file_to_write;
FILE *log_file;

int inputN, N; // data array size
int *arr;    // arr data array
int num_thread;
int test_amount;
double avg_serial = 0, avg_par = 0;

const int ASCENDING  = 1;
const int DESCENDING = 0;

void init(void);
void print(void);
void test(void);
void exchange(int i, int j);
void compare(int i, int j, int dir);
void bitonicMerge(int lo, int cnt, int dir);
void recBitonicSort(int lo, int cnt, int dir);
void impBitonicSort(void);
void parImpBitonicSort(void);
void parRecBitonicSort(int lo, int cnt, int dir);
void parBitonicMerge(int lo, int cnt, int dir);

/** the main program **/ 
int main(int argc, char **argv) {

  if (argc == 2) {
    test_amount = 3;
    num_thread = 4;
  } else if (argc == 3 && atoi(argv[2]) > 0) {
    test_amount = atoi(argv[2]);
    num_thread = 4;
  } else if (argc == 4 && atoi(argv[2]) > 0) {
    test_amount = atoi(argv[2]);
    num_thread = atoi(argv[3]);
  } else {
    printf("Usage: %s n x\n  where n is problem size\n", argv[0]);
    printf("  where x is test amount (optional with default=3, must be greater than 0)\n");
    exit(1);
  }

  

  inputN = atoi(argv[1]);
  double logarithm = log2(inputN);
  N = exp2(ceil(logarithm));

  arr = (int *) malloc(N * sizeof(int));
  init();
  file_to_write = fopen("data/input.txt", "w");
  if (file_to_write != NULL) {
    int j;
    for (j = 0; j < inputN; j++)
      fprintf(file_to_write, "%d\n", arr[j]);
    fclose(file_to_write);
  }

  log_file = fopen("output/log.txt", "a");
  if (log_file == NULL) {
    printf("Error: can't open/create file");
    exit(1);
  }
  
  time_t rawtime;
  struct tm* timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  printf("%d %d\n", inputN, N);
  printf("-----------------------------------------\n");
  fprintf(log_file, "-----------------------------------------\n");
  fprintf(log_file, "%s", asctime(timeinfo));
  fprintf(log_file, "Problem Size: %d\n", inputN);
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
    seq_time *= 1000000;
    printf("[%d] Serial wall clock time (microseconds) = %f\n", i, seq_time);
    fprintf(log_file, "[%d] Serial wall clock time (microseconds) = %f\n", i, seq_time);
    
    test();
    avg_serial += seq_time;
  }

  printf("-----------------------------------------\n");
  fprintf(log_file, "-----------------------------------------\n");
  for (i = 0; i < test_amount; i++) {
    init();

    // [Start Time]
    gettimeofday (&startwtime, NULL);
    parImpBitonicSort();
    gettimeofday (&endwtime, NULL);
    // [End Time]

    seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
            + endwtime.tv_sec - startwtime.tv_sec);
    seq_time *= 1000000;
    printf("[%d] Parallel wall clock time (microseconds) = %f\n", i, seq_time);
    fprintf(log_file, "[%d] Parallel wall clock time (microseconds) = %f\n", i, seq_time);
    
    test();

    if (i < test_amount - 1) {
      file_to_write = fopen("data/output.txt", "w");
      if (file_to_write != NULL) {
        int j;
        for (j = 0; j < inputN; j++)
          fprintf(file_to_write, "%d\n", arr[j]);
        fclose(file_to_write);
      }
    }
    
    avg_par += seq_time;
  }
  printf("-----------------------------------------\n");
  fprintf(log_file, "-----------------------------------------\n");
  fprintf(log_file, "Average Serial Time (microseconds): %f\n", avg_serial/test_amount);
  fprintf(log_file, "Average Parallel Time (microseconds): %f\n", avg_par/test_amount);
  fprintf(log_file, "-----------------------------------------\n\n\n");
  printf("%f %f\n", avg_serial/test_amount, avg_par/test_amount);
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
  int seed = 13515057;
  srand(seed);
  int i;
  for (i = 0; i < N; i++) {
    if (i < inputN) {
      arr[i] = (int)rand();
    } else {
      arr[i] = 2147483647;
    }
  }
}


/** procedure  print() : print arr array elements **/
void print() {
  int i;
  for (i = 0; i < N; i++) {
    printf("%d ", arr[i]);
  }
}



/** procedure exchange() : pair swap **/
void exchange(int i, int j) {
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

void parImpBitonicSort() {
  int i,j,k;
  
  //#pragma omp parallel shared(N) private(j,k)
  for (k=2; k<=N; k=2*k) {
    for (j=k>>1; j>0; j=j>>1) {
      //#pragma omp parallel for num_threads(num_thread) shared(arr,N) private(i)
      #pragma omp parallel for num_threads(num_thread) shared(arr,j,k,N) private(i)
      //#pragma omp parallel for
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

void parRecBitonicSort(int lo, int cnt, int dir) {
  if (cnt>1) {
    int k=cnt/2;

    #pragma omp parallel
    {
      #pragma omp single
      {
        #pragma omp task
        parRecBitonicSort(lo, k, ASCENDING);

        #pragma omp task
        parRecBitonicSort(lo+k, k, DESCENDING);
      }
    }
    
    parBitonicMerge(lo, cnt, dir);
  }
}

void parBitonicMerge(int lo, int cnt, int dir) {
  if (cnt>1) {
    int k=cnt/2;
    int i;
    for (i=lo; i<lo+k; i++)
      compare(i, i+k, dir);
    
    #pragma omp parallel
    {
      #pragma omp single
      {
        #pragma omp task
        parBitonicMerge(lo, k, dir);

        #pragma omp task
        parBitonicMerge(lo+k, k, dir);
      }
    }
  }
}