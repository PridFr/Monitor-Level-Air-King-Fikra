#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <tank.h>
#include<config.h>
#include <string>
#include <vector>
#include <deque>
#include <mutex>

class DataManager {
public:
    //Constructor
    DataManager(const std::string& binary_storage_path,
        const std::string& json_report_path,
        config& app_config);
    ~DataManager();

    //Read data yang telah ada di tank.h
    void addReading(const Tank& reading); // nanti diskusi lagi soalnya belum jadi tank.h tanggal 21 Mei 2025

    //Memuat data historis dari file biner
    void loadFromBinary();

    //Append data buffer ke file binner
    void saveNewReadingsToBinary();

    //Ekspor data yang memiliki kondisi kritis ke file JSON
    void exportCriticalDataToJson();

private:
    std::string binary_storage_path_;
    std::string  json_report_path_;
    Config& config_; //Referensi ke objek config tapi masih belum ada

    //buffer utama buat disimpan ke objek tank
    std::deque<Tank> reading_buffer_;//Ambil tipe data dari tank.h
    std::vector<CriticalPeriod> critical_periods_; //log kondisi kritis tapi 21 mei tank h nyta belum di update ke main

    //mutex untuk menghindari kesalahan thread
    mutable std::mutex mutex_;

    size_t unsaved_readings_count_ = 0;
    const size_t SAVE_BATCH_SIZE = 10;

    void checkForCriticalPeriod(const Tank& reading);


};
#endif //DATA_MANAGER_H
