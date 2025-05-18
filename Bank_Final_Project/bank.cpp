#include "bank_structures.h"
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
#include <stdexcept>

Bank::Bank(void* shared_mem, int n) {
    size = n;
   
    char* mem = static_cast<char*>(shared_mem);
    bills = reinterpret_cast<Bill*>(mem + sizeof(Bank));

    semaphore = sem_open("./bank_sem", O_CREAT | O_EXCL, 0666, 1);
    if (semaphore == SEM_FAILED) {
        throw std::runtime_error("Failed to create semaphore");
    }
}

Bank::~Bank() {
    sem_close(semaphore);
}

//выводы семафоров 

void Bank::safe_sem_wait() {
    if (sem_wait(semaphore)) {
        throw std::runtime_error("Semaphore WAIT failed");
    }
}

void Bank::safe_sem_post() {
    if (sem_post(semaphore)) {
        throw std::runtime_error("Semaphore POST failed");
    }
}
void Bank::show_balance(int account) {
    safe_sem_wait();
    if (account < 0 || account >= size) {
        safe_sem_post();
        throw std::runtime_error("Invalid account number");
    }
    int balance = bills[account].balance;
    safe_sem_post();
    std::cout << "Account " << account << " balance: " << balance << std::endl;
}

void Bank::transfer(int from, int to, int amount) {
    if (amount <= 0) {
        throw std::runtime_error("Amount must be positive");
    }

    safe_sem_wait();
    try {
        if (from < 0 || from >= size || to < 0 || to >= size) {
            throw std::runtime_error("Invalid account number");
        }

        Bill& from_acc = bills[from];
        Bill& to_acc = bills[to];

        if (from_acc.frozen) throw std::runtime_error("Source account frozen");
        if (to_acc.frozen) throw std::runtime_error("Destination account frozen");
        if (from_acc.balance - amount < from_acc.min_balance) {
            throw std::runtime_error("Insufficient funds (min balance limit)");
        }
        if (to_acc.balance + amount > to_acc.max_balance) {
            throw std::runtime_error("Exceeds destination's max balance");
        }

        from_acc.balance -= amount;
        to_acc.balance += amount;
        safe_sem_post();
        std::cout << "Transfer successful: " << amount << " from account " << from << " to account " << to << std::endl;
    } catch (...) {
        safe_sem_post();
        throw;
    }
}

