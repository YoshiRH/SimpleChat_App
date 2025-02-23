#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include <thread>
#include <mutex>
#include <deque>
#include "UserManager.h"

#define SERVER_IP "127.0.0.1"
#define PORT 55555
#define MAX_CLIENTS 20
#define MAX_HISTORY_SIZE 25

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

	void handleClient(SOCKET clientSocket);
	void broadcast(const std::string& msg, SOCKET excludedSocket = INVALID_SOCKET);

	void sendHistory(SOCKET& clientSocket);
	void addMsgToHistory(const std::string& message);

	void deleteClient(SOCKET clientSocket);
};

