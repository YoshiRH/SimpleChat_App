#include "../include/Server.h"
#include "iostream"


Server::Server() : serverSocket(INVALID_SOCKET) {
	WSAData wsaData;
	WORD version = MAKEWORD(2, 2);

	if (WSAStartup(version, &wsaData) != 0) {
		throw std::runtime_error("Couldn't initialize Winsock: " + std::to_string(WSAGetLastError()));
	}
}

Server::~Server()
{
	closesocket(serverSocket);
	WSACleanup();
}

void Server::startServer()
{
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET) {
		closesocket(serverSocket);
		WSACleanup();
		throw std::runtime_error("Couldn't create a server socket: " + std::to_string(WSAGetLastError()));
	}

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

	if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		closesocket(serverSocket);
		WSACleanup();
		throw std::runtime_error("Couldn't bind to server socket");
	}

	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
		closesocket(serverSocket);
		WSACleanup();
		throw std::runtime_error("Error while listening for connections...");
	}

	std::cout << "[SERVER] Listening for connections on: " << SERVER_IP << " : " << PORT << std::endl;

	while (true) {
		SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);

		if (clientSocket == INVALID_SOCKET) {
			std::cerr << "Accept error: " << WSAGetLastError() << '\n';
			continue;
		}

		{
			std::lock_guard<std::mutex> lock(clientsMutex);
			clients.emplace_back(clientSocket);
		}

		std::thread handleClientThread(&Server::handleClient, this, clientSocket);
		handleClientThread.detach();
	}

	closesocket(serverSocket);
}

void Server::handleClient(SOCKET clientSocket)
{
	std::cout << "[SERVER] New client connected\n";
	char buffer[1024];
	int bytesReceived {};

	while (true) {
		bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

		if (bytesReceived <= 0) {
			std::cout << "[SERER] Client disconnected\n";
			break;
		}

		std::string message(buffer, bytesReceived);

		if (message == "exit") {
			std::cout << "[SERVER] Client requested to disconnect\n";
			break;
		}

		std::cout << message << std::endl;
		broadcast(message, clientSocket);
	}

	closesocket(clientSocket);
	{
		std::lock_guard<std::mutex> lock(clientsMutex);
		auto it = std::find(clients.begin(), clients.end(), clientSocket);
		clients.erase(it);
	}
}

void Server::broadcast(const std::string& msg, SOCKET excludedSocket)
{
	std::lock_guard<std::mutex> lock(clientsMutex);

	for (auto it = clients.begin(); it != clients.end();) {
		SOCKET client = *it;
		if (client == excludedSocket) {
			++it;
			continue;
		}

		int result = send(client, msg.c_str(), msg.size(), 0);

		if (result == SOCKET_ERROR) {
			std::cerr << "[SERVER] Couldn't send the message to: " << client << ". Deleting client...\n";
			closesocket(client);
			it = clients.erase(it);
		}
		else {
			++it;
		}
	}
}
