#include "socket.hpp"

ServerSocket::ServerSocket(int port) {
    // init winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        throw std::runtime_error("WSAStartup failed: " + std::to_string(result));

    }

    // creating the socket
    sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_ == INVALID_SOCKET) {
        WSACleanup();
        throw std::runtime_error("socket() failed: " + std::to_string(WSAGetLastError()));
    }
    // binding to port
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Bind to all interfaces
    serverAddr.sin_port = htons(port);       // Port 8080

    if(bind(sock_, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(sock_);
        WSACleanup();
        throw std::runtime_error("bind() failed: " + std::to_string(WSAGetLastError()));
    }
    // listen for connections
    if (listen(sock_, SOMAXCONN) == SOCKET_ERROR) {
        closesocket(sock_);
        WSACleanup();
        throw std::runtime_error("listen() failed: " + std::to_string(WSAGetLastError()));
    }
    std::cout << "Server listening on port " << port << "..." << std::endl;
}

// server socket destructor
ServerSocket::~ServerSocket() {
    if (sock_ != INVALID_SOCKET) {
        closesocket(sock_);
    }
    WSACleanup();
}

// accept incoming connection
SOCKET ServerSocket::accept() {
    sockaddr_in clientAddr;
    int clientSize = sizeof(clientAddr);
    SOCKET clientSock = ::accept(sock_, (sockaddr*)&clientAddr, &clientSize);
    if (clientSock == INVALID_SOCKET) {
        throw std::runtime_error("accept() failed: " + std::to_string(WSAGetLastError()));
    }
    return clientSock;
}

// ClientSocket Constructor
ClientSocket::ClientSocket(SOCKET sock) : sock_(sock){}

// ClientSocket destructor
ClientSocket::~ClientSocket() {
    if (sock_ != INVALID_SOCKET){
        closesocket(sock_);
    }
}

// Move constructor - "steal" socket from other
ClientSocket::ClientSocket(ClientSocket&& other) noexcept 
    : sock_(other.sock_) {
    other.sock_ = INVALID_SOCKET;  // Prevent other's destructor from closing
}

// Move assignment operator
ClientSocket& ClientSocket::operator=(ClientSocket&& other) noexcept {
    if (this != &other) {  // Guard against self-assignment
        if (sock_ != INVALID_SOCKET) {
            closesocket(sock_);  // Close our current socket if any
        }
        sock_ = other.sock_;     // Steal other's socket
        other.sock_ = INVALID_SOCKET;
    }
    return *this;
}

std::string ClientSocket::recv(int maxBytes) {
    std::string buffer(maxBytes, '\0');
    int bytesReceived = ::recv(sock_, &buffer[0], maxBytes, 0);
    if (bytesReceived <= 0) {
        // connection closed on error
        return "";
    }
    buffer.resize(bytesReceived);
    return buffer;
}

void ClientSocket::send(const std::string& data) {
    int totalSent = 0;
    // possibility of sent fewer bytes than req so loop
    while (totalSent < data.size()) {
        int sent = ::send(sock_, data.c_str() + totalSent,
                          data.size() - totalSent, 0);
        if (sent == SOCKET_ERROR) {
            throw std::runtime_error("send failed");
        }
        totalSent += sent;
    }
}