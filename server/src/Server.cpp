#include "../include/Server.hpp"
#include <iostream>
#include <thread>
#include <vector>

Server::Server(asio::io_context& io_context, unsigned short port)
    : io_context_(io_context),
      acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {
    start_accept();
}

void Server::run(unsigned int thread_count) {
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < thread_count; ++i) {
        threads.emplace_back([this]() {
            io_context_.run();
        });
    }

    std::cout << "Server running on port "
              << acceptor_.local_endpoint().port()
              << " with " << thread_count << " threads...\n";

    for (auto& t : threads) {
        t.join();
    }
}

void Server::start_accept() {
    auto socket = std::make_shared<asio::ip::tcp::socket>(io_context_);

    acceptor_.async_accept(*socket, [this, socket](std::error_code ec) {
        if (!ec) {
            std::cout << "New connection from " << socket->remote_endpoint() << "\n";
            start_session(socket);
        } else {
            std::cerr << "Accept error: " << ec.message() << "\n";
        }
        start_accept();
    });
}

void Server::start_session(std::shared_ptr<asio::ip::tcp::socket> socket) {
    auto buffer = std::make_shared<std::array<char, 1024>>();

    socket->async_read_some(asio::buffer(*buffer),
        [this, socket, buffer](std::error_code ec, std::size_t length) {
            if (!ec) {
                std::string message(buffer->data(), length);
                std::cout << "Received: " << message << "\n";
                start_session(socket);
            } else {
                std::cerr << "Read error or disconnect: " << ec.message() << "\n";
            }
        });
}
