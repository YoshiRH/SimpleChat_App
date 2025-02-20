#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include <thread>
#include <mutex>


#define SERVER_IP "127.0.0.1"
#define PORT 55555
#define MAX_CLIENTS 20

class Server
{
public:
	Server();
	~Server();
	void startServer();

private:
	SOCKET serverSocket;

	std::vector<SOCKET> clients;
	std::vector<std::thread> clientThreads;
	std::mutex clientsMutex;

	void handleClient(SOCKET clientSocket);
	void broadcast(const std::string& msg, SOCKET excludedSocket = INVALID_SOCKET);
};

