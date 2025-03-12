#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <memory>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

constexpr int clients_max = 10;
constexpr int buf_size = 4096;

struct t_info {
    int operand; // 0 add, 1 sub, 2 div, 3 mul
    int x;
    int y;
    int result;
    int error; // 0 no error, 1 delenie na 0, 2 eror parsing
};

class ClientHandler {
public:
    explicit ClientHandler(int client_socket) : client_socket(client_socket) {}
    void operator()();

private:
    int client_socket;
    t_info info;

    void parser(const std::string &str);
    void calculate();
};

void ClientHandler::parser(const std::string &str) {
    char op[4] = {0};
    info.error = 0;
    if (sscanf(str.c_str(), "%3s %d %d", op, &info.x, &info.y) != 3) {
        info.error = 2;
        return;
    }

    if (strcmp(op, "ADD") == 0)
        info.operand = 0;
    else if (strcmp(op, "SUB") == 0)
        info.operand = 1;
    else if (strcmp(op, "DIV") == 0)
        info.operand = 2;
    else if (strcmp(op, "MUL") == 0)
        info.operand = 3;
    else
        info.error = 2;
}

void ClientHandler::calculate() {
    if (info.error != 0)
        return;

    switch (info.operand) {
        case 0:
            info.result = info.x + info.y;
            break;
        case 1:
            info.result = info.x - info.y;
            break;
        case 2:
            if (info.y == 0)
                info.error = 1;
            else
                info.result = info.x / info.y;
            break;
        case 3:
            info.result = info.x * info.y;
            break;
        default:
            info.error = 2;
            break;
    }
}

void ClientHandler::operator()() {
    char buf[buf_size];
    while (true) {
        ssize_t msg_len = recv(client_socket, buf, buf_size - 1, 0);
        if (msg_len <= 0)
            break;
        buf[msg_len] = '\0';

        parser(buf);
        calculate();

        std::string res;
        if (info.error == 1)
            res = "error division by zero";
        else if (info.error == 2)
            res = "error invalid command";
        else
            res = std::to_string(info.result);

        send(client_socket, res.c_str(), res.size() + 1, 0);
    }

    close(client_socket);
}

void server_thread() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
        perror("connect");
        return;
    }

    if (listen(server_fd, clients_max) < 0) {
        perror("listen");
        return;
    }

    std::cout << "server is listening, spaseq..." << std::endl;

    while (true) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, reinterpret_cast<sockaddr *>(&client_addr), &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        std::thread(ClientHandler(client_fd)).detach();
    }

    close(server_fd);
}

void client_thread() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8888);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("inet_pton");
        return;
    }

    if (connect(sock, reinterpret_cast<sockaddr *>(&serv_addr), sizeof(serv_addr)) < 0) {
        perror("connect");
        return;
    }

    std::cout << "connected to server. Available commands: \nADD <op1> <op2>\nMUL <op1> <op2>\nSUB <op1> <op2>\nDIV <op1> <op2>\n";

    std::string buf;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, buf);
        if (buf.empty())
            break;

        if (send(sock, buf.c_str(), buf.size() + 1, 0) <= 0) {
            perror("send");
            break;
        }

        char res[buf_size];
        ssize_t len = recv(sock, res, buf_size - 1, 0);
        if (len <= 0) {
            perror("recv");
            break;
        }
        res[len] = '\0';
        std::cout << "result: " << res << std::endl;
    }

    close(sock);
}

int main() {
    std::thread server(server_thread);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::thread client(client_thread);

    server.join();
    client.join();
    return 0;
}
