#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <sstream>
#pragma comment(lib, "ws2_32.lib") // Линковка Winsock библиотеки

#define PORT 8080
using namespace std;
class Client
{
private:
    SOCKET client_sock;
    sockaddr_in serv_addr;
    const char* ip = "127.0.0.1";
    bool work;

    void startChat();
public:
    Client();
    ~Client();
    void startClient();

};

