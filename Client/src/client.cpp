#include <iostream>
#include <WS2tcpip.h>
#include <WinSock2.h>

#define SERVER_IP "127.0.0.1"
#define PORT 55555

int main() {
	WSADATA wsaData;
	WORD version = MAKEWORD(2, 2);

	// Find dll and initialize winsock
	if (WSAStartup(version, &wsaData) != 0) {
		std::cerr << "Couldn't initialize winsock...\n";
		return 0;
	}
	else {
		std::cout << "Winsock dll found!\n";
		std::cout << "Status: " << wsaData.szSystemStatus << std::endl;
	}

	// Create client sockiet [IPv4, TCP]
	SOCKET clientSocket = INVALID_SOCKET;
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "Couldn't create a client socket\n";
		WSACleanup();
		return 0;
	}
	else {
		std::cout << "Client socket created!\n";
	}

	// Connect client socket to server socket passed in "SERVER_IP"
	sockaddr_in clientAddr;
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(PORT);
	inet_pton(AF_INET, SERVER_IP, &clientAddr.sin_addr.s_addr);

	if (connect(clientSocket, (sockaddr*)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR) {
		std::cerr << "Couldn't connect to the server\n";
		WSACleanup();
		return 0;
	}
	else {
		std::cout << "Connected!\n";
	}

	char buffer[200];

	std::cout << "Msg: ";
	std::cin.getline(buffer, 200);

	int sendBytes = send(clientSocket, buffer, sizeof(buffer), 0);
	
	if (sendBytes > 0) {
		std::cout << "Message was sent!\n";
	}
	else {
		std::cerr << "Couldn't send the message...\n";
		WSACleanup();
	}

	sendBytes = recv(clientSocket, buffer, sizeof(buffer), 0);

	if (sendBytes > 0) {
		std::cout << "Received message: " << buffer << std::endl;
	}
	else {
		std::cerr << "Couldn't receive the message...\n";
		WSACleanup();
	}

	system("pause");
	WSACleanup();


	return 0;
}