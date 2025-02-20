#include "../include/Client.h"
#include <iostream>
#include <windows.h>

Client::Client() : clientSocket(INVALID_SOCKET) {
	WSADATA wsaData;
	WORD version = MAKEWORD(2, 2);

	if (WSAStartup(version, &wsaData) != 0) {
		throw std::runtime_error("Couldn't initialize Winsock: " + std::to_string(WSAGetLastError()));
	}
}

Client::~Client()
{
	closesocket(clientSocket);
	WSACleanup();
}

bool Client::Connect()
{
	for (int attempts = 0; attempts < 5; ++attempts) {
		clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (clientSocket == INVALID_SOCKET) {
			std::cerr << "Couldn't create clientSocket: " << WSAGetLastError();
			return false;
		}

		sockaddr_in serverAddr;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(PORT);
		inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

		if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
			std::cerr << "Attempt " << attempts+1 << ": Couldn't connect to server: " << WSAGetLastError();
			closesocket(clientSocket);
			Sleep(2000);
			continue;
		}

		std::cout << "Connected to server!\n";

		return true;
	}

	std::cerr << "Failed to connect after 5 attempts.\n";
	WSACleanup();
	return false;
}

void Client::Run()
{
	sendThread = std::thread(&Client::sendMessage, this);

	receiveThread = std::thread(&Client::receiveMessage, this);
	receiveThread.join();

	running = false;

	sendThread.join();

	closesocket(clientSocket);
	WSACleanup();
}

void Client::receiveMessage()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	char buffer[1024];
	int bytesReceived;

	while (running) {
		ZeroMemory(buffer, sizeof(buffer));
		bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

		if (bytesReceived <= 0) {
			std::cout << "\n[SERVER] Disconnected from the server\n";
			break;
		} 
		else {
			std::string msg(buffer, bytesReceived);

			std::lock_guard<std::mutex> lock(coutMutex);
			SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
			std::cout << msg << '\n';
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

		}
	}
}

void Client::sendMessage()
{
	std::cout << "Welcome to chat room, type messages as you want (type 'exit' to quit)\n";
	std::cout << "Enter your chat name: ";
	std::string nickname;
	std::cout << "> ";
	std::getline(std::cin, nickname);

	std::string joinMsg = "[" + nickname + "] has joined";
	send(clientSocket, joinMsg.c_str(), joinMsg.length(), 0);

	std::string message;
	while (running) {
		std::getline(std::cin, message);
		if (message.empty()) continue;

		std::string msg = "[" + nickname + "]: " + message;


		if (message == "exit") {
			std::cout << "Disconnecting...\n";
			send(clientSocket, "exit", 4, 0);
			break;
		}

		int bytesSend = send(clientSocket, msg.c_str(), msg.length(), 0);
		
		if (bytesSend == SOCKET_ERROR) {
			std::cerr << "[SERVER] Couldn't send the message, try again...\n";
			continue;
		}

	}
}



