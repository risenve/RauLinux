#ifndef BANK_STRUCTURES_H
#define BANK_STRUCTURES_H

#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

const char* SHM_NAME = "/bank";
const char* SEM_NAME = "/bank_sem";

struct Bill {
    int balance;
    int min_balance;
    int max_balance;
    bool frozen;
};

struct Bank {
    sem_t* semaphore;
    int size;
    Bill bills[];
    
};

#endif
