/*
 * kosmos-mcv.c (mutexes & condition variables)
 *
 * For UVic CSC 360, Summer 2022
 *
 * Here is some code from which to start.
 *
 * PLEASE FOLLOW THE INSTRUCTIONS REGARDING WHERE YOU ARE PERMITTED
 * TO ADD OR CHANGE THIS CODE. Read from line 133 onwards for
 * this information.
 */

#include <assert.h>
#include <pthread.h>
#include <sched.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "logging.h"


/* Random # below threshold indicates H; otherwise C. */
#define ATOM_THRESHOLD 0.55
#define DEFAULT_NUM_ATOMS 40

#define MAX_ATOM_NAME_LEN 10
#define MAX_KOSMOS_SECONDS 5

/* Global / shared variables */
int  cNum = 0, hNum = 0;
long numAtoms;


/* Function prototypes */
void kosmos_init(void);
void *c_ready(void *);
void *h_ready(void *);
void make_radical(int, int, int, char *);
void wait_to_terminate(int);


/* Needed to pass legit copy of an integer argument to a pthread */
int *dupInt( int i )
{
    int *pi = (int *)malloc(sizeof(int));
    assert( pi != NULL);
    *pi = i;
    return pi;
}


int main(int argc, char *argv[])
{
    long seed;
    numAtoms = DEFAULT_NUM_ATOMS;
    pthread_t **atom;
    int i;
    int status;

    if ( argc < 2 ) {
        fprintf(stderr, "usage: %s <seed> [<num atoms>]\n", argv[0]);
        exit(1);
    }

    if ( argc >= 2) {
        seed = atoi(argv[1]);
    }

    if (argc == 3) {
        numAtoms = atoi(argv[2]);
        if (numAtoms < 0) {
            fprintf(stderr, "%ld is not a valid number of atoms\n",
                numAtoms);
            exit(1);
        }
    }

    kosmos_log_init();
    kosmos_init();

    srand(seed);
    atom = (pthread_t **)malloc(numAtoms * sizeof(pthread_t *));
    assert (atom != NULL);
    for (i = 0; i < numAtoms; i++) {
        atom[i] = (pthread_t *)malloc(sizeof(pthread_t));
        if ( (double)rand()/(double)RAND_MAX < ATOM_THRESHOLD ) {
            hNum++;
            status = pthread_create (
                    atom[i], NULL, h_ready,
                    (void *)dupInt(hNum)
                );
        } else {
            cNum++;
            status = pthread_create (
                    atom[i], NULL, c_ready,
                    (void *)dupInt(cNum)
                );
        }
        if (status != 0) {
            fprintf(stderr, "Error creating atom thread\n");
            exit(1);
        }
    }

    /* Determining the maximum number of ethynyl radicals is fairly
     * straightforward -- it will be the minimum of the number of
     * hNum and cNum/2.
     */

    int max_radicals = (hNum < cNum/2 ? hNum : (int)(cNum/2));
#ifdef VERBOSE
    printf("Maximum # of radicals expected: %d\n", max_radicals);
#endif

    wait_to_terminate(max_radicals);
}


/*
* Now the tricky bit begins....  All the atoms are allowed
* to go their own way, but how does the Kosmos ethynyl-radical
* problem terminate? There is a non-zero probability that
* some atoms will not become part of a radical; that is,
* many atoms may be blocked on some condition variable of
* our own devising. How do we ensure the program ends when
* (a) all possible radicals have been created and (b) all
* remaining atoms are blocked (i.e., not on the ready queue)?
*/


/*
 * ^^^^^^^
 * DO NOT MODIFY CODE ABOVE THIS POINT.
 *
 *************************************
 *************************************
 *
 * ALL STUDENT WORK MUST APPEAR BELOW.
 * vvvvvvvv
 */

// Author: Alireza Tahmasebi
// Login:  alirezatahmasebi1378@uvic.ca 

/* 
 * DECLARE / DEFINE NEEDED VARIABLES IMMEDIATELY BELOW.
 */

int carbon_forming = 0; // used for checking if carbon is the combiner atom (0:No,1:Yes)
int hydrogen_forming = 0; // used for checking if hydrogen is the combiner atom (0:No,1:Yes)

__thread int hid; // declaring local thread variable for maintaining the id's of suspended carbon atoms 
int h_atoms[500]; // integer array for storing id's of hydrogen atoms
int h_remove = 0; // integer for tracking the index at which to retrieve the id of a hydrogen atom
int h_add = 0; // integer for tracking the index at which to add the id of a hydrogen atom
__thread int cid; // declaring local thread variable for maintaining the id's of suspended carbon atoms 
int c_atoms[500]; // integer array for storing id's of carbon atoms
int c_remove = 0; // integer for tracking the index at which to retrieve the id of a hydrogen atom
int c_add = 0; // integer for tracking the index at which to add the id of a carbon atom

int radicals; // # of ethynyl radicals (which is made up of two carbon atoms and one hydrogen atom)
int num_free_c; // # of free carbon atoms
int num_free_h; // # of free hydrogen atoms

int combining_c1; // id of the first combining carbon atom
int combining_c2; // id of the second combining carbon atom
int combining_h; // id of the combining hydrogen atom
char combiner[MAX_ATOM_NAME_LEN]; // name of the atom that triggers the creation of the radical

pthread_mutex_t m; // mutex lock for protecting shared variables/avoiding race conditions
pthread_cond_t wait_c; // carbon atoms wait on this condition variable 
pthread_cond_t wait_h; // hydrogen atoms wait on this condition variable

/*
 * FUNCTIONS YOU MAY/MUST MODIFY.
 */

void kosmos_init() {
    int status_wait_c = pthread_cond_init(&wait_c, NULL); // initialize cond var referenced by '&wait_c'
    if (status_wait_c != 0) {
        fprintf(stderr, "Could not initialize condition variable 'wait_c'\n");
        exit(1);
    }
    int status_wait_h = pthread_cond_init(&wait_h, NULL); // initialize cond var referenced by '&wait_h'
    if (status_wait_h != 0) {
        fprintf(stderr, "Could not initialize condition variable 'wait_h'\n");
        exit(1);
    }
    int status_m = pthread_mutex_init(&m, NULL); // initialize the mutex referenced by '&m'
    if (status_m != 0) {
        fprintf(stderr, "Error creating mutex 'm'\n");
        exit(1);
    }
}

void *h_ready( void *arg )
{
    char line[500]; // char array for printing messages to stdout
    hid = *((int *)arg); // casting argument to an int for atom identification number 
    char name[MAX_ATOM_NAME_LEN]; // char array for atom name
    sprintf(name, "h%03d", hid); // compose the name of the hydrogen atom from the id #
    sprintf(line, "%s now exists", name); // compose a message saying that a hydrogen atom is created
    kosmos_log_add_line(line); // send the message to be printed in the log
    pthread_mutex_lock(&m); // acquire the lock 
    num_free_h++; // a hydrogen atom is generated/created (we have a free h atom)
    hydrogen_forming = 1;
    pthread_cond_signal(&wait_c); // is there a carbon atom waiting? (signal)
    pthread_cond_signal(&wait_c); // is there a carbon atom waiting? (signal)
    pthread_mutex_unlock(&m); // release the lock 
    // as each hydrogen atom comes in, are there enough atoms (2 carbons) to make radicals?
    if (num_free_h >= 1 && num_free_c >= 2){
        pthread_mutex_lock(&m); // acquire the lock 
        combining_h = hid; // set the name of the combining h atom to be the recently created h atom
        combining_c1 = c_atoms[c_remove];
        c_remove++;
        combining_c2 = c_atoms[c_remove];
        c_remove++;
        radicals++; // increment the # of radicals
        num_free_h--; // decrement the # of hydrogen atoms
        num_free_c--; // decrement the # of carbon atoms
        num_free_c--; // decrement the # of carbon atoms
        hydrogen_forming = 0;
        sprintf(combiner, "h%03d", combining_h); // compose the name of the combiner hydrogen atom from the id #
        make_radical(combining_c1, combining_c2, combining_h, combiner); // print details about the radical via a function all
        pthread_mutex_unlock(&m); // release the lock 
    }
    else{
        pthread_mutex_lock(&m); // acquire the lock 
        while (!(num_free_h >= 1 && num_free_c >= 2) && (carbon_forming == 0)){ /* block */
            pthread_cond_wait(&wait_h, &m);
        }
        h_atoms[h_add] = hid;
        h_add++;
        pthread_mutex_unlock(&m); // release the lock 
    }
    // terminate the thread/atom
    pthread_exit(NULL);
    return NULL;
}

void *c_ready( void *arg )
{
    char line[500]; // char array for printing messages to stdout
    cid = *((int *)arg); // casting argument to an int for atom identification number;    
    char name[MAX_ATOM_NAME_LEN]; // char array for atom name
    sprintf(name, "c%03d", cid); // compose the name of the carbon atom from the id #
    sprintf(line, "%s now exists", name); // compose a message saying that a carbon atom is created
    kosmos_log_add_line(line); // send the message to be printed in the log
    pthread_mutex_lock(&m); // acquire the lock 
    num_free_c++; // a carbon atom is generated/created (we have a free c atom)
    // as each carbon atom comes in, are there enough atoms (1 hydrogen and 1 carbon) to make radicals?
    carbon_forming = 1;
    pthread_cond_signal(&wait_h); // is there a hydrogen atom waiting? (signal)
    pthread_cond_signal(&wait_c); // is there a carbon atom waiting? (signal)
    pthread_mutex_unlock(&m); // release the lock 
    if (num_free_h >= 1 && num_free_c >= 2){
        pthread_mutex_lock(&m); // acquire the lock 
        combining_h = h_atoms[h_remove];
        h_remove++;
        combining_c1 = cid; // set the name of the combining c atom to be the recently created c atom
        combining_c2 = c_atoms[c_remove];
        c_remove++;
        radicals++; // increment the # of radicals
        num_free_h--; // decrement the # of hydrogen atoms
        num_free_c--; // decrement the # of carbon atoms
        num_free_c--; // decrement the # of carbon atoms
        carbon_forming = 0;
        sprintf(combiner, "c%03d", combining_c1); // compose the name of the combiner carbon atom from the id #
        make_radical(combining_c1, combining_c2, combining_h, combiner); // print details about the radical via a function all
        pthread_mutex_unlock(&m); // release the lock 
    }
    else{
        pthread_mutex_lock(&m); // acquire the lock 
        while (!(num_free_h >= 1 && num_free_c >= 2) && !((carbon_forming == 1) || (hydrogen_forming == 1))){ /* block */
            pthread_cond_wait(&wait_c, &m);
        }
        c_atoms[c_add] = cid;
        c_add++;
        pthread_mutex_unlock(&m); // release the lock 
    }
    // terminate the thread/atom
    pthread_exit(NULL);
    return NULL;
}

/* 
 * Note: The function below need not be used, as the code for making radicals
 * could be located within h_ready and c_ready. However, it is perfectly
 * possible that you have a solution which depends on such a function
 * having a purpose as intended by the function's name.
 */
void make_radical(int c1, int c2, int h, char *maker)
{
#ifdef VERBOSE
    fprintf(stdout, "%03d: c%03d c%03d h%03d (%s)\n", radicals, c1, c2, h, maker);
#endif
    kosmos_log_add_entry(radicals, c1, c2, h, maker);
}

void wait_to_terminate(int expected_num_radicals) {
    /* A rather lazy way of doing it, for now. */  
    sleep(MAX_KOSMOS_SECONDS);
    kosmos_log_dump();      
    exit(0);
}
