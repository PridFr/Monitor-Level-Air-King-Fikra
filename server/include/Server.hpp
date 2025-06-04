#ifndef SERVER_H
#define SERVER_H

#include <memory>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <sstream>
#include "../shared/include/Tank.h"

// Forward declarations
class ThreadPool;
class Session;
//Class thread pool
class ThreadPool {
private:
    std::vector<std::thread> workers; //deklarasi worker
    std::queue<std::function<void()>> tasks; //antrian tugas
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;

public:
    explicit ThreadPool(size_t threads);
    ~ThreadPool();
    
    template<class F>
    void enqueue(F&& f) { //fungsi menambahkan tugas baru ke thread pool
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.emplace(std::forward<F>(f));
        }
        condition.notify_one();
    }
};

class Session {
private:
    SOCKET client_socket;
    static const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    std::shared_ptr<Server> server_ptr;

    // Helper functions untuk parsing dan handling data
    TankData parse_data(const std::string& data);
    void handle_tank_data(const TankData& data);

public:
    Session(SOCKET socket, std::shared_ptr<Server> server);
    ~Session();
    void handle_client();
};

class Server : public std::enable_shared_from_this<Server> {
private:
    WSADATA wsaData;
    SOCKET server_soc;
    struct addrinfo* result;
    struct addrinfo hints;
    int result_idx;
    std::unique_ptr<ThreadPool> pool;

    // Collection untuk menyimpan data tank
    std::vector<TankData> tank_data_collection;
    mutable std::mutex data_mutex;  // Untuk thread-safe access ke collection

public:
    Server();
    ~Server();
    void socketStart();
    void serverListen();
    void acceptClient();

    
    // Fungsi untuk akses data
    void add_tank_data(const TankData& data);
    std::vector<TankData> get_tank_data() const;
};

#endif