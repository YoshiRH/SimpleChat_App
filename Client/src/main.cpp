#include "../include/Client.h"

int main() {
	Client client;

	if(!client.Connect()) return 1;
	client.Run();

	return 0;
}