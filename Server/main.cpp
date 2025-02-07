#include "Server.h"
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
	Server* server = new Server(ip, port);
	server->startServer();
	return 0;
}