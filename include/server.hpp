#pragma once
#include "socket.hpp"
#include "response.hpp"

class Server {
public:
    explicit Server(int port, const std::string& rootDir = "www");
    void start();
    void run();

    ~Server();
private:
    void handleClient(ClientSocket client);
    std::string readFile(const std::string& path) const;
    std::string getMimeType(const std::string& path) const;
    std::string sanitizePath(const std::string& path) const;

    int port_;
    std::string rootDir_;
    ServerSocket* serverSocket_;
};