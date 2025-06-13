#include "data_manager.h"
#include "config.h" // Untuk LOW_THRESHOLD, HIGH_THRESHOLD
#include <iostream>
#include <vector> // Diperlukan di beberapa tempat

// Konstruktor
DataManager::DataManager(const std::string& binary_file, const std::string& json_report_file)
    : binary_file_path(binary_file), json_report_file_path(json_report_file) {
    std::cout << "DataManager: Initialized. Storage: " << binary_file_path
              << ", Report: " << json_report_file_path << std::endl;
    std::cout << "Using LOW_THRESHOLD: " << LOW_THRESHOLD << ", HIGH_THRESHOLD: " << HIGH_THRESHOLD << std::endl;
}

// Menyimpan satu TankData ke akhir file biner
void DataManager::appendToBinaryFile(const TankData& data_to_save) {
    std::ofstream bin_out(binary_file_path, std::ios::binary | std::ios::app);
    if (!bin_out.is_open()) {
        std::cerr << "DM_ERROR: Cannot open binary file for append: " << binary_file_path << std::endl;
        return;
    }

    // Serialisasi manual sederhana (sesuai dengan TankData)
    size_t id_len = data_to_save.tank_id.length();
    bin_out.write(reinterpret_cast<const char*>(&id_len), sizeof(id_len));
    bin_out.write(data_to_save.tank_id.c_str(), id_len);
    bin_out.write(reinterpret_cast<const char*>(&data_to_save.level), sizeof(data_to_save.level));
    bin_out.write(reinterpret_cast<const char*>(&data_to_save.timestamp), sizeof(data_to_save.timestamp));
    // Simpan status sebagai integer dasarnya
    auto status_val = static_cast<std::underlying_type_t<TankStatus>>(data_to_save.status);
    bin_out.write(reinterpret_cast<const char*>(&status_val), sizeof(status_val));

    bin_out.close();
}

// Mencatat kejadian kritis ke log di memori
void DataManager::logCriticalEvent(const TankData& critical_data) {
    CriticalEvent event;
    event.tank_id_event = critical_data.tank_id;
    event.status_event = critical_data.get_status_string(); // Gunakan helper dari TankData
    event.timestamp_event = critical_data.get_formatted_time(); // Gunakan helper dari TankData
    event.level_at_event = critical_data.level;
    critical_event_log.push_back(event);
    // Tidak ada cout di sini
}

// Menerima data, menentukan status, menyimpan, dan log jika kritis
void DataManager::processAndStoreReading(const TankData& raw_reading) {
    TankData current_reading = raw_reading; // Buat salinan untuk dimodifikasi

    // DataManager menentukan status akhir berdasarkan threshold dari config.h
    if (current_reading.level < LOW_THRESHOLD) {
        current_reading.status = TankStatus::CRIT_LOW;
    } else if (current_reading.level > HIGH_THRESHOLD) {
        current_reading.status = TankStatus::CRIT_HIGH;
    } else {
        current_reading.status = TankStatus::NORMAL;
    }

    appendToBinaryFile(current_reading); // Langsung simpan ke file biner

    if (current_reading.status == TankStatus::CRIT_LOW || current_reading.status == TankStatus::CRIT_HIGH) {
        logCriticalEvent(current_reading);
    }
    // Tidak ada cout di sini
}

// Memuat semua data dari file biner (sangat sederhana)
std::vector<TankData> DataManager::loadAllReadingsFromBinary() {
    std::vector<TankData> all_data;
    std::ifstream bin_in(binary_file_path, std::ios::binary);
    if (!bin_in.is_open()) {
        return all_data; // Kembalikan vektor kosong
    }

    while (bin_in.peek() != EOF) {
        TankData temp_data;
        size_t id_len;

        bin_in.read(reinterpret_cast<char*>(&id_len), sizeof(id_len));
        if (bin_in.gcount() != sizeof(id_len)) break;

        std::vector<char> id_buf(id_len);
        bin_in.read(id_buf.data(), id_len);
        if (static_cast<size_t>(bin_in.gcount()) != id_len) break;
        temp_data.tank_id.assign(id_buf.data(), id_len);

        bin_in.read(reinterpret_cast<char*>(&temp_data.level), sizeof(temp_data.level));
        if (bin_in.gcount() != sizeof(temp_data.level)) break;
        bin_in.read(reinterpret_cast<char*>(&temp_data.timestamp), sizeof(temp_data.timestamp));
        if (bin_in.gcount() != sizeof(temp_data.timestamp)) break;

        std::underlying_type_t<TankStatus> status_val;
        bin_in.read(reinterpret_cast<char*>(&status_val), sizeof(status_val));
        if (bin_in.gcount() != sizeof(status_val)) break;
        temp_data.status = static_cast<TankStatus>(status_val);

        all_data.push_back(temp_data);
    }
    bin_in.close();
    return all_data;
}

// Mengekspor log kejadian kritis ke file JSON
void DataManager::exportAllCriticalEventsToJson() {
    if (critical_event_log.empty()) {
        std::cout << "DM_EXPORT: No critical events to export." << std::endl;
        return;
    }

    nlohmann::json json_output = critical_event_log; // Menggunakan NLOHMANN_DEFINE_TYPE_INTRUSIVE dari CriticalEvent

    std::ofstream json_out(json_report_file_path);
    if (!json_out.is_open()) {
        std::cerr << "DM_ERROR: Cannot open JSON report file for writing: " << json_report_file_path << std::endl;
        return;
    }
    json_out << std::setw(4) << json_output << std::endl; // Pretty print
    json_out.close();
    std::cout << "DM_EXPORT: Exported " << critical_event_log.size() << " critical events to " << json_report_file_path << std::endl;
}

void DataManager::showCriticalEventLogTable() {
    if (critical_event_log.empty()) {
        std::cout << "Tidak ada critical event log yang tercatat.\n";
        return;
    }
    std::cout << "-------------------------------------------------------------\n";
    std::cout << "| No | Tank ID | Level | Status     | Timestamp            |\n";
    std::cout << "-------------------------------------------------------------\n";
    int idx = 1;
    for (const auto& e : critical_event_log) {
        std::cout << "| " << idx++
                  << " | " << e.tank_id_event
                  << " | " << e.level_at_event
                  << " | " << e.status_event
                  << " | " << e.timestamp_event
                  << " |\n";
    }
    std::cout << "-------------------------------------------------------------\n";
}