#include "server.hpp"
#include "request.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
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

    // Sanitize path to prevent directory traversal attacks
    std::string safePath = sanitizePath(path);
    if (safePath.empty()) {
        Response response = Response::forbidden()
            .setHeader("Content-Type", "text/html")
            .setBody("<h1>403 Forbidden</h1>");
        client.send(response.build());
        return;
    }

    std::string content = readFile(safePath);

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

std::string Server::sanitizePath(const std::string& path) const {
    namespace fs = std::filesystem;

    // Build the full path by combining root directory with requested path
    fs::path fullPath = fs::path(rootDir_) / fs::path(path).relative_path();

    // Get canonical (absolute, normalized) paths
    // weakly_canonical resolves .. and . without requiring the file to exist
    fs::path canonicalRoot = fs::weakly_canonical(rootDir_);
    fs::path canonicalPath = fs::weakly_canonical(fullPath);

    // Convert to strings for comparison
    std::string rootStr = canonicalRoot.string();
    std::string pathStr = canonicalPath.string();

    // Check if the canonical path starts with the root directory
    // If not, the path is trying to escape (e.g., ../../../etc/passwd)
    if (pathStr.size() < rootStr.size() ||
        pathStr.compare(0, rootStr.size(), rootStr) != 0) {
        return "";  // Path escapes root directory - reject it
    }

    return canonicalPath.string();
}