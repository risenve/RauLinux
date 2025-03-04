#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

const std::string SERVER = "httpforever.com";
const std::string PORT = "80";
const std::string REQUEST = "GET / HTTP/1.1\r\nHost: httpforever.com\r\nConnection: close\r\n\r\n";

int create_socket(struct addrinfo*& res) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(SERVER.c_str(), PORT.c_str(), &hints, &res) != 0) {
        std::cerr << "error " << std::endl;
        return -1;
    }

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        std::cerr << "error, cant create socket" << std::endl;
        freeaddrinfo(res);
    }
    return sockfd;
}

bool send_request(int sockfd) {
    if (send(sockfd, REQUEST.c_str(), REQUEST.size(), 0) == -1) {
        std::cerr << "error, cant send request" << std::endl;
        return false;
    }
    return true;
}

void receive_response(int sockfd) {
    std::ofstream output("httpforever.html");
    if (!output) {
        std::cerr << "error, cant open file" << std::endl;
        return;
    }

    char buffer[4096];
    while (true) {
        ssize_t bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) break;
        buffer[bytes_received] = '\0';
        output << buffer;
    }
    output.close();
    std::cout << "th eanswer is saved in httpforever.html" << std::endl;
}

int main() {
    struct addrinfo* res;
    int sockfd = create_socket(res);
    if (sockfd == -1) return 1;

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        std::cerr << "error, cant connect " << std::endl;
        close(sockfd);
        freeaddrinfo(res);
        return 1;
    }
    freeaddrinfo(res);

    if (!send_request(sockfd)) {
        close(sockfd);
        return 1;
    }

    receive_response(sockfd);
    close(sockfd);
    return 0;
}

