#ifndef BANK_H
#define BANK_H

#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

//const char* SHM_NAME = "/bank";
//const char* SEM_NAME = "/bank_sem";

class Bill {
public:
    int balance;
    int min_balance;
    int max_balance;
    bool frozen;
};

class Bank {
public:
    sem_t* semaphore;
    int size;
    Bill* bills;

    //for correct debugging
    void safe_sem_wait();
    void safe_sem_post();
public:
    Bank(void* shared_mem, int n);
    ~Bank();

    void show_balance(int account);
    void show_min_balance(int account);
    void show_max_balance(int account);
    void freeze_account(int account);
    void unfreeze_account(int account);
    void transfer(int from, int to, int amount);
    void credit_all(int amount);
    void debit_all(int amount);
    void set_min_balance(int account, int new_min);
    void set_max_balance(int account, int new_max);

};

#endif
