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
#include <semaphore.h>

using namespace std;

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
    int fd = shm_open("/bank", O_RDWR, 0666);
    if (fd == -1) {
        cerr << "Error: Bank not initialized. Run ./init first" << endl;
        return 1;
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("Failed to get shared memory size");
        close(fd);
        return 1;
    }
    
    Bank* bank = (Bank*)mmap(NULL, sb.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    print_help();
    
    int command;
    cin >> command;

    
while (command) {
    switch (command) {
        case 1: { // Show balance
            int id;
            cin >> id;
            bank->show_balance(id);
            break;
        }
        case 2: { // Show min balance
            int id;
            cin >> id;
            bank->show_min_balance(id);
            break;
        }
        case 3: { // Show max balance
            int id;
            cin >> id;
            bank->show_max_balance(id);
            break;
        }
        case 4: { // Freeze account
            int id;
            cin >> id;
            bank->freeze_account(id);
            break;
        }
        case 5: { // Unfreeze account
            int id;
            cin >> id;
            bank->unfreeze_account(id);
            break;
        }
        case 6: { // Transfer
            int from, to, amount;
            cin >> from >> to >> amount;
            try {
                bank->transfer(from, to, amount);
            } catch (const exception& e) {
                cerr << "Error: " << e.what() << endl;
            }
            break;
        }
	case 7: { // Credit all accounts
            int amount;
            cin >> amount;
            bank->credit_all(amount);
    	    break;
	}
	case 8: { // Debit all accounts
    	    int amount;
    	    cin >> amount;
    	    bank->debit_all(amount);
    	    break;
	}
	case 9: { // Set min balance
            int id, new_min;
            cin >> id >> new_min;
            bank->set_min_balance(id, new_min);
            break;
	}
	case 10: { // Set max balance
            int id, new_max;
   	    cin >> id >> new_max;
    	    bank->set_max_balance(id, new_max);
    	    break;
	    } 
        case 11: // Help
            print_help();
            break;
        case 12: // Exit
            munmap(bank, sb.st_size);
            close(fd);
            return 0;
        default:
            cout << "Unknown command" << endl;
    }
    cout << "Enter command: ";
    cin >> command;
    }

}
