/*
    getprime_threads.c - main, isPrime, threadCalc, createThreads
    To calculate the prime numbers using threads.
    Update variable K to modify the thread count.
*/

#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#define N0    2             // Start Range for Primes
#define N    10000000           // Upper limit for Primes
#define K    200              // Number of Threads to use
#define FALSE 0
#define TRUE 1
typedef int bool;
int sum[K] = {0};


/*
isPrime - To check whether the given number is
          a prime number or not.
*/
bool isPrime(long num) {
    long limit = sqrt(num);

    if (num < 2) {
        return FALSE;
    }

    else {
    for(long i=2; i<=limit; i++) {
        if(num % i == 0) {
            return FALSE;
        }
    }
    return TRUE;
    }
}

/*
threadCalc - Performs the calculation of primes for
             each thread.
*/
void *threadCalc (void *threadid) {
    
    static int count=0;
    long thid = (long) threadid;
    int pcount = 0;
    long nextCand = N0+(N-N0)*count/K;
    long ulimit = N0+(N-N0)*(count+1)/K;
    count+=1;


    while (nextCand < ulimit) {

        if (isPrime(nextCand)) {
            //printf("is prime: %ld\n", nextCand );   //debug
            pcount++;
        }
        nextCand += 1;
    }
    
    sum[thid] = pcount;
    //printf("End of thread..%ld, %d\n", tid, pcount);    //debug
    return 0;
}


/*
createThreads - Creates threads as per the defined variable K
*/

void createThreads(int low, int high, int numThreads) {

    pthread_t threads[numThreads];                                          // Initializing Threads
    long t;
    int total_primes = 0;

    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    for (t = 0; t < numThreads; t++) {
        pthread_create(&threads[t], NULL, threadCalc, (void *)t);      //Creating Threads
        pthread_join(threads[t],NULL);
    }

    clock_gettime(CLOCK_REALTIME, &end);
    double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0f;

    
    for (int i=0; i<K; i++) {
        //printf("Thread %d has %d\n", i, sum[i] );     //debug
        total_primes += sum[i];
    }
    
    printf("Time taken to calculate: %f seconds\n", time_spent);
    printf("Number of Primes found: %d\n", total_primes);

}

/*
main - Main function to create threads
*/

int main() {

    createThreads(N0, N, K);
    pthread_exit(NULL);             // Thread Exit
    return 0;
}
