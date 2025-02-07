#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <sstream>

#pragma comment(lib, "ws2_32.lib") // Линковка Winsock библиотеки

using namespace std;
class Client
{
private:
    SOCKET client_sock;
    thread sendThread, recvThread;
    sockaddr_in serv_addr;
    const char* ip;
    int PORT;
    bool work;
    int N_try;

    void recieveMsg();
    void sendMsg();
    void sendFile();
    void recieveFile();
public:
    Client(const char* IP, int port);
    ~Client();
    void startClient();

};

