cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(SimpleChat_App VERSION 1.0.0 LANGUAGES C CXX)

add_subdirectory(Client)
add_subdirectory(Server)