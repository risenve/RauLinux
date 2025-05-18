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

    while (command){
        switch (command)
        {
        case -1:
          print_help();
          break;
        case 1:
          int id;
          cin >> id;
          bank->show_balance(id);
          break;
      default:
        cout << "Unknown command" << endl;
    }
    cout << "Enter command: ";
    cin >> command;
    }
        munmap(bank, sb.st_size);
        close(fd);
    }
