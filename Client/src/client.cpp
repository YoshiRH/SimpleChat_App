#include "Client.h"
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
		return false;
	}

	std::cout << "Connected to server!\n";

	return true;
}

void Client::Run()
{
	receiveThread = std::thread(&Client::receiveMessage, this);
	receiveThread.detach();

	char buffer[200];
	std::cout << "Welcome to char room, type messages as you want (type 'exit' to quit)\n";

	while (true) {
		std::cout << "> ";
		std::cin.getline(buffer, sizeof(buffer));

		if (send(clientSocket, buffer, sizeof(buffer), 0) == SOCKET_ERROR) {
			std::cerr << "[SERVER] Couldn't send the message, try again...\n";
			continue;
		}

		if (buffer == "exit") break;
	}

	closesocket(clientSocket);
}

void Client::receiveMessage()
{
	char buffer[200];
	int bytesReceived;

	while (true) {
		bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

		if (bytesReceived <= 0) {
			std::cout << "\n[SERVER] Disconnected from server\n";
			closesocket(clientSocket);
			exit(0);
		}

		buffer[bytesReceived] = '\0';
		std::cout << '\n' << buffer << '\n';
	}
}



