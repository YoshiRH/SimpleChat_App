cmake_minimum_required(VERSION 3.27)

project(Server VERSION 1.0.0 LANGUAGES C CXX)

set(SERVER_SOURCES
	src/main.cpp
	src/Server.cpp
	src/Log.cpp
	src/User.cpp
	src/UserManager.cpp
)

set(SERVER_HEADERS
	include/Server.h
	include/Log.h
	include/User.h
	include/UserManager.h
)

add_executable(Server ${SERVER_SOURCES})

target_include_directories(Server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Server ws2_32 bcrypt)

add_subdirectory(external/googletest EXCLUDE_FROM_ALL)

enable_testing()
add_subdirectory(tests)

set_target_properties(Server PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
