#pragma once
#include <WinSock2.h>
#include <string>
#include <iostream>

// RAII warpper for the listening socket
// passive just listens and accepts
class ServerSocket {
public:
    explicit ServerSocket(int port); // creates binds, listens
    ~ServerSocket();

    SOCKET accept(); // returns raw socket
    
    // delete copying as sockets are unique resources
    ServerSocket(const ServerSocket&) = delete;
    ServerSocket& operator=(const ServerSocket&) = delete;
private:
    SOCKET sock_;
};

// RAII warpper for accepted client connections
class ClientSocket {
public: 
    explicit ClientSocket(SOCKET sock);
    ~ClientSocket();
    std::string recv(int maxBytes = 4096); // read data from client
    void send(const std::string& data); // send data to client

    // delete copying
    ClientSocket(const ClientSocket&) = delete;
    ClientSocket& operator=(const ClientSocket&) = delete;
    
    // Move constructor and assignment
    ClientSocket(ClientSocket&& other) noexcept;
    ClientSocket& operator=(ClientSocket&& other) noexcept;
private:
    SOCKET sock_;
};