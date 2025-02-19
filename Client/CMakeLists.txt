cmake_minimum_required(VERSION 3.27)

project(Client VERSION 1.0.0 LANGUAGES C CXX)

set(CLIENT_SOURCES
	src/main.cpp
	src/Client.cpp
)

set(CLIENT_HEADERS
	include/Client.h
)

add_executable(Client ${CLIENT_SOURCES})

target_include_directories(Client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)