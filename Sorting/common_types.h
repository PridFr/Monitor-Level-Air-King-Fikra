#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <string>
// #include <ctime> // Atau header lain untuk tipe data timestamp jika diperlukan

struct CriticalEvent {
    std::string tank_id;
    long long timestamp; // Misal: epoch time
    double level;
    std::string event_type;

    CriticalEvent(std::string id, long long ts, double lvl, std::string type)
        : tank_id(std::move(id)), timestamp(ts), level(lvl), event_type(std::move(type)) {}
};

// Jika Anda juga memerlukan TankData, definisikan di sini atau di Tank.h
// struct TankData {
//     std::string tank_id;
//     long long timestamp;
//     double level;
//     // ...
// };

#endif // COMMON_TYPES_H
