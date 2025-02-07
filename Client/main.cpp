#include "Client.h"
int main(int argc, char* argv[]) {
	const char* ip;
	int port;
	if (argc < 3) {
		ip = "127.0.0.2";
		port = 8080;
	}
	else {
		ip = argv[1];
		port = stoi(argv[2]);
	}

	Client* client = new Client(ip, port);
	client->startClient();
	return 0;
}