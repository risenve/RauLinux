#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>

#define N 5

sem_t* create_shared_sem() {
    sem_t* sem = (sem_t*)mmap(NULL, sizeof(sem_t), 
                          PROT_READ | PROT_WRITE,
                          MAP_SHARED | MAP_ANONYMOUS, 
                          -1, 0);
    if (sem == MAP_FAILED) return NULL;
    return sem;
}

sem_t* sticks[N];

void philosopher(int id) {
    int left = id;
    int right = (id + 1) % N;

    if (id == 0) std::swap(left, right);

    for (int meals = 0; meals < 3; ++meals) { 
        
        std::cout << "философ " << id << " думает\n";
        sleep(1 + rand() % 3);
        
        sem_wait(sticks[left]);
        sem_wait(sticks[right]);
        
        std::cout << "философ " << id << " ест\n";
        sleep(1 + rand() % 2);
        
        
        sem_post(sticks[right]);
        sem_post(sticks[left]);
    }
}

int main() {
    srand(time(NULL)); 
    for (int i = 0; i < N; ++i) {
        sticks[i] = create_shared_sem();
        sem_init(sticks[i], 1, 1); 
    }

    for (int i = 0; i < N; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            philosopher(i);
            exit(0);
        }
    }

    for (int i = 0; i < N; ++i) wait(NULL);

    for (int i = 0; i < N; ++i) {
        sem_destroy(sticks[i]);
        munmap(sticks[i], sizeof(sem_t));
    }

    return 0;
}
