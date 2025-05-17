#include "Server.hpp"
#include <asio.hpp>
#include <iostream>

int main() {
    try {
        asio::io_context io_context;
        Server server(io_context, 1234);
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
    }
    return 0;
}
