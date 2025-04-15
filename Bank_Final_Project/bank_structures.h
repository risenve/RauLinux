#ifndef BANK_STRUCTURES_H
#define BANK_STRUCTURES_H

#include <pthread.h>

const char* SHM_NAME = "/bank";

struct Bill {
    int balance;
    int min_balance;
    int max_balance;
    bool frozen;
};

struct Bank {
    sem_t *sem_open(const char '/bank', int oflag,
                       mode_t mode, unsigned int value);
    int size;
    Bill bills[];
    i
};

#endif
