#include "data_manager.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>

DataManager::DataManager(const std::string& binary_storage_path,
                         const std::string& json_report_path,
                         Config& app_config)
    : binary_storage_path_(binary_storage_path),
      json_report_path_(json_report_path),
      config_(app_config) {
    loadFromBinary();
    std::cout << "DataManager: Initialized." << std::endl;
}

DataManager::~DataManager() {
    if (unsaved_readings_count_ > 0) {
        saveNewReadingsToBinary();
    }
    std::cout << "DataManager: Destroyed." << std::endl;
}

void DataManager::addReading(const Tank& base_tank_reading) {
    std::lock_guard<std::mutex> lock(data_mutex_);

    ProcessedTankReading reading_to_store(base_tank_reading); // Buat ProcessedTankReading dari Tank

    TankThresholds th = config_.getThresholds(reading_to_store.tank_id);
    if (reading_to_store.level > th.overflowLevel) {
        reading_to_store.status = TankStatus::OVERFLOW_CRITICAL;
    } else if (reading_to_store.level < th.dryLevel) {
        reading_to_store.status = TankStatus::DRY_CRITICAL;
    } else {
        reading_to_store.status = TankStatus::NORMAL;
    }

    readings_buffer_.push_back(reading_to_store);
    unsaved_readings_count_++;

    std::cout << "DM: Added " << reading_to_store.tank_id << " Lvl:" << reading_to_store.level
              << " Sts:" << tankStatusToString(reading_to_store.status) << std::endl;

    if (reading_to_store.status != TankStatus::NORMAL && reading_to_store.status != TankStatus::UNKNOWN) {
        critical_periods_log_.push_back({
            reading_to_store.tank_id,
            tankStatusToString(reading_to_store.status),
            reading_to_store.timestamp,
            reading_to_store.timestamp,
            reading_to_store.level
        });
    }

    if (unsaved_readings_count_ >= SAVE_BATCH_SIZE) {
        saveNewReadingsToBinary();
    }
}

void DataManager::loadFromBinary() {
    std::lock_guard<std::mutex> lock(data_mutex_);
    std::ifstream ifs(binary_storage_path_, std::ios::binary);
    if (!ifs.is_open()) return;

    readings_buffer_.clear();
    critical_periods_log_.clear();
    unsaved_readings_count_ = 0;

    ProcessedTankReading reading_from_file;
    while (ifs.peek() != EOF && (reading_from_file.deserialize(ifs), ifs.gcount() > 0 && !ifs.fail())) {
        // Re-evaluate status on load based on current config, or trust stored status
        TankThresholds th = config_.getThresholds(reading_from_file.tank_id);
        if (reading_from_file.level > th.overflowLevel) reading_from_file.status = TankStatus::OVERFLOW_CRITICAL;
        else if (reading_from_file.level < th.dryLevel) reading_from_file.status = TankStatus::DRY_CRITICAL;
        else reading_from_file.status = TankStatus::NORMAL;

        readings_buffer_.push_back(reading_from_file);
        if (reading_from_file.status != TankStatus::NORMAL && reading_from_file.status != TankStatus::UNKNOWN) {
             critical_periods_log_.push_back({reading_from_file.tank_id, tankStatusToString(reading_from_file.status), reading_from_file.timestamp, reading_from_file.timestamp, reading_from_file.level});
        }
    }
    std::cout << "DM: Loaded " << readings_buffer_.size() << " readings." << std::endl;
}

void DataManager::saveNewReadingsToBinary() {
    if (unsaved_readings_count_ == 0) return;
    std::ofstream ofs(binary_storage_path_, std::ios::binary | std::ios::app);
    if (!ofs.is_open()) { std::cerr << "DM: Error opening binary for append." << std::endl; return; }

    size_t start_idx = readings_buffer_.size() - unsaved_readings_count_;
    for (size_t i = start_idx; i < readings_buffer_.size(); ++i) {
        readings_buffer_[i].serialize(ofs);
    }
    std::cout << "DM: Saved " << unsaved_readings_count_ << " new readings." << std::endl;
    unsaved_readings_count_ = 0;
}

void DataManager::exportCriticalPeriodsToJson() {
    std::lock_guard<std::mutex> lock(data_mutex_);
    if (critical_periods_log_.empty()) return;
    json json_export_data = critical_periods_log_;
    std::ofstream ofs(json_report_path_);
    if (!ofs.is_open()) { std::cerr << "DM: Error opening JSON report." << std::endl; return; }
    ofs << std::setw(4) << json_export_data << std::endl;
    std::cout << "DM: Exported " << critical_periods_log_.size() << " critical periods." << std::endl;
}

std::vector<ProcessedTankReading> DataManager::getAllReadingsSortedByTime() const {
    std::lock_guard<std::mutex> lock(data_mutex_);
    std::vector<ProcessedTankReading> copy(readings_buffer_.begin(), readings_buffer_.end());
    std::sort(copy.begin(), copy.end(), [](const auto& a, const auto& b){ return a.timestamp < b.timestamp; });
    return copy;
}

std::vector<ProcessedTankReading> DataManager::getReadingsForTank(const std::string& tankId) const {
    std::lock_guard<std::mutex> lock(data_mutex_);
    std::vector<ProcessedTankReading> result;
    for(const auto& r : readings_buffer_) if(r.tank_id == tankId) result.push_back(r);
    std::sort(result.begin(), result.end(), [](const auto& a, const auto& b){ return a.timestamp < b.timestamp; });
    return result;
}

std::vector<CriticalPeriod> DataManager::getAllCriticalPeriods() const {
    std::lock_guard<std::mutex> lock(data_mutex_);
    return critical_periods_log_;
}