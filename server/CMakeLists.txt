# Ambil semua source file kecuali main.cpp
file(GLOB SERVER_SOURCES src/*.cpp)
list(REMOVE_ITEM SERVER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

set(MAIN_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

# Buat library dari semua implementasi selain main
add_library(server_lib ${SERVER_SOURCES})

target_include_directories(server_lib PUBLIC
    ${PROJECT_SOURCE_DIR}/server/include
    ${PROJECT_SOURCE_DIR}/shared/include
)

# Tambah library socket kalau di Windows
if(WIN32)
    target_link_libraries(server_lib PRIVATE ws2_32)
endif()

# Buat executable dari main.cpp
add_executable(main_executable ${MAIN_SOURCE})

target_link_libraries(main_executable PRIVATE server_lib)

target_include_directories(main_executable PRIVATE
    ${PROJECT_SOURCE_DIR}/server/include
    ${PROJECT_SOURCE_DIR}/shared/include
)
