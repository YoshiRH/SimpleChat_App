#pragma once
#include <winsock2.h>
#include <unordered_map>
#include <mutex>
#include <string>
#include "User.h"

class UserManager
{
public:

	UserManager();
	bool registerUser(const std::string& username, const std::string& password, SOCKET clientSocket);
	bool loginUser(const std::string& username, const std::string& password, SOCKET clientSocket);
	void loadUsersFromFile();
	std::string getUsername(SOCKET clientSocket);

private:
	std::unordered_map<std::string, User> users; // All registered users
	std::mutex userMutex;
	std::unordered_map<SOCKET, std::string> activeUsers; // All online users
	std::mutex activeUsersMutex;

	void saveUserToFile(const std::string& username, const std::string& password);
	std::string hashPassword(const std::string& password);
};

