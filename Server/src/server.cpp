#include "../include/Server.h"
#include "../include/Log.h"
#include <iostream>
#include <windows.h>
#include <sstream>

static volatile bool running = true;

BOOL WINAPI ConsoleHandler(DWORD signal) {
	if (signal == CTRL_C_EVENT) running = false;
	return TRUE;
}

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
	SetConsoleCtrlHandler(ConsoleHandler, TRUE); // Ctrl+C
	Log::getInstance().printLog("[SERVER] Starting server...");

	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET) {
		Log::getInstance().printLog("Couldn't create a server socket: " + std::to_string(WSAGetLastError()));
		closesocket(serverSocket);
		WSACleanup();
		throw std::runtime_error("Couldn't create a server socket: " + std::to_string(WSAGetLastError()));
	}

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

	if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		Log::getInstance().printLog("Couldn't bind to server socket: " + std::to_string(WSAGetLastError()));
		closesocket(serverSocket);
		WSACleanup();
		throw std::runtime_error("Couldn't bind to server socket");
	}

	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
		Log::getInstance().printLog("Error while listening for connections...");
		closesocket(serverSocket);
		WSACleanup();
		throw std::runtime_error("Error while listening for connections...");
	}

	Log::getInstance().printLog("[SERVER] Listening for connections on: " + std::string(SERVER_IP) + " : " + std::to_string(PORT));
	std::cout << "[SERVER] Listening for connections on: " << SERVER_IP << " : " << PORT << std::endl;

	while (running) {
		if (clients.size() >= MAX_CLIENTS) {
			Log::getInstance().printLog("[SERVER] Max clients reached, rejecting new connection");
			Sleep(1000);
			continue;
		}

		SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);

		if (clientSocket == INVALID_SOCKET) {
			std::cerr << "Accept error: " << WSAGetLastError() << '\n';
			if (WSAGetLastError() == WSAEINTR) break;
			continue;
		}

		{
			std::lock_guard<std::mutex> lock(clientsMutex);
			clients.emplace_back(clientSocket);
			clientThreads.emplace_back(&Server::handleClient, this, clientSocket);
		}
	}

	std::lock_guard<std::mutex> lock(clientsMutex);

	for (auto& client : clients) {
		closesocket(client);
	}

	for (auto& thread : clientThreads) {
		if (thread.joinable()) thread.join();
	}

	closesocket(serverSocket);
	serverSocket = INVALID_SOCKET;
}

void Server::handleClient(SOCKET clientSocket)
{
	Log::getInstance().printLog("[SERVER] New client connected");
	std::cout << "[SERVER] New client connected\n";

	char buffer[1024];
	int bytesReceived {};

	while (running) {
		ZeroMemory(buffer, sizeof(buffer));
		bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

		if (bytesReceived <= 0) {
			deleteClient(clientSocket);
			return;
		}

		std::string message(buffer, bytesReceived);

		while (!message.empty() && std::isspace(message.back())) {
			message.pop_back();
		}

		std::istringstream iss(message);
		std::string command{}, arg1{}, arg2{};
		iss >> command >> arg1 >> arg2;

#ifdef _DEBUG
		std::cout << "[DEBUG] Received: command=" << command << ", arg1=" << arg1 << ", arg2=" << arg2 << std::endl;
#endif

		if (command == "REGISTER") {
			std::cout << "Registered\n";
			userManager.registerUser(arg1, arg2, clientSocket);
		}
		else if (command == "LOGIN") {
			std::cout << "Login\n";
			if(userManager.loginUser(arg1, arg2, clientSocket))
				break;
		}
	}

	std::string username = userManager.getUsername(clientSocket);
	std::string joinMsg = "[" + username + "] has joined";
	std::cout << joinMsg << std::endl;
	broadcast(joinMsg, clientSocket);
	sendHistory(clientSocket);

	while (running) {
		ZeroMemory(buffer, sizeof(buffer));
		bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

		if (bytesReceived <= 0) {
			Log::getInstance().printLog("[SERVER] Client disconnected");
			std::cout << "[SERVER] Client disconnected\n";
			broadcast("[" + username + "] disconnected", clientSocket);
			break;
		}

		std::string message(buffer, bytesReceived);
		if (message == "exit") {
			Log::getInstance().printLog("[SERVER] Client requested to disconnect");
			std::cout << "[SERVER] Client requested to disconnect\n";
			broadcast("[" + username + "] disconnected", clientSocket);
			break;
		}

		std::string formatedMsg = "[" + username + "]: " + message;
		std::cout << formatedMsg << std::endl;
		broadcast(formatedMsg, clientSocket);
	}

	deleteClient(clientSocket);
}

void Server::broadcast(const std::string& msg, SOCKET excludedSocket)
{
	std::lock_guard<std::mutex> lock(clientsMutex);
	
	addMsgToHistory(msg);
	for (auto it = clients.begin(); it != clients.end();) {
		SOCKET client = *it;
		if (client == excludedSocket) {
			++it;
			continue;
		}

		std::string username = userManager.getUsername(client);

		if (!username.empty()) {
			int result = send(client, msg.c_str(), msg.size(), 0);

			if (result == SOCKET_ERROR) {
				Log::getInstance().printLog("[SERVER] Couldn't send the message to: " + std::to_string(client));
				std::cerr << "[SERVER] Couldn't send the message to: " << client << ". Deleting client...\n";
				closesocket(client);
				it = clients.erase(it);
			}
			else {
				++it;
			}
		}
		else {
			++it;
		}
	}
}

void Server::sendHistory(SOCKET& clientSocket)
{
	std::lock_guard<std::mutex> lock(historyMutex);

	for (const auto& msg : chatHistory) {
		send(clientSocket, msg.c_str(), msg.size(), 0);
	}
}

void Server::addMsgToHistory(const std::string& message)
{
	std::lock_guard<std::mutex> lock(historyMutex);
	
	std::string historyMsg = "~" + message + '\n';
	chatHistory.push_back(historyMsg);
	if (chatHistory.size() > MAX_HISTORY_SIZE) {
		chatHistory.pop_front();
	}
}

void Server::deleteClient(SOCKET clientSocket)
{
	closesocket(clientSocket);
	{
		std::lock_guard<std::mutex> lock(clientsMutex);
		for (auto it = clients.begin(); it != clients.end(); ++it) {
			if (*it == clientSocket) {
				clients.erase(it);
				break;
			}
		}
	}
}
