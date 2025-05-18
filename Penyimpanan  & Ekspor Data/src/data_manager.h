#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H
#include <string>
#include <fstream>
#include <ctime> / untuk time_t
#include <nlohmann/json.hpp> //pemanggilan JSOn

using namespace std;

class DataManager{
public:
//constructor
    DataManager(const string binPath,
                const string jsonPath,
                double overflowThreshold,
                double droughtThreshold);
    ~DataManager();

    //mengatur threshold overlfow
    void setOverflowThreshold (double threshold);
//mengatur threshold drought
void se5DroughtThreshold (double threshold);

//memroses data baru dan mencatat
void processData(time_t time, double level);

private:
string binFile;
string jsonFile;
double overflowThreshold;
double droughtThreshold;
ofstream binStream;

//simpan data ke file bin
void saceToBInary (time_t time, double level);
//mencatat kondisi kritis ke JSON
void reportCriticalCondition(time_T time, double lev3el, const string tipe, double threshold);

//ubah time_t ke string
string timeToString(time_t time) const;
};
#endif //DATA_MANAGER_H
