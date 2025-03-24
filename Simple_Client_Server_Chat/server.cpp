#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

// структура для передачи аргументов
struct ClientArgs {
    int client_socket;
    struct sockaddr_in client_address;
};

pthread_mutex_t cout_mutex = PTHREAD_MUTEX_INITIALIZER; 

void* handle_client(void* arg) {
    ClientArgs* args = (ClientArgs*)arg;
    int client_socket = args->client_socket;
    struct sockaddr_in client_address = args->client_address;
    delete args; 

    pthread_mutex_lock(&cout_mutex);
    std::cout << "Connected client: " << inet_ntoa(client_address.sin_addr) 
              << ":" << ntohs(client_address.sin_port) << std::endl;
    pthread_mutex_unlock(&cout_mutex);

    char buffer[1024];
    while (true) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) break; 
        
        buffer[bytes_received] = '\0'; 
        
        pthread_mutex_lock(&cout_mutex);
        std::cout << "Message from " << inet_ntoa(client_address.sin_addr)
                  << ": " << buffer << std::endl;
        pthread_mutex_unlock(&cout_mutex);
    }

    close(client_socket);
    pthread_mutex_lock(&cout_mutex);
    std::cout << "Client disconnected: " << inet_ntoa(client_address.sin_addr)
              << ":" << ntohs(client_address.sin_port) << std::endl;
    pthread_mutex_unlock(&cout_mutex);

    return nullptr;
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket creation error");
        exit(errno);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET; 
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); 
    server_address.sin_port = htons(8888); 

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("bind failed");
        exit(errno);
    }

    if (listen(server_socket, 10) < 0) {
        perror("listen failed");
        exit(errno);
    }

    std::cout << "Server started. Waiting for connections..." << std::endl;

    while (true) {
        struct sockaddr_in client_address;
        socklen_t client_addr_len = sizeof(client_address);
        
        int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_addr_len);
        if (client_socket < 0) {
            perror("accept failed");
            continue;
        }

        ClientArgs* args = new ClientArgs{client_socket, client_address};

        pthread_t thread_id;
        if (pthread_create(&thread_id, nullptr, handle_client, (void*)args) != 0) {
            perror("pthread_create failed");
            delete args;
            close(client_socket);
            continue;
        }

        pthread_detach(thread_id);
    }

    close(server_socket);
    pthread_mutex_destroy(&cout_mutex);
    return 0;
}
