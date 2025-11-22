cmake_minimum_required(VERSION 2.8)
project(final)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -O2")

# Исходные файлы
set(SOURCES
    src/main.cpp
    src/server.cpp
    src/http_parser.cpp
    src/thread_pool.cpp
)

# Исполняемый файл
add_executable(final ${SOURCES})
