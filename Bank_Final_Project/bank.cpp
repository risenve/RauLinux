#include "bank_structures.h"
#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <climits>

using namespace std;

Bank::Bank(int n) : size(n) {
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


void Bank::show_min_balance(int account) {
    sem_wait(&semaphore);
    if (account < 0 || account >= size) {
        cout << "Invalid account number" << endl;
        sem_post(&semaphore);
        return;
    }
    cout << "Account " << account << " min balance: " << bills[account].min_balance << endl;
    sem_post(&semaphore);
}

void Bank::show_max_balance(int account) {
    sem_wait(&semaphore);
    if (account < 0 || account >= size) {
        cout << "Invalid account number" << endl;
        sem_post(&semaphore);
        return;
    }
    cout << "Account " << account << " max balance: " << bills[account].max_balance << endl;
    sem_post(&semaphore);
}

void Bank::freeze_account(int account) {
    sem_wait(&semaphore);
    if (account < 0 || account >= size) {
        cout << "Invalid account number" << endl;
        sem_post(&semaphore);
        return;
    }
    bills[account].frozen = true;
    cout << "Account " << account << " frozen" << endl;
    sem_post(&semaphore);
}

void Bank::unfreeze_account(int account) {
    sem_wait(&semaphore);
    if (account < 0 || account >= size) {
        cout << "Invalid account number" << endl;
        sem_post(&semaphore);
        return;
    }
    bills[account].frozen = false;
    cout << "Account " << account << " unfrozen" << endl;
    sem_post(&semaphore);
}
void Bank::credit_all(int amount) {
    if (amount <= 0) {
        cout << "Amount must be positive" << endl;
        return;
    }

    sem_wait(&semaphore);
    for (int i = 0; i < size; ++i) {
        Bill& acc = bills[i];
        if (!acc.frozen) {
            if (acc.balance + amount > acc.max_balance) {
                cout << "Warning: Account " << i << " would exceed max balance. Skipping." << endl;
                continue;
            }
            acc.balance += amount;
        }
    }
    sem_post(&semaphore);
    cout << "Credited all accounts by " << amount << endl;
}

void Bank::debit_all(int amount) {
    if (amount <= 0) {
        cout << "Amount must be positive" << endl;
        return;
    }

    sem_wait(&semaphore);
    for (int i = 0; i < size; ++i) {
        Bill& acc = bills[i];
        if (!acc.frozen) {
            if (acc.balance - amount < acc.min_balance) {
                cout << "Warning: Account " << i << " would go below min balance. Skipping." << endl;
                continue;
            }
            acc.balance -= amount;
        }
    }
    sem_post(&semaphore);
    cout << "Debited all accounts by " << amount << endl;
}

void Bank::set_min_balance(int account, int new_min) {
    sem_wait(&semaphore);
    if (account < 0 || account >= size) {
        cout << "Invalid account number" << endl;
        sem_post(&semaphore);
        return;
    }

    Bill& acc = bills[account];
    if (new_min > acc.max_balance) {
        cout << "Error: Min balance cannot be greater than max balance" << endl;
    } else if (acc.balance < new_min) {
        cout << "Error: Current balance is below new min balance" << endl;
    } else {
        acc.min_balance = new_min;
        cout << "Account " << account << " min balance set to " << new_min << endl;
    }
    sem_post(&semaphore);
}

void Bank::set_max_balance(int account, int new_max) {
    sem_wait(&semaphore);
    if (account < 0 || account >= size) {
        cout << "Invalid account number" << endl;
        sem_post(&semaphore);
        return;
    }

    Bill& acc = bills[account];
    if (new_max < acc.min_balance) {
        cout << "Error: Max balance cannot be less than min balance" << endl;
    } else if (acc.balance > new_max) {
        cout << "Error: Current balance is above new max balance" << endl;
    } else {
        acc.max_balance = new_max;
        cout << "Account " << account << " max balance set to " << new_max << endl;
    }
    sem_post(&semaphore);
}
