#include "data_manager.h" // Menggunakan DataManager
#include "config.h"       // Menggunakan Config dari shared/
#include "Tank.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <csignal>

// Variabel global untuk mengontrol loop utama aplikasi
std::atomic<bool> app_is_running(true);

// Signal handler untuk Ctrl+C
void interruptSignalHandler(int signum) {
    std::cout << "\nSinyal interrupt (" << signum << ") diterima. Memulai proses shutdown..." << std::endl;
    app_is_running = false;
}

// Fungsi simulasi untuk mengirim data ke DataManager
// Dalam sistem nyata, panggilan ke dm.addReading() akan datang dari ClientHandler (Server)
void simulateTankDataProducer(DataManager& dm, const std::string& tank_id, double initial_level, int simulation_duration_seconds) {
    std::cout << "Simulasi untuk tangki '" << tank_id << "' dimulai." << std::endl;
    auto simulation_start_time = std::chrono::steady_clock::now();
    double current_level = initial_level;
    bool level_is_increasing = true;

    while (app_is_running &&
           std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - simulation_start_time).count() < simulation_duration_seconds) {

        // Buat objek Tank dasar (yang akan diparsing oleh Server dari data Client Farrij)
        // Status awal bisa diabaikan atau UNKNOWN, karena DataManager akan menentukannya.
        Tank raw_tank_data;
        raw_tank_data.tank_id = tank_id;
        raw_tank_data.timestamp = std::chrono::system_clock::now();
        raw_tank_data.level = current_level;
        // raw_tank_data.status biarkan default atau TankStatus::UNKNOWN

        dm.addReading(raw_tank_data); // Server (via ClientHandler) akan memanggil ini

        // Logika sederhana untuk mengubah level air dalam simulasi
        if (level_is_increasing) {
            current_level += 3.5; // Naikkan level
            if (current_level > 110.0) level_is_increasing = false; // Batas atas, lalu turun
        } else {
            current_level -= 4.0; // Turunkan level
            if (current_level < 0.0) level_is_increasing = true;   // Batas bawah, lalu naik
        }

        std::this_thread::sleep_for(std::chrono::seconds(1)); // Interval pengiriman data
    }
    std::cout << "Simulasi untuk tangki '" << tank_id << "' selesai." << std::endl;
}

int main() {
    // Mendaftarkan signal handler untuk SIGINT (Ctrl+C)
    signal(SIGINT, interruptSignalHandler);

    std::cout << "Aplikasi Monitoring Level Air Dimulai..." << std::endl;

    // 1. Inisialisasi Konfigurasi (Thresholds)
    Config app_config;
    std::string config_file = "app_config.json"; // Nama file konfigurasi
    if (!app_config.loadFromFile(config_file)) {
        std::cout << "File konfigurasi '" << config_file << "' tidak ditemukan. Membuat konfigurasi default." << std::endl;
        app_config.setDefaultThresholds(90.0, 15.0);          // Threshold default
        app_config.setThresholds("Tank-A1", 85.0, 20.0);     // Threshold spesifik untuk Tank-A1
        app_config.setThresholds("Tank-B2", 95.0, 10.0);     // Threshold spesifik untuk Tank-B2
        if (!app_config.saveToFile(config_file)) {
            std::cerr << "Gagal menyimpan konfigurasi default ke '" << config_file << "'." << std::endl;
        }
    }

    // 2. Inisialisasi DataManager
    //    Menyediakan path untuk penyimpanan data biner dan laporan JSON, serta objek config.
    DataManager data_manager("water_tank_data.dat", "critical_events_report.json", app_config);
    std::cout << "\nMemulai simulasi pengiriman data dari beberapa tangki." << std::endl;
    std::cout << "Tekan Ctrl+C untuk menghentikan aplikasi dengan aman." << std::endl;

    // 3. Jalankan simulasi pengiriman data di thread terpisah
    //    Ini untuk menguji DataManager menerima data dari berbagai sumber.
    std::thread producer1(simulateTankDataProducer, std::ref(data_manager), "Tank-A1", 10.0, 20); // Simulasikan selama 20 detik
    std::thread producer2(simulateTankDataProducer, std::ref(data_manager), "Tank-B2", 105.0, 25); // Mulai dari level tinggi
    std::thread producer3(simulateTankDataProducer, std::ref(data_manager), "Tank-C3", 50.0, 15);  // Tangki dengan threshold default

    // 4. Loop utama aplikasi
    //    Aplikasi akan tetap berjalan di sini, menunggu sinyal untuk berhenti.
    while (app_is_running) {
        // Bisa melakukan pekerjaan latar belakang lain di sini jika perlu
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Cek kondisi berhenti setiap 0.5 detik
    }

    // 5. Proses Shutdown Aplikasi
    std::cout << "\nMemulai proses shutdown..." << std::endl;
    std::cout << "Menunggu semua thread simulasi produsen data selesai..." << std::endl;
    if (producer1.joinable()) producer1.join();
    if (producer2.joinable()) producer2.join();
    if (producer3.joinable()) producer3.join();
    std::cout << "Mengekspor laporan kondisi kritis terakhir..." << std::endl;
    data_manager.exportCriticalPeriodsToJson();

    std::cout << "Aplikasi Monitoring Level Air Selesai." << std::endl;

    return 0;
}