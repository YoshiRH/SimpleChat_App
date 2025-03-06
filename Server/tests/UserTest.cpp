#include <gtest/gtest.h>
#include "../include/User.h"

// Simple test for basis function of User class

struct UserTest : public ::testing::Test {
	User testUser {"testuser", "pass123"};
};

TEST_F(UserTest, GetUsername) {
	ASSERT_EQ("testuser", testUser.getUsername());
};

TEST_F(UserTest, VeifyPassword) {
	ASSERT_TRUE(testUser.verifyPassword("pass123"));
	ASSERT_FALSE(testUser.verifyPassword("wrongpass"));
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}