#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "Tank.h"         // Menggunakan TankData dari Budi
#include "common_types.h" // Untuk CriticalEvent
#include "config.h"       // Untuk LOW_THRESHOLD, HIGH_THRESHOLD (dari #define)
#include <string>
#include <vector>
#include <fstream>        // Untuk file stream

class DataManager {
public:
    // Konstruktor dengan path file penyimpanan
    DataManager(const std::string& binary_file, const std::string& json_report_file);

    // Menerima data baru, memproses, dan langsung menyimpannya
    void processAndStoreReading(const TankData& new_reading);

    // Memuat semua data dari file biner (sederhana, untuk dilihat)
    std::vector<TankData> loadAllReadingsFromBinary();

    // Mengekspor semua kondisi kritis yang tercatat ke JSON
    void exportAllCriticalEventsToJson();

private:
    std::string binary_file_path;
    std::string json_report_file_path;

    // Untuk menyimpan kondisi kritis di memori sebelum diekspor
    std::vector<CriticalEvent> critical_event_log;

    // Helper internal
    void appendToBinaryFile(const TankData& data_to_save);
    void logCriticalEvent(const TankData& critical_data);
};

#endif