#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include <thread>
#include <mutex>
#include <deque>
#include <atomic>
#include "UserManager.h"

#define SERVER_IP "127.0.0.1"
#define PORT 55555
#define MAX_CLIENTS 20
#define MAX_HISTORY_SIZE 25

// Declare class for gTest purpose
class ServerTest;

class Server
{
public:
	Server();
	~Server();
	void startServer();

private:
	SOCKET serverSocket;
	UserManager userManager;

	std::vector<SOCKET> clients;
	std::vector<std::thread> clientThreads;
	std::mutex clientsMutex;

	std::deque<std::string> chatHistory;
	std::mutex historyMutex;

	// Manage communication with user
	void handleClient(SOCKET clientSocket);
	void broadcast(const std::string& msg, SOCKET excludedSocket = INVALID_SOCKET);

	// Send last 25 messages new logged client
	void sendHistory(SOCKET& clientSocket);
	void addMsgToHistory(const std::string& message);

	void deleteClient(SOCKET clientSocket);
	friend class ServerTest;
};

