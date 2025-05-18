#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "bank_structures.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <N>" << std::endl;
        return 1;
    }

    int n = std::atoi(argv[1]);
    if (n <= 0) {
        std::cerr << "Error: Number of accounts must be positive" << std::endl;
        return 1;
    }

    int fd = shm_open("/bank", O_CREAT | O_RDWR | O_EXCL, 0666);
    if (fd == -1) {
        perror("shm_open failed");
        return 1;
    }

    size_t size = sizeof(Bank) + n * sizeof(Bill);
    if (ftruncate(fd, size) == -1) {
        perror("ftruncate failed");
        shm_unlink("/bank");
        close(fd);
        return 1;
    }

    void* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap failed");
        shm_unlink("/bank");
        close(fd);
        return 1;
    }

    Bank* bank = new (ptr) Bank(n);
    bank->print_info();
    close(fd);

    return 0;
}
