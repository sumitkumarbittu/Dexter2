#include <httplib.h>
#include <cstdlib>  // add at the top for getenv

int main() {
    using namespace httplib;
    Server svr;

    svr.Get("/", [](const Request&, Response& res) {
        res.set_content("Hello from C++ backend!", "text/plain");
    });

    int port = std::getenv("PORT") ? std::stoi(std::getenv("PORT")) : 8080;
    svr.listen("0.0.0.0", port);
}
