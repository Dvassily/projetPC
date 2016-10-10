#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define N 15

void calcul_fibo(int n){
  printf("n = %d \n", n);
  int t1=0, t2=1, nextTerm = 0;
  nextTerm = t1+t2;

  while(nextTerm < n)
  {   t1 = t2;
      t2 = nextTerm;
      nextTerm = t1+t2;
  }
  printf("fibo = %d\n", nextTerm);
}

void * run (void * ptr_data){
    time_t t1 = clock();
    calcul_fibo(*((int * ) ptr_data));
    time_t t2 = clock();
    printf("\n%f %d\n",
     (float) (t2 - t1)/ CLOCKS_PER_SEC,*((int * ) ptr_data));
     return NULL;
}

pthread_t threads[N];

int main(int argc, char const *argv[]) {
  int data[N] = {1, 3, 80, 150, 40, 50, 33, 22, 27, 30, 44, 42, 18, 19, 49};
  for (size_t i = 0; i < N ; i++) {
    pthread_create(&threads[i], NULL, run, data + i);
  }
  int rc = 0;
  int t = 0;
  void *  status;
  for(t=0; t<N; t++) {
       rc = pthread_join(threads[t], &status);
  }
  return 0;
}
