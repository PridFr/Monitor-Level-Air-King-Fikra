cmake_minimum_required(VERSION 3.16)
project(MonitorLevelAir)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------- Server Library -----------
add_library(server_lib
    server/src/Server.cpp
    "Penyimpanan  & Ekspor Data/src/data_manager.cpp"
    # Tambahkan file sumber lain jika ada
)

target_include_directories(server_lib PUBLIC
    server/include
    shared/include
    "${CMAKE_CURRENT_SOURCE_DIR}/Penyimpanan  & Ekspor Data/include"
    lib/nlohmann
)

if (WIN32)
    target_link_libraries(server_lib ws2_32)
endif()

# ----------- Server Executable -----------
add_executable(server
    server/src/main.cpp
)
target_link_libraries(server server_lib)

# ----------- Client Executable -----------
add_executable(client
    client/src/client.cpp
)
target_include_directories(client PRIVATE
    shared/include
    lib/nlohmann
)
if (WIN32)
    target_link_libraries(client ws2_32)
endif()
