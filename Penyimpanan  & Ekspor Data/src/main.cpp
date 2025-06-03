#include "data_manager.h" // Untuk DataManager
#include "Tank.h"         // Untuk membuat objek TankData
#include <iostream>
#include <vector>        // Untuk menampung hasil loadAllReadingsFromBinary
#include <thread>        // Untuk std::this_thread::sleep_for
#include <chrono>        // Untuk std::chrono::seconds


// Fungsi  membuat data TankData
TankData createSampleTankData(const std::string& id, double lvl) {
    TankData data;
    data.tank_id = id;
    data.level = lvl;
    data.timestamp = std::time(nullptr); // Timestamp saat ini
    data.update_status();
    return data;
}

int main() {
    std::cout << "Aplikasi Fundamental Monitoring Level Air Dimulai..." << std::endl;

    // Inisialisasi DataManager
    DataManager data_manager("tank_sensor_data.dat", "critical_report_output.json");

    // Simulasikan penerimaan beberapa data dan proses
    std::cout << "\n--- Memproses Data Baru ---" << std::endl;
    data_manager.processAndStoreReading(createSampleTankData("Tank-01", 15.5));  // Akan CRIT_LOW
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Jeda kecil antar data
    data_manager.processAndStoreReading(createSampleTankData("Tank-02", 50.0));  // Akan NORMAL
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    data_manager.processAndStoreReading(createSampleTankData("Tank-01", 85.0));  // Akan CRIT_HIGH
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    data_manager.processAndStoreReading(createSampleTankData("Tank-03", 5.0));   // Akan CRIT_LOW

    // Muat semua data yang telah disimpan  untuk verifikasi
    std::cout << "\n--- Memuat Semua Data dari File Biner ---" << std::endl;
    std::vector<TankData> loaded_data = data_manager.loadAllReadingsFromBinary();
    for (const auto& data : loaded_data) {
        std::cout << "Loaded: ID=" << data.tank_id
                  << ", Lvl=" << data.level
                  << ", Sts=" << data.get_status_string()
                  << ", Time=" << data.get_formatted_time() << std::endl;
    }

    // Ekspor laporan kejadian kritis
    std::cout << "\n--- Mengekspor Laporan Kejadian Kritis ---" << std::endl;
    data_manager.exportAllCriticalEventsToJson();

    std::cout << "\nAplikasi Fundamental Monitoring Level Air Selesai." << std::endl;
    std::cout << "Periksa file 'tank_sensor_data.dat' dan 'critical_report_output.json'." << std::endl;

    return 0;
}