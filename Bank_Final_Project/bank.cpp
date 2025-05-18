#include "bank_structures.h"
#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <climits>

using namespace std;

Bank::Bank(int n) : size(n) {
    bills = new Bill[n];
    sem_init(&semaphore, 1, 1);
    sem_wait(&semaphore);
    for (int i = 0; i < size; i++) {
        bills[i].id = i;
        bills[i].balance = 0;
        bills[i].min_balance = INT_MIN;
        bills[i].max_balance = INT_MAX;
        bills[i].frozen = false;
    }
    sem_post(&semaphore);
}

Bank::~Bank() {
    sem_destroy(&semaphore);
    delete[] bills;
}

void Bank::print_info() {
    sem_wait(&semaphore);
    for (int i = 0; i < size; ++i) {
        cout << "id: " << bills[i].id << "\n"
             << "Balance: " << bills[i].balance << "\n"
             << "Min balance: " << bills[i].min_balance << "\n"
             << "Max balance: " << bills[i].max_balance << "\n"
             << "Frozen Status: " << bills[i].frozen << endl;
    }
    sem_post(&semaphore);
}

void Bank::show_balance(int account) {
    sem_wait(&semaphore);
    if (account < 0 || account >= size) {
        cout << "Invalid account number" << endl;
        sem_post(&semaphore);
        return;
    }
    cout << "Account " << account << " balance: " << bills[account].balance << endl;
    sem_post(&semaphore);
}

void Bank::transfer(int from, int to, int amount) {
    if (amount <= 0) {
        throw runtime_error("Amount must be positive");
    }
    if (from == to) {
        throw runtime_error("Unable to transfer money to the same account");
    }
    sem_wait(&semaphore);
    try {
        if (from < 0 || from >= size || to < 0 || to >= size) {
            throw runtime_error("Invalid account number");
        }

        Bill& from_acc = bills[from];
        Bill& to_acc = bills[to];

        if (from_acc.frozen) throw runtime_error("Source account frozen");
        if (to_acc.frozen) throw runtime_error("Destination account frozen");
        if (from_acc.balance - amount < from_acc.min_balance) {
            throw runtime_error("Insufficient funds (min balance limit)");
        }
        if (to_acc.balance + amount > to_acc.max_balance) {
            throw runtime_error("Exceeds destination's max balance");
        }

        from_acc.balance -= amount;
        to_acc.balance += amount;
        sem_post(&semaphore);
        cout << "Transfer successful: " << amount << " from account " << from << " to account " << to << endl;
    } catch (...) {
        sem_post(&semaphore);
        throw;
    }
}
