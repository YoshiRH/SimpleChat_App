cmake_minimum_required(VERSION 3.27)
project(ServerTests)

add_executable(UserManagerTest
	UserManagerTest.cpp
	../src/UserManager.cpp
	../src/User.cpp
	../src/Log.cpp
)

target_include_directories(UserManagerTest PRIVATE
	../include
	${CMAKE_CURRENT_SOURCE_DIR}/../external/googletest/googletest/include
	${CMAKE_CURRENT_SOURCE_DIR}/../external/googletest/googlemock/include
)

target_link_libraries(UserManagerTest PRIVATE
	gtest
	gtest_main
	bcrypt
	ws2_32
)

add_test(NAME UserManagerTest COMMAND UserManagerTest)




add_executable(UserTest
	UserTest.cpp
	../src/User.cpp
)

target_include_directories(UserTest PRIVATE
	../include
	${CMAKE_CURRENT_SOURCE_DIR}/../external/googletest/googletest/include
	${CMAKE_CURRENT_SOURCE_DIR}/../external/googletest/googlemock/include
)

target_link_libraries(UserTest PRIVATE
	gtest
	gtest_main
)

add_test(NAME UserTest COMMAND UserTest)