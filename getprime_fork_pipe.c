/*
getprime_fork_pipe - main, isPrime
    To calculate the prime numbers using forks.
    Update variable K to modify the thread count.

*/

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define N 10000000  //Upper limit for Primes
#define K 200        // Number of forks to use
typedef int bool;
#define FALSE 0
#define TRUE 1

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
main - Main function to create forks
*/

int  main() {
    pid_t pid;
    int p[K][2];
    int total_primes = 0;
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    for ( int i=0; i<K; i++) {
        pipe(p[i]);
        pid = fork();

        if ( pid == 0 ) {
            close(p[i][0]);
            int nextCand = (N/K) * i;
            int ulimit = (N/K) * (i+1);
            int count = 0;

            while(nextCand < ulimit) {
                if(isPrime(nextCand)) {
                    //printf("is prime: %ld\n", nextCand );   //debug
                    count++;
                }
                nextCand +=1;
            }

            write(p[i][1], (void*)&count, sizeof count);
            exit(0);
        }


        else {
            close(p[i][1]);
        }
    }

    for (int i = 0; i < K; i++) {
        int count;
        read(p[i][0], (void*)&count, sizeof count);
        close(p[i][0]);
        total_primes += count;
    }

    for (int i = 0; i < K; i++)
    wait(NULL);


    clock_gettime(CLOCK_REALTIME, &end);
    double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0f;

    printf("Time taken to calculate: %f seconds\n", time_spent);
    printf("Number of Primes found: %d\n", total_primes);
    return 0;
}