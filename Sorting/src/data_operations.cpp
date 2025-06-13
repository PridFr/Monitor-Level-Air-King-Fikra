#include "data_operations.h"
#include <algorithm>

namespace DataOps {

    void merge_inplace_helper(std::vector<TankData>& arr, int l, int m, int r,
                              const std::function<bool(const TankData&, const TankData&)>& comparator) {
        int n1 = m - l + 1;
        int n2 = r - m;

        std::vector<TankData> L(n1), R(n2);

        for (int i = 0; i < n1; i++)
            L[i] = arr[l + i];
        for (int j = 0; j < n2; j++)
            R[j] = arr[m + 1 + j];

        int i = 0;
        int j = 0;
        int k = l;
        while (i < n1 && j < n2) {
            if (comparator(L[i], R[j])) {
                arr[k] = L[i];
                i++;
            } else {
                arr[k] = R[j];
                j++;
            }
            k++;
        }
        while (i < n1) {
            arr[k] = L[i];
            i++;
            k++;
        }
        while (j < n2) {
            arr[k] = R[j];
            j++;
            k++;
        }
    }

    void mergeSort_recursive_indexed(std::vector<TankData>& arr, int l, int r,
                                     const std::function<bool(const TankData&, const TankData&)>& comparator) {
        if (l >= r) {
            return;
        }
        int m = l + (r - l) / 2;
        mergeSort_recursive_indexed(arr, l, m, comparator);
        mergeSort_recursive_indexed(arr, m + 1, r, comparator);
        merge_inplace_helper(arr, l, m, r, comparator);
    }

    void mergeSort(std::vector<TankData>& data,
                   const std::function<bool(const TankData&, const TankData&)>& comparator) {
        if (data.empty()) return;
        mergeSort_recursive_indexed(data, 0, static_cast<int>(data.size() - 1), comparator);
    }

    std::vector<TankData> findByTankId(const std::vector<TankData>& all_data, const std::string& target_id) {
        std::vector<TankData> results;
        for (const auto& data_point : all_data) {
            if (data_point.tank_id == target_id) {
                results.push_back(data_point);
            }
        }
        return results;
    }

    std::vector<TankData> findByTimestampRange(const std::vector<TankData>& all_data,
                                               time_t start_time,
                                               time_t end_time) {
        std::vector<TankData> results;
        for (const auto& data_point : all_data) {
            if (data_point.timestamp >= start_time && data_point.timestamp <= end_time) {
                results.push_back(data_point);
            }
        }
        return results;
    }

    std::vector<TankData> findByStatus(const std::vector<TankData>& all_data, TankStatus target_status) {
        std::vector<TankData> results;
        for (const auto& data_point : all_data) {
            if (data_point.status == target_status) {
                results.push_back(data_point);
            }
        }
        return results;
    }
}