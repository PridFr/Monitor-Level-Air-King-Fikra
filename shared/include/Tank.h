#ifndef TANK_HPP
#define TANK_HPP

#include <string>
#include <ctime>

typedef struct Tank {
    std::string tank_id;
    std::time_t timestamp;
    float level;

    // constructor
    Tank(const std::string& id, std::time_t ts, float lvl)
        : tank_id(id), timestamp(ts), level(lvl) {};
}Tank;

#endif 
