cmake_minimum_required(VERSION 3.27)

project(Server VERSION 1.0.0 LANGUAGES C CXX)

set(SERVER_SOURCES
	src/main.cpp
	src/Server.cpp
	src/Log.cpp
)

set(SERVER_HEADERS
	include/Server.h
	include/Log.h
)

add_executable(Server ${SERVER_SOURCES})

target_include_directories(Server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Server ws2_32)
