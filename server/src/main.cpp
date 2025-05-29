#include "../include/Server.hpp"
#include <iostream>
#include <thread>

int main() {
    try {
        auto server = std::make_shared<Server>();
        server->socketStart();
        server->serverListen();
        
        std::cout << "Server started. Waiting for connections...\n";
        
        server->acceptClient();
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
