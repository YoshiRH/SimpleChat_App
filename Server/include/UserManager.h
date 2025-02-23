#pragma once
#include <winsock2.h>
#include <map>
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
	std::map<std::string, User> users;
	std::mutex userMutex;
	std::map<SOCKET, std::string> activeUsers;
	std::mutex activeUsersMutex;

	void saveUserToFile(const std::string& username, const std::string& password);
	std::string hashPassword(const std::string& password);
};

