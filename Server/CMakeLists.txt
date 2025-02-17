cmake_minimum_required(VERSION 3.27)

project(ServerTest VERSION 1.0.0 LANGUAGES C CXX)

set(MAIN_FILE src/main.cpp)

file(GLOB_RECURSE SOURCES src/**/*.cpp)
file(GLOB_RECURSE HEADERS src/**/*.h)

add_executable(ServerTest ${MAIN_FILE} ${SOURCES} ${HEADERS})