cmake_minimum_required(VERSION 3.31)
project(Monitor_Level_Air_King_Fikra)

set(CMAKE_CXX_STANDARD 20)

add_executable(Monitor_Level_Air_King_Fikra "Penyimpanan  & Ekspor Data/src/main.cpp"
        "Penyimpanan  & Ekspor Data/include/data_manager.h"
        "Penyimpanan  & Ekspor Data/src/data_manager.cpp"
        shared/include/common_types.h)
