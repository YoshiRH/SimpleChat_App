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
	if (clientSocket != INVALID_SOCKET) {
		closesocket(clientSocket);
	}

	WSACleanup();
}

// Connect to server (5 attempts)
bool Client::Connect()
{
	for (int attempts = 0; attempts < 5; ++attempts) {
		clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (clientSocket == INVALID_SOCKET) {
			std::cerr << "Couldn't create clientSocket: " << WSAGetLastError();
			return false;
		}

		// Configure server addres (to change it go to .h file and change SERVER_IP and PORT)
		sockaddr_in serverAddr;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(PORT);
		inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

		if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
			std::cerr << "Attempt " << attempts+1 << ": Couldn't connect to server: " << WSAGetLastError();
			closesocket(clientSocket);
			clientSocket = INVALID_SOCKET;
			Sleep(2000);
			continue;
		}

		std::cout << "Connected to server!\n";

		return true;
	}

	std::cerr << "Failed to connect after 5 attempts.\n";
	return false;
}

void Client::Run()
{
	if (!authenticate()) {
		std::cout << "Authentication failed, exiting...\n";
		return;
	}

	// Thread for sending messages to server
	sendThread = std::thread(&Client::sendMessage, this);

	// Thread for receiving messages from server
	receiveThread = std::thread(&Client::receiveMessage, this);
	receiveThread.join();

	running = false;

	sendThread.join();
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

			if (msg.find("~") == 0)  // History messages in gray color
				SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
			else // Normal messages in green color
				SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);

			std::cout << msg << '\n';
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

		}
	}
}

void Client::sendMessage()
{
	std::cout << "Welcome to chat room, type messages as you want (type 'exit' to quit)\n\n";

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
		int bytesSend = send(clientSocket, message.c_str(), message.length() + 1 , 0); // +1 for /0 terminator from c_str()
		
		if (bytesSend == SOCKET_ERROR) {
			std::lock_guard<std::mutex> lock(coutMutex);
			std::cerr << "[SERVER] Couldn't send the message: " << WSAGetLastError() << '\n';
			running = false;
			break;
		}

	}
}

// Login/Register function
bool Client::authenticate()
{
	std::string choice{}, username{}, password{};

	while (running) {
		std::cout << "1) Login\n2) Register\n> ";
		std::getline(std::cin, choice);

		if (choice != "1" && choice != "2") {
			std::cout << "Invalid choice, try again\n";
			continue;
		}

		std::cout << "\nEnter username: ";
		std::getline(std::cin, username);


		std::cout << "\nEnter password: ";
		std::getline(std::cin, password);

		// Delete white spaces from username and password
		while (!username.empty() && std::isspace(username.back())) username.pop_back();
		while (!password.empty() && std::isspace(password.back())) password.pop_back();

		// Create a command for server
		std::string command = (choice == "1") ? "LOGIN" : "REGISTER";
		std::string message = command + " " + username + " " + password;

#ifdef _DEBUG
		std::cout << "[DEBUG] Sending: " << message << std::endl;
#endif

		if (send(clientSocket, message.c_str(), message.length() + 1, 0) == SOCKET_ERROR) {
			std::cerr << "Failed to send auth request: " << WSAGetLastError() << '\n';
			return false;
		}

		// Return message from server
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



