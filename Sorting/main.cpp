#include "event_processor.h"
#include "common_types.h"
#include <iostream>
#include <vector>

// Fungsi untuk menampilkan event (helper)
void printEvents(const std::string& title, const std::vector<CriticalEvent>& events) {
    std::cout << title << std::endl;
    if (events.empty()) {
        std::cout << "  (Tidak ada kejadian)" << std::endl;
        return;
    }
    for (const auto& event : events) {
        std::cout << "  ID: " << event.tank_id 
                  << ", TS: " << event.timestamp 
                  << ", Lvl: " << event.level 
                  << ", Type: " << event.event_type << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    std::vector<CriticalEvent> events;
    events.push_back(CriticalEvent("Tank01", 1678886400LL, 15.0, "LOW_LEVEL"));
    events.push_back(CriticalEvent("Tank02", 1678883000LL, 95.0, "HIGH_LEVEL"));
    events.push_back(CriticalEvent("Tank01", 1678880000LL, 12.0, "LOW_LEVEL"));
    events.push_back(CriticalEvent("Tank03", 1678890000LL, 50.0, "CRITICAL"));

    printEvents("Kejadian Awal:", events);

    EventProcessor::sortEventsByTimestamp(events);
    printEvents("Kejadian Setelah Diurutkan:", events);

    std::vector<CriticalEvent> tank01_events = EventProcessor::findEventsByTankId(events, "Tank01");
    printEvents("Kejadian Kritis untuk Tank01:", tank01_events);

    long long start_ts = 1678883000LL;
    long long end_ts = 1678886400LL;
    std::vector<CriticalEvent> time_range_events = EventProcessor::findEventsInTimeRange(events, start_ts, end_ts);
    printEvents("Kejadian Kritis antara timestamp " + std::to_string(start_ts) + " dan " + std::to_string(end_ts) + ":", time_range_events);
    
    return 0;
}
