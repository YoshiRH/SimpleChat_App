#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <string>

#define SERVER_IP "127.0.0.1"
#define PORT 55555

class Client
{
public:
	Client();
	~Client();
	bool Connect();
	void Run();

private:
	SOCKET clientSocket;

	std::string username;

	std::thread receiveThread;
	std::thread sendThread;
	std::mutex coutMutex;
	std::atomic<bool> running{ true };
	std::atomic<bool> joined{ false };

	void receiveMessage();
	void sendMessage();
	bool authenticate();
};

