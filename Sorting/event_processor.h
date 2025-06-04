#ifndef EVENT_PROCESSOR_H
#define EVENT_PROCESSOR_H

#include "common_types.h" // Diperlukan untuk CriticalEvent
#include <vector>
#include <string>
#include <algorithm>

namespace EventProcessor {

    // Mengurutkan vector CriticalEvent berdasarkan timestamp (ascending)
    void sortEventsByTimestamp(std::vector<CriticalEvent>& events);

    // Mencari kejadian kritis berdasarkan ID tangki
    std::vector<CriticalEvent> findEventsByTankId(const std::vector<CriticalEvent>& all_events, const std::string& target_tank_id);

    // Mencari kejadian kritis dalam rentang waktu tertentu (inklusif)
    std::vector<CriticalEvent> findEventsInTimeRange(const std::vector<CriticalEvent>& all_events, long long start_time, long long end_time);

} // namespace EventProcessor

#endif // EVENT_PROCESSOR_H