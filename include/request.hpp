#pragma once
#include <string>
#include <unordered_map>

class Request {
public:
    bool parse(const std::string& rawRequest);

    const std::string& method() const { return method_; }
    const std::string& path() const { return path_; }
    const std::string& version() const { return version_; }
    const std::string& body() const { return body_; }

    std::string header(const std::string& name) const;
    bool hasHeader(const std::string& name) const;
private:
    std::string method_;
    std::string path_;
    std::string version_;
    std::string body_;
    std::unordered_map<std::string, std::string> headers_;

    bool parseRequestLine(const std::string& line);
    void parseHeader(const std::string& line);
};