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
	if (!authenticate()) {
		std::cout << "Authentication failed, exiting...\n";
		return;
	}

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
			std::lock_guard<std::mutex> lock(coutMutex);
			std::cout << "\n[SERVER] Disconnected from the server\n";
			running = false;
			break;
		} 
		else {
			std::string msg(buffer, bytesReceived);

			std::lock_guard<std::mutex> lock(coutMutex);
			if (msg.find("~") == 0) 
				SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
			else 
				SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);

			std::cout << msg << '\n';
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

		}
	}
}

void Client::sendMessage()
{
	std::cout << "Welcome to chat room, type messages as you want (type 'exit' to quit)\n\n";

	joined = true;

	std::string message;
	while (running) {
		std::getline(std::cin, message);
		if (message.empty()) continue;

		if (message == "exit") {
			std::cout << "Disconnecting...\n";
			send(clientSocket, "exit", 4, 0);
			running = false;
			break;
		}
		int bytesSend = send(clientSocket, message.c_str(), message.length(), 0);
		
		if (bytesSend == SOCKET_ERROR) {
			std::cerr << "[SERVER] Couldn't send the message, try again...\n";
			running = false;
			continue;
		}

	}
}

bool Client::authenticate()
{
	std::string choice{}, username{}, password{};

	while (running) {
		std::cout << "1) Login\n2)Register\n> ";
		std::getline(std::cin, choice);

		if (choice != "1" && choice != "2") {
			std::cout << "Invalid choice, try again\n";
			continue;
		}

		std::cout << "\nEnter username: ";
		std::getline(std::cin, username);


		std::cout << "\nEnter password: ";
		std::getline(std::cin, password);

		while (!username.empty() && std::isspace(username.back())) username.pop_back();
		while (!password.empty() && std::isspace(password.back())) password.pop_back();


		std::string command = (choice == "1") ? "LOGIN" : "REGISTER";
		std::string message = command + " " + username + " " + password;

#ifdef _DEBUG
		std::cout << "[DEBUG] Sending: " << message << std::endl;
#endif

		if (send(clientSocket, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
			std::cerr << "Failed to send auth request: " << WSAGetLastError() << '\n';
			return false;
		}

		char buffer[1024];
		ZeroMemory(buffer, sizeof(buffer));
		int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (bytesReceived <= 0) {
			std::cerr << "Server disconnected during authentication\n";
			return false;
		}

		std::string response(buffer, bytesReceived);
		std::cout << response << std::endl;

		if (response.find("Login successful") != std::string::npos) {
			this->username = username;
			return true;
		}
	}

	return false;
}



