#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>

#define n 5 // Number of philosophers

sem_t* sticks[n]; 

void philosopher(int id) {
    srand(time(NULL) ^ (getpid() << 16)); 
    int left = id; 
    int right = (id + 1) % n; 
    if (id == 0) {
        std::swap(left, right);
    }

    while (true) {
        // thinking
        int think_time = rand() % 3 + 1; // time for thinking 
        std::cout << "philosopher " << id << " is thinking for " << think_time << " sec.\n";
        sleep(think_time);
        //choosing
        sem_wait(sticks[left]); 
        sem_wait(sticks[right]); 
        
        // eating
        int eat_time = rand() % 3 + 1; //time for eating
        std::cout << "philosopher " << id << " is eating for " << eat_time << " sec.\n";
        sleep(eat_time);
        
        // put down
        sem_post(sticks[right]); 
        sem_post(sticks[left]);
    }
}

int main() {
    for (int i = 0; i < n; i++) {
        sticks[i] = new sem_t;
        sem_init(sticks[i], 1, 1);  
    }
    
     
    pid_t pids[n];
    for (int i = 0; i < n; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {  
            philosopher(i);
            exit(0);
        }
    }
    
     
    for (int i = 0; i < n; i++) {
        waitpid(pids[i], NULL, 0);
    }
    
     
    for (int i = 0; i < n; i++) {
        sem_destroy(sticks[i]); 
        delete sticks[i]; 
    }
    
    return 0;
}

