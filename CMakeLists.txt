cmake_minimum_required(VERSION 3.16)
project(http_server VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(http_server
    src/main.cpp
    src/socket.cpp
    src/server.cpp
    src/response.cpp
    src/request.cpp
    src/thread_pool.cpp
)

target_include_directories(http_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(WIN32)
    target_link_libraries(http_server PRIVATE ws2_32)
endif()
