#pragma once
#include "socket.hpp"
#include "response.hpp"
#include "thread_pool.hpp"
#include <thread>

class Server {
public:
    explicit Server(int port, 
                    const std::string& rootDir = "www",
                    size_t threads = std::thread::hardware_concurrency());
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
    size_t threadCount_;
    ThreadPool pool_;
    ServerSocket* serverSocket_;
};