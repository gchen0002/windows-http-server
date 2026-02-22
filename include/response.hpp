#pragma once
#include <string>
#include <unordered_map>

class Response {
public:
    Response& setStatus(int code, std::string reason);
    Response& setHeader(const std::string& name, const std::string& value);
    Response& setBody(std::string body);
    std::string build() const;

    static Response ok();
    static Response notFound();
    static Response badRequest();
    static Response methodNotAllowed();
    static Response forbidden();
private:
    int statusCode_ = 200;
    std::string statusReason_ = "OK";
    std::string body_;
    std::unordered_map<std::string, std::string> headers_;
};
