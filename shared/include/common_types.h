#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include "Tank.h"
#include <string>
#include <vector>
#include "nlohmann/json.hpp" // Untuk JSON

using json = nlohmann::json;

// Struktur  untuk laporan kejadian kritis
struct CriticalEvent {
    std::string tank_id_event;
    std::string status_event; // "CRIT_LOW", "CRIT_HIGH"
    std::string timestamp_event; // Waktu kejadian dalam format string
    double level_at_event;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(CriticalEvent, tank_id_event, status_event, timestamp_event, level_at_event)
};
#endif //COMMON_TYPES_H
