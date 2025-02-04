#include <iostream>
#include <ostream>
#include <thread>
#include <vector>
#include <cstring>
#include <winsock2.h>
#include <string>
#pragma comment(lib, "ws2_32.lib")

#define PORT 8080

using namespace std;

class Server {
private:
    vector<thread> clients;
    SOCKET server_sock, client_sock; // сокет
    sockaddr_in address;
    int addrlen;
    bool work;

    void pricessingClientSock(SOCKET client);
    void acceptClientConnection();

public:
    Server();
    ~Server();
    void startServer();

};

