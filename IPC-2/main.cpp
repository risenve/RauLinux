#include <iostream>
#include <unistd.h>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>
#include <sys/wait.h>


int calculate_prime(int n) {
    if (n < 1) return -1;

    std::vector<int> primes;
    int num = 2;

    while (primes.size() < static_cast<std::size_t>(n)) {
        bool is_prime = true;
        for (int p : primes) {
            if (p * p > num) break;
            if (num % p == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime) primes.push_back(num);
        ++num;
    }
    return primes.back();
}

int main() {
    int pipe1[2], pipe2[2];

    // Create pipes
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe creation failed");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        return 1;
    }

    if (pid == 0) {
               close(pipe1[1]);
        close(pipe2[0]);

        while (true) {
            int m;
            if (read(pipe1[0], &m, sizeof(m)) <= 0) break;

            std::cout << "[Child] Calculating " << m << "-th prime number...\n";
            int result = calculate_prime(m);
             std::cout << "[Child] Sending calculation result of prime(" << m << ")...\n";
            write(pipe2[1], &result, sizeof(result));
        }

        close(pipe1[0]);
        close(pipe2[1]);
    } else {
       
        close(pipe1[0]); 
        close(pipe2[1]); 
        while (true) {
            std::cout << "[Parent] Please enter a number (or type 'exit' to quit): ";
            std::string input;
            std::getline(std::cin, input);

            if (input == "exit") {
                std::cout << "[Parent] Exiting...\n";
                break;
            }

            int m;
            std::stringstream ss(input);
            if (!(ss >> m) || m <= 0) {
                std::cerr << "[Parent] Invalid input. Please enter a positive integer.\n";
                continue;
            }

            std::cout << "[Parent] Sending " << m << " to the child process...\n";
            write(pipe1[1], &m, sizeof(m));

            std::cout << "[Parent] Waiting for the response from the child process...\n";
            int result;
            if (read(pipe2[0], &result, sizeof(result)) > 0) {
                std::cout << "[Parent] Received calculation result of prime(" << m << ") = " << result << ".\n";
            }
        }

        close(pipe1[1]);
        close(pipe2[0]);
        wait(nullptr); // parents are waiting theyr erexaner
    }

    return 0;
}

