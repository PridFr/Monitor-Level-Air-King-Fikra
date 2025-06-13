cmake_minimum_required(VERSION 3.10)
project(ServerApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Source files
file(GLOB SERVER_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../shared/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Penyimpanan  & Ekspor Data/src/*.cpp"
)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../shared/include
    "${CMAKE_CURRENT_SOURCE_DIR}/../Penyimpanan  & Ekspor Data/include"
)

# Add executable
add_executable(main_executable ${SERVER_SRC})

# Link with ws2_32 for Windows sockets
if (WIN32)
    target_link_libraries(main_executable ws2_32)
endif()