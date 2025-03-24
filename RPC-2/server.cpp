#include <iostream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <pthread.h>

constexpr int MAX_CLIENTS = 10;
constexpr int BUF_SIZE = 4096;
constexpr int PORT = 8888;

struct ThreadData {
    int client_fd;
    struct sockaddr_in client_addr;
};

void* handle_client(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int client_fd = data->client_fd;
    char* client_ip = inet_ntoa(data->client_addr.sin_addr);
    delete data;

    char buffer[BUF_SIZE];
    
    std::cout << "new client connected: " << client_ip << std::endl;

    while (true) {
        int bytes = recv(client_fd, buffer, BUF_SIZE-1, 0);
        if (bytes <= 0) {
            std::cout << "client disconnected: " << client_ip << std::endl;
            break;
        }
        
        buffer[bytes] = '\0';
        std::cout << "message from " << client_ip << ": " << buffer << std::endl;

        std::string response = "server received: ";
        response += buffer;
        
        send(client_fd, response.c_str(), response.size()+1, 0);
    }

    close(client_fd);
    return nullptr;
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "socket creation failed" << std::endl;
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "bind failed" << std::endl;
        return 1;
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        std::cerr << "listen failed" << std::endl;
        return 1;
    }

    std::cout << "server started on port " << PORT << std::endl;
    std::cout << "waiting for connections..." << std::endl;

    pollfd fds[MAX_CLIENTS+1];
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    for (int i = 1; i <= MAX_CLIENTS; i++) {
        fds[i].fd = -1;
    }

    while (true) {
        if (poll(fds, MAX_CLIENTS+1, -1) < 0) {
            std::cerr << "poll error" << std::endl;
            break;
        }

        if (fds[0].revents & POLLIN) {
            sockaddr_in client_addr;
            socklen_t len = sizeof(client_addr);
            int client_fd = accept(server_fd, (sockaddr*)&client_addr, &len);
            
            if (client_fd < 0) {
                std::cerr << "accept failed" << std::endl;
                continue;
            }

            for (int i = 1; i <= MAX_CLIENTS; i++) {
                if (fds[i].fd == -1) {
                    fds[i].fd = client_fd;
                    fds[i].events = POLLIN;
                    
                    ThreadData* data = new ThreadData;
                    data->client_fd = client_fd;
                    data->client_addr = client_addr;

                    pthread_t thread;
                    if (pthread_create(&thread, NULL, handle_client, data) != 0) {
                        std::cerr << "thread creation failed" << std::endl;
                        delete data;
                        close(client_fd);
                        fds[i].fd = -1;
                    }
                    pthread_detach(thread);
                    break;
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
