#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "bank_structures.h"

int main() {
    int fd = shm_open("./bank", O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open failed (возможно, банк уже удален)");
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

   // pthread_mutex_destroy(&bank->mutex);
    
    if (sem_close(bank->semaphore) == -1) {
        perror("sem_close failed");
    }

    if (sem_unlink("./bank_sem") == -1) {
        perror("sem_unlink failed (возможно, уже удален)");
    }

    munmap(bank, sb.st_size);
    close(fd);

    if (shm_unlink("./bank") == -1) {
        perror("shm_unlink failed");
        return 1;
    }

    std::cout << "Банк успешно уничтожен.\n";
    return 0;
}
