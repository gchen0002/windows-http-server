#include "server.hpp"

int main() {
    Server server(8080, "../www");
    server.start();
    server.run();
    return 0;
}