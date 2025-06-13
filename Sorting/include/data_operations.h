#ifndef DATA_OPERATIONS_H
#define DATA_OPERATIONS_H

#include "Tank.h"
#include <vector>
#include <string>
#include <functional>
#include <ctime>

namespace DataOps {

    void mergeSort(std::vector<TankData>& data,
                   const std::function<bool(const TankData&, const TankData&)>& comparator);

    std::vector<TankData> findByTankId(const std::vector<TankData>& all_data, const std::string& target_id);
    std::vector<TankData> findByTimestampRange(const std::vector<TankData>& all_data,
                                               time_t start_time,
                                               time_t end_time);
    std::vector<TankData> findByStatus(const std::vector<TankData>& all_data, TankStatus target_status);

    namespace Comparators {
        inline bool byTimestampAsc(const TankData& a, const TankData& b) {
            return a.timestamp < b.timestamp;
        }
        inline bool byTimestampDesc(const TankData& a, const TankData& b) {
            return a.timestamp > b.timestamp;
        }
        inline bool byLevelAsc(const TankData& a, const TankData& b) {
            return a.level < b.level;
        }
        inline bool byTankIdAsc(const TankData& a, const TankData& b) {
            if (a.tank_id == b.tank_id) {
                return a.timestamp < b.timestamp;
            }
            return a.tank_id < b.tank_id;
        }
    }
}

#endif //DATA_OPERATIONS_H
