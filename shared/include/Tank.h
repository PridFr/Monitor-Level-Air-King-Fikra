#ifndef TANK_H
#define TANK_H

#include <string>
#include "./config.h"
#include <stdexcept>
#include <ctime>

// Define threshold constants

// Tank status enum
enum class TankStatus {
    CRIT_LOW,        // Level dibawah LOW_THRESHOLD
    NORMAL,     // Level antara LOW_THRESHOLD dan HIGH_THRESHOLD
    CRIT_HIGH,       // Level diatas HIGH_THRESHOLD
    UNKNOWN     // Status tidak valid/belum diset
};

struct TankDataError : public std::runtime_error {
    explicit TankDataError(const std::string& message) 
        : std::runtime_error(message) {}
};

struct TankData {
    std::string tank_id;
    double level;
    time_t timestamp;
    TankStatus status;

    // Constructor with default values
    TankData() : level(0.0), timestamp(0), status(TankStatus::UNKNOWN) {}

    // Update status based on level
    void update_status() {
        if (level < LOW_THRESHOLD) {
            status = TankStatus::CRIT_LOW;
        } else if (level > HIGH_THRESHOLD) {
            status = TankStatus::CRIT_HIGH;
        } else {
            status = TankStatus::NORMAL;
        }
    }
    
    bool is_valid() const {
        return !tank_id.empty() && 
               level >= 0.0 && level <= 100.0 &&
               timestamp > 0 &&
               status != TankStatus::UNKNOWN;
    }
    
    // Helper function untuk format timestamp
    std::string get_formatted_time() const {
        struct tm* timeinfo;
        char buffer[80];
        
        timeinfo = localtime(&timestamp);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        return std::string(buffer);
    }

    // Helper function untuk convert status ke string
    std::string get_status_string() const {
        switch (status) {
            case TankStatus::CRIT_LOW: return "CRIT_LOW";
            case TankStatus::NORMAL: return "NORMAL";
            case TankStatus::CRIT_HIGH: return "CRIT_LOW";
            default: return "UNKNOWN";
        }
    }
};

#endif
