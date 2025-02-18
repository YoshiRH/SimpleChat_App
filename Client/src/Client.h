#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <mutex>

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
	std::thread receiveThread;
	std::mutex coutMutex;

	void receiveMessage();
};

