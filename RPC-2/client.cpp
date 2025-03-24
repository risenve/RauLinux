#include <iostream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

constexpr int PORT = 8888;

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "socket creation failed" << std::endl;
        return 1;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "invalid address" << std::endl;
        return 1;
    }

    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "connection failed" << std::endl;
        return 1;
    }

    std::cout << "connected to server. type messages (type 'exit' to quit):" << std::endl;

    while (true) {
        std::string message;
        std::cout << "> ";
        std::getline(std::cin, message);
        
        if (message == "exit") break;

        if (send(sock, message.c_str(), message.size()+1, 0) <= 0) {
            std::cerr << "send failed" << std::endl;
            break;
        }

        char buffer[1024];
        int bytes = recv(sock, buffer, sizeof(buffer)-1, 0);
        if (bytes <= 0) {
            std::cerr << "server disconnected" << std::endl;
            break;
        }
        
        buffer[bytes] = '\0';
        std::cout << "server response: " << buffer << std::endl;
    }

    close(sock);
    return 0;
}
