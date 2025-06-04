#include "event_processor.h"
// #include <iostream> // Hanya jika diperlukan untuk debugging

namespace EventProcessor {

    void sortEventsByTimestamp(std::vector<CriticalEvent>& events) {
        std::sort(events.begin(), events.end(), 
            [](const CriticalEvent& a, const CriticalEvent& b) {
                return a.timestamp < b.timestamp;
            }
        );
    }

    std::vector<CriticalEvent> findEventsByTankId(const std::vector<CriticalEvent>& all_events, const std::string& target_tank_id) {
        std::vector<CriticalEvent> found_events;
        std::copy_if(all_events.begin(), all_events.end(), 
                     std::back_inserter(found_events), 
                     [&target_tank_id](const CriticalEvent& event) {
                         return event.tank_id == target_tank_id;
                     }
        );
        return found_events;
    }

    std::vector<CriticalEvent> findEventsInTimeRange(const std::vector<CriticalEvent>& all_events, long long start_time, long long end_time) {
        std::vector<CriticalEvent> found_events;
        std::copy_if(all_events.begin(), all_events.end(),
                     std::back_inserter(found_events),
                     [start_time, end_time](const CriticalEvent& event) {
                         return event.timestamp >= start_time && event.timestamp <= end_time;
                     }
        );
        return found_events;
    }

} // namespace EventProcessor
