cmake_minimum_required(VERSION 3.27)

project(Server VERSION 1.0.0 LANGUAGES C CXX)

set(SERVER_SOURCES
	src/main.cpp
	src/Server.cpp
)

set(SERVER_HEADERS
	include/Server.h
)

add_executable(Server ${SERVER_SOURCES} ${SERVER_HEADERS})

target_include_directories(Server PRIVATE include);