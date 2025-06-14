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
                std::cout << "\nMenu: data | log | refresh\n> ";
                std::string input;
                std::getline(std::cin, input);
                if (input == "data") {
                    while (true) {
                        std::cout << "\nData Menu: show | sort_level | sort_time | filter_id | filter_status | back\n> ";
                        std::string dinput;
                        std::getline(std::cin, dinput);
                        if (dinput == "show") {
                            server->showTankLogTable();
                        } else if (dinput == "sort_level") {
                            auto all_data = server->getDataManager()->loadAllReadingsFromBinary();
                            DataOps::mergeSort(all_data, DataOps::Comparators::byLevelAsc);
                            std::cout << "Data tank diurutkan berdasarkan level (naik):\n";
                            int idx = 1;
                            for (const auto& d : all_data) {
                                std::cout << idx++ << ". " << d.tank_id << " | " << d.level << " | " << d.get_status_string() << " | " << d.get_formatted_time() << std::endl;
                            }
                        } else if (dinput == "sort_time") {
                            auto all_data = server->getDataManager()->loadAllReadingsFromBinary();
                            DataOps::mergeSort(all_data, DataOps::Comparators::byTimestampAsc);
                            std::cout << "Data tank diurutkan berdasarkan waktu (naik):\n";
                            int idx = 1;
                            for (const auto& d : all_data) {
                                std::cout << idx++ << ". " << d.tank_id << " | " << d.level << " | " << d.get_status_string() << " | " << d.get_formatted_time() << std::endl;
                            }
                        } else if (dinput == "filter_id") {
                            std::cout << "Masukkan tank_id: ";
                            std::string tid; std::getline(std::cin, tid);
                            auto all_data = server->getDataManager()->loadAllReadingsFromBinary();
                            auto filtered = DataOps::findByTankId(all_data, tid);
                            std::cout << "Data untuk tank_id '" << tid << "':\n";
                            int idx = 1;
                            for (const auto& d : filtered) {
                                std::cout << idx++ << ". " << d.tank_id << " | " << d.level << " | " << d.get_status_string() << " | " << d.get_formatted_time() << std::endl;
                            }
                        } else if (dinput == "filter_status") {
                            std::cout << "Masukkan status (NORMAL/CRIT_LOW/CRIT_HIGH): ";
                            std::string s; std::getline(std::cin, s);
                            TankStatus st;
                            if (s == "NORMAL") st = TankStatus::NORMAL;
                            else if (s == "CRIT_LOW") st = TankStatus::CRIT_LOW;
                            else if (s == "CRIT_HIGH") st = TankStatus::CRIT_HIGH;
                            else { std::cout << "Status tidak valid!\n"; continue; }
                            auto all_data = server->getDataManager()->loadAllReadingsFromBinary();
                            auto filtered = DataOps::findByStatus(all_data, st);
                            std::cout << "Data dengan status '" << s << "':\n";
                            int idx = 1;
                            for (const auto& d : filtered) {
                                std::cout << idx++ << ". " << d.tank_id << " | " << d.level << " | " << d.get_status_string() << " | " << d.get_formatted_time() << std::endl;
                            }
                        } else if (dinput == "back") {
                            break;
                        }
                    }
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
