#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <vector>
#include <string>

struct ClientInfo {
    int client_socket;
    sockaddr_in client_address;
    std::string username;
};

std::vector<ClientInfo*> clients;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void send_all(const std::string &message, int sender_socket) {
    pthread_mutex_lock(&clients_mutex);
    for(auto client : clients)
        if(client->client_socket != sender_socket)
            if(send(client->client_socket, message.c_str(), message.size(), 0) < 0)
                perror("Send message error");
    pthread_mutex_unlock(&clients_mutex);
}

void send_message_to_client(int client_socket, const std::string &message) {
    if(send(client_socket, message.c_str(), message.size(), 0) < 0)
        perror("Send message error");
}

void remove_client(int client_socket) {
    pthread_mutex_lock(&clients_mutex);
    for(auto it = clients.begin(); it != clients.end(); ++it)
        if((*it)->client_socket == client_socket) {
            clients.erase(it);
            break;
        }
    pthread_mutex_unlock(&clients_mutex);
}

void* client_handler(void* arg) {
    ClientInfo* client_info = (ClientInfo*)arg;
    int client_socket = client_info->client_socket;
    sockaddr_in client_address = client_info->client_address;
    char buffer[1024];

    int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if(bytes_received <= 0) {
        close(client_socket);
        delete client_info;
        pthread_exit(NULL);
    }
    buffer[bytes_received] = '\0';
    client_info->username = std::string(buffer);

    pthread_mutex_lock(&clients_mutex);
    clients.push_back(client_info);
    pthread_mutex_unlock(&clients_mutex);

    std::cout << "Пользователь подключился: " << client_info->username 
              << " с адреса " << inet_ntoa(client_address.sin_addr) << "\n";

    while(true) {
        bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if(bytes_received <= 0) {
            std::cout << "Пользователь отключился: " << client_info->username << "\n";
            close(client_socket);
            remove_client(client_socket);
            delete client_info;
            pthread_exit(NULL);
        }
        buffer[bytes_received] = '\0';
        std::string msg(buffer);
        if(msg == "/exit") {
            send_message_to_client(client_socket, "Отключаемся...\n");
            std::cout << "Пользователь вышел: " << client_info->username << "\n";
            close(client_socket);
            remove_client(client_socket);
            delete client_info;
            pthread_exit(NULL);
        } else if(msg == "/list") {
            std::string list = "Подключенные пользователи:\n";
            pthread_mutex_lock(&clients_mutex);
            for(auto client : clients)
                list += client->username + "\n";
            pthread_mutex_unlock(&clients_mutex);
            send_message_to_client(client_socket, list);
        } else {
            std::string full_msg = client_info->username + ": " + msg + "\n";
            send_all(full_msg, client_socket);
        }
    }
    return NULL;
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1) {
        perror("Socket creation error");
        exit(errno);
    }
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(8888);
    if(bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Bind error");
        close(server_socket);
        exit(errno);
    }
    if(listen(server_socket, 10) < 0) {
        perror("Listen error");
        close(server_socket);
        exit(errno);
    }
    std::cout << "Ожидание подключений...\n";
    while(true) {
        sockaddr_in client_address;
        socklen_t client_addr_len = sizeof(client_address);
        int client_socket = accept(server_socket, (sockaddr*)&client_address, &client_addr_len);
        if(client_socket < 0) {
            perror("Accept error");
            continue;
        }
        ClientInfo* client_info = new ClientInfo;
        client_info->client_socket = client_socket;
        client_info->client_address = client_address;
        pthread_t thread;
        if(pthread_create(&thread, NULL, client_handler, client_info) != 0) {
            perror("Thread creation error");
            close(client_socket);
            delete client_info;
        } else
            pthread_detach(thread);
    }
    close(server_socket);
    return 0;
}

