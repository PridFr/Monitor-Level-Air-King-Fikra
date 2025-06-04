#include "../include/Server.hpp"
#include "../../shared/include/config.h"
#include <iostream>

ThreadPool::ThreadPool(size_t threads) : stop(false) {
    for(size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this] {
            while(true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    condition.wait(lock, [this] { 
                        return stop || !tasks.empty(); 
                    });
                    if(stop && tasks.empty()) return;
                    task = std::move(tasks.front());
                    tasks.pop();
                }
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(std::thread &worker: workers) {
        worker.join();
    }
}

Session::Session(SOCKET socket, std::shared_ptr<Server> server) 
        : client_socket(socket), server_ptr(server) {}

TankData Session::parse_data(const std::string& data) {
    TankData tank_data;
    std::vector<std::string> tokens;
    std::stringstream ss(data);
    std::string token;

    while (std::getline(ss, token, ',')) {
        tokens.push_back(token);
    }

    if (tokens.size() != 2) {  
        throw TankDataError("Invalid data format. Expected: tank_id,level");
    }

    try {
        // validasi tank_id
        if (tokens[0].empty() || tokens[0][0] != 't' || !std::isdigit(tokens[0][1])) {
            throw TankDataError("Invalid tank_id format. Expected: t<number>");
        }
        tank_data.tank_id = tokens[0];

        // validasi level
        tank_data.level = std::stod(tokens[1]);
        if (tank_data.level < 0.0 || tank_data.level > 100.0) {
            throw TankDataError("Tank level must be between 0 and 100");
        }

        // Set timestamp dan update status
        tank_data.timestamp = std::time(nullptr);
        tank_data.update_status();

    } catch (const std::invalid_argument&) {
        throw TankDataError("Invalid number format for level");
    } catch (const std::out_of_range&) {
        throw TankDataError("Level value out of range");
    }

    return tank_data;
}

void Session::handle_tank_data(const TankData& data) {
    std::cout << "Tank ID: " << data.tank_id 
              << "\nLevel: " << data.level 
              << "\nStatus: " << data.get_status_string()
              << "\nTime: " << data.get_formatted_time() << std::endl;
}

void Session::handle_client() {
    while (true) {
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received > 0) {
            std::string received_data(buffer, bytes_received);
            std::cout << "Raw data received: " << received_data << std::endl;
            
            std::string response;
            try {
                TankData tank_data = parse_data(received_data);
                
                if (tank_data.is_valid()) {
                    handle_tank_data(tank_data);
                    server_ptr->getDataManager()->processAndStoreReading(tank_data);
                    server_ptr->add_tank_data(tank_data);
                    response = "Data valid and processed";
                } else {
                    response = "Data validation failed";
                }
            }
            catch (const TankDataError& e) {
                std::cerr << "Data error: " << e.what() << std::endl;
                response = std::string("Error: ") + e.what();
            }
            catch (const std::exception& e) {
                std::cerr << "Unexpected error: " << e.what() << std::endl;
                response = "Internal server error";
            }
            
            send(client_socket, response.c_str(), response.length(), 0);
        }
        else if (bytes_received == 0 || bytes_received == SOCKET_ERROR) {
            break;
        }
    }
    closesocket(client_socket);
}

Session::~Session() {
    closesocket(client_socket);
}

Server::Server() : server_soc(INVALID_SOCKET), result(nullptr), pool(std::make_unique<ThreadPool>(4)) {
    result_idx = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (result_idx != 0) {
        throw std::runtime_error("WSAStartup failed with error: " + std::to_string(result_idx));
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    result_idx = getaddrinfo(NULL, PORT, &hints, &result);
    if (result_idx != 0) {
        WSACleanup();
        throw std::runtime_error("getaddrinfo failed with error: " + std::to_string(result_idx));
    }
    data_manager = std::make_unique<DataManager>("tank_data.bin", "critical_report.json");
}

void Server::socketStart() {
    server_soc = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (server_soc == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return;
    }

    result_idx = bind(server_soc, result->ai_addr, (int)result->ai_addrlen);
    if (result_idx == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(server_soc);
        WSACleanup();
        return;
    }
}

void Server::serverListen() {
    if (listen(server_soc, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed with error: %ld\n", WSAGetLastError());
        closesocket(server_soc);
        WSACleanup();
        return;
    }
}

void Server::acceptClient() {
    while(true) {
        SOCKET client_socket = accept(server_soc, NULL, NULL);
        if (client_socket == INVALID_SOCKET) {
            printf("accept failed: %d\n", WSAGetLastError());
            continue;
        }

        std::cout << "New client connected!" << std::endl;
        
        pool->enqueue([self = shared_from_this(), client_socket]() {
            Session session(client_socket,self);
            session.handle_client();
        });
    }
}

void Server::add_tank_data(const TankData& data) {
    std::lock_guard<std::mutex> lock(data_mutex);
    tank_data_collection.push_back(data);
}

std::vector<TankData> Server::get_tank_data() const {
    std::lock_guard<std::mutex> lock(data_mutex);
    return tank_data_collection;
}

Server::~Server() {
    if (server_soc != INVALID_SOCKET) {
        closesocket(server_soc);
    }
    if (result != nullptr) {
        freeaddrinfo(result);
    }
    WSACleanup();
}




