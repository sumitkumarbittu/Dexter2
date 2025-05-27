#include "cpp-httplib/httplib.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

std::string read_file(const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    using namespace httplib;
    Server svr;

    svr.Get("/", [](const Request&, Response& res) {
        std::string html = read_file("index.html");
        res.set_content(html, "text/html");
    });

    int port = std::getenv("PORT") ? std::stoi(std::getenv("PORT")) : 8080;
    std::cout << "Listening on port " << port << std::endl;
    svr.listen("0.0.0.0", port);
}
