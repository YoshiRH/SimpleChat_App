#include <gtest/gtest.h>
#include "../include/UserManager.h"
#include <WinSock2.h>
#include <fstream>

struct UserManagerTest : public ::testing::Test {
	UserManager userManager;
	SOCKET dummySocket = INVALID_SOCKET;

	void SetUp() override {
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		dummySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		std::remove("user.txt");
	}

	void TearDown() override {
		if (dummySocket != INVALID_SOCKET) {
			closesocket(dummySocket);
		}
		WSACleanup();
		std::remove("users.txt");
	}
};

TEST_F(UserManagerTest, RegisterUserSuccess) {
	bool result = userManager.registerUser("testuser", "pass123", dummySocket);
	ASSERT_TRUE(result);
};

TEST_F(UserManagerTest, RegisterUserDuplicate) {
	userManager.registerUser("testuser", "pass123", dummySocket);
	bool result = userManager.registerUser("testuser", "otherpass", dummySocket);
	ASSERT_FALSE(result);
};

TEST_F(UserManagerTest, LoginUserSuccess) {
	userManager.registerUser("testuser", "pass123", dummySocket);
	bool result = userManager.loginUser("testuser", "pass123", dummySocket);
	ASSERT_TRUE(result);
	ASSERT_EQ("testuser", userManager.getUsername(dummySocket));
};

TEST_F(UserManagerTest, LoginUserWrongPassword) {
	userManager.registerUser("testuser", "pass123", dummySocket);
	bool result = userManager.loginUser("testuser", "wrongpass", dummySocket);
	ASSERT_FALSE(result);
};

TEST_F(UserManagerTest, HashPasswordSameWay) {
	std::string pass1 = userManager.hashPassword("password");
	std::string pass2 = userManager.hashPassword("password");
	ASSERT_EQ(pass1, pass2);
};

TEST_F(UserManagerTest, SaveAndLoadUserFromFile) {
	std::string username = "testuser";
	std::string pass = "pass123";
	std::string passHash = userManager.hashPassword(pass);

	userManager.saveUserToFile(username, passHash);
	userManager.loadUsersFromFile();

	bool result = userManager.loginUser(username, pass, dummySocket);
	ASSERT_TRUE(result);
};

TEST_F(UserManagerTest, RegisterWithEmptyUsername) {
	bool result = userManager.registerUser("", "pass123", dummySocket);
	ASSERT_FALSE(result);
};

TEST_F(UserManagerTest, RegisterWithEmptyPassword) {
	bool result = userManager.registerUser("testuser", "", dummySocket);
	ASSERT_FALSE(result);
};

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}