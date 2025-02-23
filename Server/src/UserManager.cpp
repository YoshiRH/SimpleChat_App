#include "../include/UserManager.h"
#include "../include/Log.h"
#include <bcrypt.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

UserManager::UserManager()
{
	loadUsersFromFile();
}

bool UserManager::registerUser(const std::string& username, const std::string& password, SOCKET clientSocket)
{
	std::string passwordHash = hashPassword(password);

	if (passwordHash.empty()) {
		std::string response = "[SERVER] Failed to hash password";
		send(clientSocket, response.c_str(), sizeof(response), 0);
		return false;
	}

	std::lock_guard<std::mutex> lock(userMutex);
	if (users.find(username) != users.end()) {
		std::string response = "[SERVER] Username already taken";
		send(clientSocket, response.c_str(), sizeof(response), 0);
		return false;
	}

	users.emplace(username, User(username, passwordHash));
	saveUserToFile(username, passwordHash);
	std::string response = "[SERVER] Registration complete, please login";
	send(clientSocket, response.c_str(), sizeof(response), 0);
	return true;
}

bool UserManager::loginUser(const std::string& username, const std::string& password, SOCKET clientSocket)
{
	std::string passwordHash = hashPassword(password);

	if (passwordHash.empty()) {
		std::string response = "[SERVER] Failed to hash password";
		send(clientSocket, response.c_str(), sizeof(response), 0);
		return false;
	}

	std::lock_guard<std::mutex> lock(userMutex);
	auto it = users.find(username);

	if (it == users.end() || !it->second.verifyPassword(passwordHash)) {
		std::string response = "[SERVER] Invalid username or password, try again";
		send(clientSocket, response.c_str(), sizeof(response), 0);
		return false;
	}

	std::string response = "[SERVER] Login successful";
	send(clientSocket, response.c_str(), sizeof(response), 0);

	std::lock_guard<std::mutex> lockActiveUsers(activeUsersMutex);
	activeUsers[clientSocket] = username;
	return true;
}

void UserManager::loadUsersFromFile()
{
	fs::path filePath = fs::current_path() / "users.txt";

	if (!fs::exists(filePath)) {
		Log::getInstance().printLog("[SERVER] Couldn't find user.txt file, no data loaded");
		return;
	}

	std::ifstream file(filePath);
	if (!file) {
		Log::getInstance().printLog("[SERVER] Failed to open user.txt file");
		return;
	}
	
	std::string line;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string username{}, password{};

		if (std::getline(iss, username, ':') && std::getline(iss, password)) {
			std::lock_guard<std::mutex> lock(userMutex);
			users.emplace(username, User(username, password));
		}
	}

	file.close();
	Log::getInstance().printLog("[SERVER] Loaded users from file"); // Change it later to sql maybe?
}

std::string UserManager::getUsername(SOCKET clientSocket)
{
	std::lock_guard<std::mutex> lock(activeUsersMutex);

	auto it = activeUsers.find(clientSocket);
	return(it != activeUsers.end() ? it->second : "");
}


std::string UserManager::hashPassword(const std::string& password)
{
	BCRYPT_ALG_HANDLE hAlg = nullptr;
	BCRYPT_HASH_HANDLE hHash = nullptr;
	NTSTATUS status;
	std::string hash;

	status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr, 0);
	if (!BCRYPT_SUCCESS(status)) {
		Log::getInstance().printLog("[SERVER] Failed to open hash algorithm");
		return "";
	}

	status = BCryptCreateHash(hAlg, &hHash, nullptr, 0, nullptr, 0, 0);
	if (!BCRYPT_SUCCESS(status)) {
		BCryptCloseAlgorithmProvider(hAlg, 0);
		Log::getInstance().printLog("[SERVER] Failed to create hash");
		return "";
	}

	status = BCryptHashData(hHash, (PUCHAR)password.c_str(), password.length(), 0);
	if (!BCRYPT_SUCCESS(status)) {
		BCryptDestroyHash(hHash);
		BCryptCloseAlgorithmProvider(hAlg, 0);
		Log::getInstance().printLog("[SERVER] Failed to hash password");
		return "";
	}

	DWORD hashLength = 32;
	std::vector<UCHAR> hashBytes(hashLength);

	status = BCryptFinishHash(hHash, hashBytes.data(), hashLength, 0);
	if (!BCRYPT_SUCCESS(status)) {
		BCryptDestroyHash(hHash);
		BCryptCloseAlgorithmProvider(hAlg, 0);
		Log::getInstance().printLog("[SERVER] Failed to finish hash");
		return "";
	}

	std::ostringstream oss;
	for (auto byte : hashBytes) {
		oss << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
	}
	hash = oss.str();

	BCryptDestroyHash(hHash);
	BCryptCloseAlgorithmProvider(hAlg, 0);
	return hash;
}

void UserManager::saveUserToFile(const std::string& username, const std::string& password)
{
	std::ofstream file("users.txt", std::ios::app);
	if (!file.is_open()) {
		Log::getInstance().printLog("[SERVER] Couldn't open user.txt file");
		return;
	}

	file << username << ":" << password << '\n';
	file.close();
}
