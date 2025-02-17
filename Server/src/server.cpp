#include <iostream>
#include <WS2tcpip.h>
#include <WinSock2.h>

#define SERVER_IP "127.0.0.1"
#define PORT 55555

int main() {
	SOCKET serverSocket, acceptSocket;
	WSADATA wsaData;
	WORD version = MAKEWORD(2, 2);

	// Search for dll and initialize Winsock
	if (WSAStartup(version, &wsaData) != 0) {
		std::cerr << "Couldn't initialize winsock...\n";
		return 0;
	}
	else {
		std::cout << "Winsock dll found!\n";
		std::cout << "Status: " << wsaData.szSystemStatus << std::endl;
	}

	// Create server socket [IPv4, TCP]
	serverSocket = INVALID_SOCKET;
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (serverSocket == INVALID_SOCKET) {
		std::cerr << "Couldn't create a server socket\n";
		WSACleanup();
		return 0;
	}
	else {
		std::cout << "Server socket created!\n";
	}

	// Bind addres to serverSocket
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr.s_addr);

	if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "Couldn't bind a server socket: " << WSAGetLastError() << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		return 0;
	}
	else {
		std::cout << "Server socket binded!\n";
	}

	// Start listenning for connection
	if (listen(serverSocket, 1) == SOCKET_ERROR) {
		std::cerr << "Error while listening on socket: " << WSAGetLastError() << std::endl;
		return 0;
	}
	else {
		std::cout << "Waiting for connection...\n";
	}

	// Accept connection
	acceptSocket = accept(serverSocket, nullptr, nullptr);
	if (acceptSocket == INVALID_SOCKET) {
		std::cerr << "Error while accepting the connection: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 0;
	}
	else {
		std::cout << "Connection accepted!\n";
	}

	char buffer[200];
	int byteCount = recv(acceptSocket, buffer, sizeof(buffer), 0);

	if (byteCount > 0) {
		std::cout << "Received message: " << buffer << std::endl;
	}
	else {
		std::cerr << "Couldn't receive the message...\n";
		WSACleanup();
	}

	char serverMsg[200] = "Message Received";
	byteCount = send(acceptSocket, serverMsg, sizeof(serverMsg), 0);

	if (byteCount > 0) {
		std::cout << "Send message back to client\n";
	}
	else {
		std::cerr << "Couldn't send message to client\n";
		WSACleanup();
	}

	system("pause");
	WSACleanup();

	return 0;
};