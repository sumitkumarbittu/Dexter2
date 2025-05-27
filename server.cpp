#include <httplib.h>

int main() {
    using namespace httplib;
    Server svr;

    svr.Get("/", [](const Request&, Response& res) {
        res.set_content("Hello from C++ backend!", "text/plain");
    });

    svr.listen("0.0.0.0", 8080); // Listen on port 8080
}
