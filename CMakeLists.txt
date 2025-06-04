cmake_minimum_required(VERSION 3.16)
project(MonitorLevelAir)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include shared headers
include_directories(shared/include)

# -------- Server --------
add_library(server_lib
    server/src/Server.cpp
)

target_include_directories(server_lib PUBLIC
    server/include
    shared/include
)

# Link Winsock for Windows
if (WIN32)
    target_link_libraries(server_lib ws2_32)
endif()

add_executable(server
    server/src/main.cpp   # buat main server (kalau ada)
)

target_link_libraries(server
    server_lib
)

# -------- Client --------
add_executable(client
    client/src/client.cpp
)

target_include_directories(client PRIVATE
    shared/include
)

if (WIN32)
    target_link_libraries(client ws2_32)
endif()
