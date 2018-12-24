/*
Assignment: Lab 6 - Solution to form Hydrogen Peroxide (H 2 O 2 ).
Submitted by: Bharath Karumudi

*/


#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t hydrogen_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t oxygen_lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t waitingO = PTHREAD_COND_INITIALIZER;
pthread_cond_t waitingH = PTHREAD_COND_INITIALIZER;

int waiting_hydrogen = 0;
int waiting_oxygen = 0;

int assigned_oxygen = 0;
int assigned_hydrogen = 0;

const int HYDROGEN_NEEDED = 2;         //This can be changed to form desired bond. Eg: H3O3
const int OXYGEN_NEEDED = 2;           //This can be changed to form desired bond. Eg: H3O3
int molecule_being_formed = 0;         //For debug to show molecule # formed.


void print(int id, char* type) {
    printf("%s-%d wH: %d wO: %d  aH: %d aO: %d \n",  type, id,
           waiting_hydrogen, waiting_oxygen, assigned_hydrogen, assigned_oxygen);
}
void bond(int id, char* type) {
    printf("!!molecule (%d) - %s atom # %d used in making Hydrogen Peroxide.\n", molecule_being_formed, type, id);
    print(id, type);
}


void* Hydrogen(void* arg) {
    long id = (long)arg;
    //print(id, "H-pre-Hlock"); //debug
    pthread_mutex_lock(&hydrogen_lock);
    //print(id, "H-pre-lock"); //debug
    pthread_mutex_lock(&lock);  
    waiting_hydrogen++; 
    
    if (waiting_hydrogen == HYDROGEN_NEEDED) {
        pthread_cond_signal(&waitingO);
    }

    else {
         pthread_mutex_unlock(&hydrogen_lock);
     }
    
    // while not allowed to leave
    while (assigned_hydrogen == 0) {
        // try to make a water molecule
        if (waiting_hydrogen >= HYDROGEN_NEEDED && waiting_oxygen >= OXYGEN_NEEDED) {
            //print(id, "H-pre-wake"); //debug
            waiting_hydrogen -= HYDROGEN_NEEDED;
            assigned_hydrogen += HYDROGEN_NEEDED;
            waiting_oxygen -= OXYGEN_NEEDED;
            assigned_oxygen += OXYGEN_NEEDED;
            molecule_being_formed++;
            
            for (int i = 0; i < OXYGEN_NEEDED; i++)
                pthread_cond_signal(&waitingO);
            for (int i = 0; i < HYDROGEN_NEEDED - 1; i++)
                pthread_cond_signal(&waitingH);

            print(id, "H-wake");

        } else {
            print(id, "H-preSleep");
            pthread_cond_wait(&waitingH, &lock);  // H0
            print(id, "H-postSleep");
        }
    }

    bond(id, "hydrogen");

    assigned_hydrogen--;  
    if (assigned_hydrogen == 0 && waiting_hydrogen == 0) {
         pthread_mutex_unlock(&hydrogen_lock);
    }
    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);

}

void* Oxygen(void* arg) {
    long id = (long)arg;
    print(id, "O-pre-Olock");
    pthread_mutex_lock (&oxygen_lock);
    print(id, "O-pre-lock");
    pthread_mutex_lock (&lock);

    waiting_oxygen++;
    

    if (waiting_oxygen == OXYGEN_NEEDED) {
        pthread_cond_signal(&waitingH);
    } else {
        pthread_mutex_unlock(&oxygen_lock);
    }
  
    while (assigned_oxygen == 0) {
        // try to make a water molecule
        if (waiting_hydrogen >= HYDROGEN_NEEDED && waiting_oxygen >= OXYGEN_NEEDED) {
            //print(id, "O-pre-wake"); //debug
            waiting_hydrogen -= HYDROGEN_NEEDED;
            assigned_hydrogen += HYDROGEN_NEEDED;
            waiting_oxygen -= OXYGEN_NEEDED;
            assigned_oxygen += OXYGEN_NEEDED;
            molecule_being_formed++;
            
            for (int i = 0; i < HYDROGEN_NEEDED; i++)
                pthread_cond_signal(&waitingH);
            for (int i = 0; i < OXYGEN_NEEDED - 1; i++)
                pthread_cond_signal(&waitingO);
            print(id, "O-wake");

        } 

        else {
            print(id, "O-preSleep");
            pthread_cond_wait (&waitingO, &lock); 
            print(id, "O-postSleep");
        }
    }
    

    bond(id, "oxygen");
    assigned_oxygen--;  //ao = 0
    if (assigned_oxygen == 0 && waiting_oxygen == 0) {
         pthread_mutex_unlock(&oxygen_lock);
    }
  
    pthread_mutex_unlock (&lock);
    pthread_exit(NULL);
}


int main(int argc, char** args) {
    int num_hydrogen = 0;
    int num_oxygen = 0;
    // Get the number of hydrogen and oxygen atoms from the user.
    printf("Number of H atoms ? \n");
    scanf("%d", &num_hydrogen);
    printf("Number of O atoms ? \n");
    scanf("%d", &num_oxygen);
    
    
    // Create an array of pthread_t pointers for hydrogen.
    pthread_t **hydrogen_threads = malloc(num_hydrogen * sizeof(struct pthread_t *)); //BK: allocating Double Pointers -- 400bytes
    // Create nH hydrogen threads.
    for (int i = 0; i < num_hydrogen; i++) {
        hydrogen_threads[i] = malloc(sizeof(pthread_t)); //BK: Individual
        pthread_create(hydrogen_threads[i], NULL, Hydrogen, (void *)(long)i);   // Individual Thread 
    }
    
    // Create an array of pthread_t pointers for oxygen.
    pthread_t **oxygen_threads =
        malloc(num_oxygen * sizeof(struct pthread_t *));
    // Create nO oxygen threads.
    for (int i = 0; i < num_oxygen; i++) {
        oxygen_threads[i] = malloc(sizeof(pthread_t));
        pthread_create(oxygen_threads[i], NULL, Oxygen, (void *)(long)i);
    }
    
    // Wait for all of the threads to finish.
    int rc;
    for (int i = 0; i < num_hydrogen; i++) {
        int* result = malloc(sizeof(int));
        *result = 0;
        rc = pthread_join(*hydrogen_threads[i], (void**)(&result));
    }
    for (int i = 0; i < num_oxygen; i++) {
        int* result = malloc(sizeof(int));
        *result = 0;
        rc = pthread_join(*oxygen_threads[i], (void**)(&result));
    }
    printf("All variables used, shutting down\n");
}


