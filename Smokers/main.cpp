#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <csignal>
#include <cstring>
#include <sys/mman.h>
#include <mutex>

struct SharedData {
    std::mutex mtx;
    bool terminate = false;
};

SharedData* shared_data;

void handle_sigterm(int) {
    std::lock_guard<std::mutex> lock(shared_data->mtx);
    shared_data->terminate = true;
}

void smoker_process(int read_fd, char output_char) {
    signal(SIGTERM, handle_sigterm);
    char c;
    while (true) {
        {
            std::lock_guard<std::mutex> lock(shared_data->mtx);
            if (shared_data->terminate) break;
        }
        
        ssize_t n = read(read_fd, &c, 1);
        if (n == -1 && errno == EINTR) continue;
        if (n <= 0) break;
        std::cout << output_char << std::endl;
    }
    close(read_fd);
    exit(0);
}

int main() {
    // создаем общ память
    shared_data = static_cast<SharedData*>(
        mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, 
             MAP_SHARED | MAP_ANONYMOUS, -1, 0));
    new (shared_data) SharedData();

    std::vector<int> pipes = {0, 0, 0};
    pipe(&pipes[0]); // T табак
    pipe(&pipes[2]); // P бумага
    pipe(&pipes[4]); // M спички

    std::vector<pid_t> children;
    
    // создает курильщиков
    for (int i = 0; i < 3; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            close(pipes[i*2 + 1]); 
            char output = "TPM"[i];
            smoker_process(pipes[i*2], output);
        }
        children.push_back(pid);
        close(pipes[i*2]); 
    }

    // читает инпут и делит объекты 
    char c;
    while (std::cin >> c) {
        int pipe_idx = -1;
        switch(c) {
            case 't': 
                pipe_idx = 0; 
                break;
            case 'p': 
                pipe_idx = 2; 
                break;
            case 'm': 
                pipe_idx = 4; 
                break;
        }
        if (pipe_idx != -1) {
            write(pipes[pipe_idx + 1], &c, 1);
        }
    }

    // закрываем каналы 
    for (int i = 0; i < 3; i++) {
        close(pipes[i*2 + 1]);
        kill(children[i], SIGTERM);
    }
    
    for (auto child : children) {
        waitpid(child, nullptr, 0);
    }

    // освобождаем ресурсы
    shared_data->~SharedData();
    munmap(shared_data, sizeof(SharedData));

    return 0;
}
