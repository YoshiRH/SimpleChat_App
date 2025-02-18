#include "Server.h"
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
		throw std::runtime_error("Couldn't create a server socket: " + std::to_string(WSAGetLastError()));
	}

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

	if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		closesocket(serverSocket);
		throw std::runtime_error("Couldn't bind to server socket");
	}

	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
		closesocket(serverSocket);
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

		std::thread(&Server::handleClient, this, clientSocket).detach();
		std::cout << "[SERVER] New client connected\n";
	}
}

void Server::handleClient(SOCKET clientSocket)
{
	char buffer[200];
	int bytesReceived{};

	while (true) {
		bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

		if (bytesReceived <= 0) {
			std::cout << "[SERER] Client disconnected\n";
			break;
		}

		buffer[bytesReceived] = '\0';
		std::string message(buffer);

		if (message == "exit") {
			std::cout << "[SERVER] Client requested to disconnect\n";
			break;
		}

		broadcast(message, clientSocket);
	}

	closesocket(clientSocket);
	{
		std::lock_guard<std::mutex> lock(clientsMutex);
		clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
	}
}

void Server::broadcast(const std::string& msg, SOCKET excludedSocket)
{
	std::lock_guard<std::mutex> lock(clientsMutex);

	for (SOCKET client : clients) {
		if (client != excludedSocket) {
			send(client, msg.c_str(), msg.size() + 1, 0);
		}
	}
}
