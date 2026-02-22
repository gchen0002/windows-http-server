#include "response.hpp"

Response& Response::setStatus(int code, std::string reason) {
    statusCode_ = code;
    statusReason_ = std::move(reason); // std::move avoids copying
    return *this;
}

Response& Response::setHeader(const std::string& name, const std::string& value) {
    headers_[name] = value; // unorderedmaps []
    return *this;
}

Response& Response::setBody(std::string body) {
    body_ = std::move(body);
    return *this;
}

// assembly of the http response string
std::string Response::build() const {
    std::string result;

    // status line: HTTP/1.1 200 OK\r\n
    result = "HTTP/1.1 " + std::to_string(statusCode_) + " " + statusReason_ + "\r\n";

    // headers from map
    for (const auto& [name, value] : headers_){
        result += name + ": " + value + "\r\n";
    }

    // Auto-add Content-Lenght based on body size
    result += "Content-Length: " + std::to_string(body_.size()) + "\r\n";

    // Blank line (end of headers)
    result += "\r\n";
    
    // body 
    result += body_;
    return result;
}

// static factory methods
Response Response::ok(){
    Response r;
    r.statusCode_ = 200;
    r.statusReason_ = "OK";
    return r;
}

Response Response::notFound() {
    Response r;
    r.statusCode_ = 404;
    r.statusReason_ = "Not Found";
    return r;
}

Response Response::badRequest() {
    Response r;
    r.statusCode_ = 400;
    r.statusReason_ = "Bad Request";
    return r;
}

Response Response::methodNotAllowed() {
    Response r;
    r.statusCode_ = 405;
    r.statusReason_ = "Method Not Allowed";
    return r;
}
