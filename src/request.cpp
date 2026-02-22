#include "request.hpp"
#include <sstream>

bool Request::parse(const std::string& rawRequest) {
    std::stringstream ss(rawRequest);
    std::string line;

    // 1. Parse request line
    if (!std::getline(ss, line)) return false;
    // remove trailing \r if present
    if (!line.empty() && line.back() == '\r') line.pop_back();
    if (!parseRequestLine(line)) return false;

    // 2. Parse headers
    while (std::getline(ss, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) break; // blank line means end of headers
        parseHeader(line);
    }

    // 3. Parse body remaining stuff
    std::string bodyContent((std::istreambuf_iterator<char>(ss)), {});
    body_ = bodyContent;

    return true;
}

bool Request::parseRequestLine(const std::string& line) {
    // "GET /path HTTP/1.1"
    std::stringstream ss(line);
    return static_cast<bool>(ss >> method_ >> path_ >> version_);
}

void Request::parseHeader(const std::string& line) {
    // "Host: localhost:8080"
    size_t colon = line.find(':');
    if (colon != std::string::npos) {
        std::string name = line.substr(0, colon);
        std::string value = line.substr(colon + 1);

        // Trim leading whitespace from value
        size_t start = value.find_first_not_of(' ');
        if (start != std::string::npos) {
            value = value.substr(start);
        }

        headers_[name] = value;
    }
}

std::string Request::header(const std::string& name) const {
    auto it = headers_.find(name);
    return (it != headers_.end()) ? it->second : "";
}

bool Request::hasHeader(const std::string& name) const {
    return headers_.find(name) != headers_.end();
}

