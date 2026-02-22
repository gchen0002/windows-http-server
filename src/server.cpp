#include "server.hpp"
#include <fstream>
#include <sstream>
// constructor
Server::Server(int port, const std::string& rootDir) : port_(port), rootDir_(rootDir), serverSocket_(nullptr){}

void Server::start(){
    serverSocket_ = new ServerSocket(port_);
}

void Server::run(){
    std::cout << "Server running on port " << port_ << std::endl;

    while (true) {
        SOCKET clientSock = serverSocket_->accept();
        ClientSocket client(clientSock);
        handleClient(std::move(client));
    }
}

std::string Server::readFile(const std::string& path) const {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()){
        return "";
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}
void Server::handleClient(ClientSocket client) {
    std::string request = client.recv();

    // Parse: "GET /index.html HTTP/1.1"
    std::string path;
    size_t firstSpace = request.find(' ');
    if (firstSpace != std::string::npos) {
        size_t secondSpace = request.find(' ', firstSpace + 1);
        if (secondSpace != std::string::npos) {
            path = request.substr(firstSpace + 1, secondSpace - firstSpace - 1);
        }
    }

    // default to index.html
    if (path == "/") {
        path = "/index.html";
    }

    // try to read file
    std::string content = readFile(rootDir_ + path);

    Response response;
    if (content.empty()) {
        response = Response::notFound()
            .setHeader("Content-Type", "text/html")
            .setBody("<h1>404 Not Found</h1>");
    } else {
        response = Response::ok()
            .setHeader("Content-Type", getMimeType(path))
            .setBody(content);
    }

    client.send(response.build());
}

Server::~Server() {
    delete serverSocket_;
}

std::string Server::getMimeType(const std::string& path) const {
    if (path.size() >= 5 && path.substr(path.size() - 5) == ".html") return "text/html";
    if (path.size() >= 4 && path.substr(path.size() - 4) == ".css") return "text/css";
    if (path.size() >= 3 && path.substr(path.size() - 3) == ".js") return "application/javascript";
    if (path.size() >= 4 && path.substr(path.size() - 4) == ".png") return "image/png";
    if (path.size() >= 4 && path.substr(path.size() - 4) == ".jpg") return "image/jpeg";
    return "application/octet-stream";
}