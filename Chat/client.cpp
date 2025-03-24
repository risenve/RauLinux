#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <pthread.h>
#include <cstdlib>

int client_socket;

void* receive_handler(void* arg) {
    char buffer[1024];
    while(true) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if(bytes_received <= 0) {
            std::cout << "Отключение от сервера.\n";
            exit(0);
        }
        buffer[bytes_received] = '\0';
        std::cout << buffer;
    }
    return NULL;
}

int main(){
    sockaddr_in server_address;
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket == -1) {
        perror("Socket creation error");
        exit(errno);
    }
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8888);
    if(connect(client_socket, (sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Connection error");
        exit(errno);
    }
    std::string username;
    std::cout << "Введите имя пользователя: ";
    std::getline(std::cin, username);
    send(client_socket, username.c_str(), username.size(), 0);
    pthread_t thread;
    if(pthread_create(&thread, NULL, receive_handler, NULL) != 0) {
        perror("Thread creation error");
        exit(errno);
    }
    std::string message;
    while(true) {
	std::cout<<">> ";
        std::getline(std::cin, message);
        if(send(client_socket, message.c_str(), message.size(), 0) < 0) {
            perror("Send message error");
            break;
        }
        if(message == "/exit")
            break;
    }
    close(client_socket);
    return 0;
}

