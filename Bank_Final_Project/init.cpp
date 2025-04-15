#include <iostream>
#include <cstdlib>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <climits>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <N>" << std::endl;
        return 1;
    }

    int n = std::atoi(argv[1]);
    if (n <= 0) {
        std::cerr << "Error: Invalid number of accounts" << std::endl;
        return 1;
    }

    int fd = shm_open("/bank", O_CREAT | O_RDWR | O_EXCL | O_TRUNC, 0666);
    if (fd == -1) {
        perror("shm_open failed");
        return 1;
    }

    
    size_t size = sizeof(Bank) + n * sizeof(Bill);
    if (ftruncate(fd, size) == -1) {
        perror("ftruncate failed");
        shm_unlink("/bank");
        return 1;
    }

    
    void* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap failed");
        shm_unlink("/bank");
        return 1;
    }
    close(fd);

    Bank* bank = static_cast<Bank*>(ptr);
    bank->size = n;

    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&bank->mutex, &mutex_attr);
    pthread_mutexattr_destroy(&mutex_attr);

    // создаю счета
    for (int i = 0; i < n; ++i) {
        Bill& bill = bank->bills[i];
        bill.balance = 0;
        bill.min_balance = INT_MIN;
        bill.max_balance = INT_MAX; 
        bill.frozen = false;
    }

    std::cout << "Bank initialized with " << n << " accounts." << std::endl;
    munmap(ptr, size);
    return 0;
}
