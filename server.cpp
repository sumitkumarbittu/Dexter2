#include "cpp-httplib/httplib.h"
#include <cstdlib>
#include <iostream>

int main() {
    using namespace httplib;
    Server svr;

    svr.Get("/", [](const Request&, Response& res) {
        res.set_content("Hello from C++ backend!", "text/plain");
    });

    int port = std::getenv("PORT") ? std::stoi(std::getenv("PORT")) : 8080;
    std::cout << "Listening on port " << port << std::endl;
    svr.listen("0.0.0.0", port);
}
