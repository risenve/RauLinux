#include <iostream>
#include <string>
#include <vector>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdexcept>
#include "bank_structures.h"

using namespace std;

void show_balance(Bank* bank, int account);
void show_min_balance(Bank* bank, int account);
void show_max_balance(Bank* bank, int account);
void freeze_account(Bank* bank, int account);
void unfreeze_account(Bank* bank, int account);
void transfer(Bank* bank, int from, int to, int amount);
void credit_all(Bank* bank, int amount);
void debit_all(Bank* bank, int amount);
void set_min_balance(Bank* bank, int account, int new_min);
void set_max_balance(Bank* bank, int account, int new_max);
vector<string> split_command(const string& cmd);

void print_help() {
    cout << "\n=== Banking System ===" << endl;
    cout << "1. Show balance" << endl;
    cout << "2. Show min balance" << endl;
    cout << "3. Show max balance" << endl;
    cout << "4. Freeze account" << endl;
    cout << "5. Unfreeze account" << endl;
    cout << "6. Transfer between accounts" << endl;
    cout << "7. Credit all accounts" << endl;
    cout << "8. Debit all accounts" << endl;
    cout << "9. Set min balance" << endl;
    cout << "10. Set max balance" << endl;
    cout << "11. Help" << endl;
    cout << "12. Exit" << endl;
}

int main() {
    int fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (fd == -1) {
        cerr << "Error: Bank not initialized. Run ./init first" << endl;
        return 1;
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("fstat failed");
        close(fd);
        return 1;
    }

    Bank* bank = static_cast<Bank*>(mmap(nullptr, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
    if (bank == MAP_FAILED) {
        perror("mmap failed");
        close(fd);
        return 1;
    }
    close(fd);

    print_help();
    
    string cmd;
    while (cout << "> ", getline(cin, cmd)) {
        try {
            vector<string> tokens = split_command(cmd);
            if (tokens.empty()) continue;

            if (tokens[0] == "12" || tokens[0] == "exit" || tokens[0] == "Exit") {
                break;
            }
            else if (tokens[0] == "11" || tokens[0] == "help" || tokens[0] == "Help") {
                print_help();
            }
            else if (tokens[0] == "1" || tokens[0] == "balance") {
                if (tokens.size() != 2) throw runtime_error("Usage: 1 <account>");
                show_balance(bank, stoi(tokens[1]));
            }
            else if (tokens[0] == "2" || tokens[0] == "min_balance") {
                if (tokens.size() != 2) throw runtime_error("Usage: 2 <account>");
                show_min_balance(bank, stoi(tokens[1]));
            }
            else if (tokens[0] == "3" || tokens[0] == "max_balance") {
                if (tokens.size() != 2) throw runtime_error("Usage: 3 <account>");
                show_max_balance(bank, stoi(tokens[1]));
            }
            else if (tokens[0] == "4" || tokens[0] == "freeze") {
                if (tokens.size() != 2) throw runtime_error("Usage: 4 <account>");
                freeze_account(bank, stoi(tokens[1]));
            }
            else if (tokens[0] == "5" || tokens[0] == "unfreeze") {
                if (tokens.size() != 2) throw runtime_error("Usage: 5 <account>");
                unfreeze_account(bank, stoi(tokens[1]));
            }
            else if (tokens[0] == "6" || tokens[0] == "transfer") {
                if (tokens.size() != 4) throw runtime_error("Usage: 6 <from> <to> <amount>");
                transfer(bank, stoi(tokens[1]), stoi(tokens[2]), stoi(tokens[3]));
            }
            else if (tokens[0] == "7" || tokens[0] == "credit_all") {
                if (tokens.size() != 2) throw runtime_error("Usage: 7 <amount>");
                credit_all(bank, stoi(tokens[1]));
            }
            else if (tokens[0] == "8" || tokens[0] == "debit_all") {
                if (tokens.size() != 2) throw runtime_error("Usage: 8 <amount>");
                debit_all(bank, stoi(tokens[1]));
            }
            else if (tokens[0] == "9" || tokens[0] == "set_min") {
                if (tokens.size() != 3) throw runtime_error("Usage: 9 <account> <amount>");
                set_min_balance(bank, stoi(tokens[1]), stoi(tokens[2]));
            }
            else if (tokens[0] == "10" || tokens[0] == "set_max") {
                if (tokens.size() != 3) throw runtime_error("Usage: 10 <account> <amount>");
                set_max_balance(bank, stoi(tokens[1]), stoi(tokens[2]));
            }
            else {
                cout << "Unknown command. Type '11' for help" << endl;
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    munmap(bank, sb.st_size);
    return 0;
}

vector<string> split_command(const string& cmd) {
    vector<string> tokens;
    size_t start = 0;
    size_t end = cmd.find(' ');
    
    while (end != string::npos) {
        if (start != end) {
            tokens.push_back(cmd.substr(start, end - start));
        }
        start = end + 1;
        end = cmd.find(' ', start);
    }
    tokens.push_back(cmd.substr(start));
    
    return tokens;
}

void show_balance(Bank* bank, int account) {
    pthread_mutex_lock(&bank->mutex);
    if (account < 0 || account >= bank->size) {
        cout << "Error: Invalid account number!" << endl;
    } else {
        cout << "Account " << account << " balance: " << bank->bills[account].balance << endl;
    }
    pthread_mutex_unlock(&bank->mutex);
}

void transfer(Bank* bank, int from, int to, int amount) {
    if (amount <= 0) {
        cout << "Error: Amount must be positive!" << endl;
        return;
    }

    pthread_mutex_lock(&bank->mutex);
    try {
        if (from < 0 || from >= bank->size || to < 0 || to >= bank->size) {
            throw runtime_error("Invalid account number");
        }

        Bill& from_acc = bank->bills[from];
        Bill& to_acc = bank->bills[to];

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
        cout << "Transfer successful: " << amount << " from account " << from << " to account " << to << endl;
    } catch (const exception& e) {
        cout << "Transfer failed: " << e.what() << endl;
    }
    pthread_mutex_unlock(&bank->mutex);
}

void credit_all(Bank* bank, int amount) {
    pthread_mutex_lock(&bank->mutex);
    for (int i = 0; i < bank->size; ++i) {
        if (!bank->bills[i].frozen) {
            bank->bills[i].balance += amount;
        }
    }
    cout << "Credited " << amount << " to all accounts" << endl;
    pthread_mutex_unlock(&bank->mutex);
}

void freeze_account(Bank* bank, int account) {
    pthread_mutex_lock(&bank->mutex);
    if (account < 0 || account >= bank->size) {
        cout << "Error: Invalid account number!" << endl;
    } else {
        bank->bills[account].frozen = true;
        cout << "Account " << account << " frozen" << endl;
    }
    pthread_mutex_unlock(&bank->mutex);
}


void show_min_balance(Bank* bank, int account) {
    pthread_mutex_lock(&bank->mutex);
    if (account < 0 || account >= bank->size) {
        cout << "Error: Invalid account number!" << endl;
    } else {
        cout << "Account " << account << " min balance: "
             << bank->bills[account].min_balance << endl;
    }
    pthread_mutex_unlock(&bank->mutex);
}

void show_max_balance(Bank* bank, int account) {
    pthread_mutex_lock(&bank->mutex);
    if (account < 0 || account >= bank->size) {
        cout << "Error: Invalid account number!" << endl;
    } else {
        cout << "Account " << account << " max balance: "
             << bank->bills[account].max_balance << endl;
    }
    pthread_mutex_unlock(&bank->mutex);
}

void unfreeze_account(Bank* bank, int account) {
    pthread_mutex_lock(&bank->mutex);
    if (account < 0 || account >= bank->size) {
        cout << "Error: Invalid account number!" << endl;
    } else {
        bank->bills[account].frozen = false;
        cout << "Account " << account << " unfrozen" << endl;
    }
    pthread_mutex_unlock(&bank->mutex);
}

void debit_all(Bank* bank, int amount) {
    pthread_mutex_lock(&bank->mutex);
    for (int i = 0; i < bank->size; ++i) {
        if (!bank->bills[i].frozen) {
            bank->bills[i].balance -= amount;
        }
    }
    cout << "Debited " << amount << " from all accounts" << endl;
    pthread_mutex_unlock(&bank->mutex);
}

void set_min_balance(Bank* bank, int account, int new_min) {
    pthread_mutex_lock(&bank->mutex);
    if (account < 0 || account >= bank->size) {
        cout << "Error: Invalid account number!" << endl;
    } else {
        bank->bills[account].min_balance = new_min;
        cout << "Account " << account << " min balance set to " << new_min << endl;
    }
    pthread_mutex_unlock(&bank->mutex);
}

void set_max_balance(Bank* bank, int account, int new_max) {
    pthread_mutex_lock(&bank->mutex);
    if (account < 0 || account >= bank->size) {
        cout << "Error: Invalid account number!" << endl;
    } else {
        bank->bills[account].max_balance = new_max;
        cout << "Account " << account << " max balance set to " << new_max << endl;
    }
    pthread_mutex_unlock(&bank->mutex);
}
