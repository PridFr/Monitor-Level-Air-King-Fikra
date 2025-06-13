#include "../include/Server.hpp"
#include "data_operations.h"
#include <iostream>
#include <thread>
#include <set>

int main() {
    try {
        auto server = std::make_shared<Server>();
        server->socketStart();
        server->serverListen();

        // Tampilkan jumlah tank unik dari file biner
        auto all_data = server->getDataManager()->loadAllReadingsFromBinary();
        std::set<std::string> unique_ids;
        for (const auto& d : all_data) unique_ids.insert(d.tank_id);
        std::cout << "Jumlah tank unik yang pernah terkoneksi: " << unique_ids.size() << std::endl;
        server->addActivityLog("Server started. " + std::to_string(unique_ids.size()) + " unique tanks found.");

        // Thread menu interaktif
        std::thread([server](){
            while (true) {
                std::cout << "\nKetik 'data' untuk tabel data tank, 'log' untuk log aktivitas, 'refresh' untuk reload data: ";
                std::string input;
                std::getline(std::cin, input);
                if (input == "data") {
                    server->showTankLogTable();
                } else if (input == "log") {
                    server->showActivityLog();
                } else if (input == "refresh") {
                    auto all_data = server->getDataManager()->loadAllReadingsFromBinary();
                    std::set<std::string> unique_ids;
                    for (const auto& d : all_data) unique_ids.insert(d.tank_id);
                    std::cout << "Jumlah tank unik yang pernah terkoneksi (refresh): " << unique_ids.size() << std::endl;
                    server->addActivityLog("Data refreshed. " + std::to_string(unique_ids.size()) + " unique tanks found.");
                }
            }
        }).detach();

        std::cout << "Server started. Waiting for connections...\n";
        server->acceptClient();

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
