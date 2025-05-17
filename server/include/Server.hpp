#ifndef SERVER_H
#define SERVER_H
#define ASIO_STANDALONE
#include <asio.hpp>
//#include <config.h>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>


class Server {
public:
    Server(asio::io_context& io_context, unsigned short port);
    void run(unsigned int thread_count = std::thread::hardware_concurrency());

private:
    void start_accept();
    void start_session(std::shared_ptr<asio::ip::tcp::socket> socket);

    asio::io_context& io_context_;
    asio::ip::tcp::acceptor acceptor_;
};


#endif