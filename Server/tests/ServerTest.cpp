#include <gtest/gtest.h>
#include <deque>
#include <string>
#include "../include/Server.h"

struct ServerTest : public ::testing::Test {
	Server server;
	std::deque<std::string>& chatHistory = server.chatHistory;
	std::mutex& historyMutex = server.historyMutex;

	void addMsgToHistory(const std::string& msg) {
		std::lock_guard lock(historyMutex);
		chatHistory.push_back("~" + msg + '\n');

		if (chatHistory.size() > 25) {
			chatHistory.pop_front();
		}
	}
};

TEST_F(ServerTest, addMsgToHistoryInLimit) {
	addMsgToHistory("test message");

	ASSERT_EQ(1, chatHistory.size());
}

TEST_F(ServerTest, addMsgToHistoryBeyondLimit) {
	for (int i = 0; i < 30; i++) {
		addMsgToHistory("Test msg - " + std::to_string(i));
	}

	ASSERT_EQ(25, chatHistory.size());
}


int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}