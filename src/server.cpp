#include "server.hpp"
#include "request.hpp"
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
    std::string rawRequest = client.recv();

    if (rawRequest.empty()) {
        return;  // Connection closed, nothing to do
    }

    Request request;
    if (!request.parse(rawRequest)) {
        Response response = Response::badRequest()
            .setHeader("Content-Type", "text/html")
            .setBody("<h1>400 Bad Request</h1>");
        client.send(response.build());
        return;
    }

    if (request.method() != "GET") {
        Response response = Response::methodNotAllowed()
            .setHeader("Content-Type", "text/html")
            .setBody("<h1>405 Method Not Allowed</h1>");
        client.send(response.build());
        return;
    }

    std::string path = request.path();
    if (path == "/") {
        path = "/index.html";
    }

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