#include "../include/Client.h"
#include <iostream>


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
		std::cerr << "Couldn't connect to server: " <<  WSAGetLastError();
		closesocket(clientSocket);
		WSACleanup();
		return false;
	}

	std::cout << "Connected to server!\n";

	return true;
}

void Client::Run()
{
	sendThread = std::thread(&Client::sendMessage, this);
	sendThread.detach();

	receiveThread = std::thread(&Client::receiveMessage, this);
	receiveThread.join();

	closesocket(clientSocket);
	WSACleanup();
}

void Client::receiveMessage()
{
	char buffer[1024];
	int bytesReceived;
	std::string msg;

	while (true) {
		bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

		if (bytesReceived <= 0) {
			std::cout << "\n[SERVER] Disconnected from the server\n";
			break;
		} 
		else {
			msg = std::string(buffer, bytesReceived);

			std::lock_guard<std::mutex> lock(coutMutex);
			std::cout << "\033[32m" << msg << "\033[0m\n";
		}
	}

	closesocket(clientSocket);
	WSACleanup();
	exit(0);
}

void Client::sendMessage()
{
	std::cout << "Welcome to chat room, type messages as you want (type 'exit' to quit)\n";
	std::cout << "Enter your chat name: ";
	std::string nickname;
	std::cout << "> ";
	std::getline(std::cin, nickname);

	std::string message;
	
	while (true) {
		std::getline(std::cin, message);
		std::string msg = "[" + nickname + "]: " + message;

		if (message.empty()) continue;

		if (message == "exit") {
			std::cout << "Disconnecting...\n";
			break;
		}

		int bytesSend = send(clientSocket, msg.c_str(), msg.length(), 0);
		
		if (bytesSend == SOCKET_ERROR) {
			std::cerr << "[SERVER] Couldn't send the message, try again...\n";
			continue;
		}

	}

	closesocket(clientSocket);
	WSACleanup();
}



