#include <iostream>
#include <fstream>
#include <ostream>
#include <thread>
#include <vector>
#include <map>
#include <cstring>
#include <winsock2.h>
#include <string>
#include <mutex>
#pragma comment(lib, "ws2_32.lib")

#define PORT 8080

using namespace std;

class Server {
private:
    vector<thread> clients_in_stream;
    vector<thread> clients_out_stream;
    vector<SOCKET> clients_sock_list;
    vector<string>que;
    map<SOCKET, vector<string>>ques;
    SOCKET server_sock, client_sock; // сокет
    sockaddr_in address;
    int addrlen;
    mutex m;
    bool work;
    //int data;
    void sendMsg(SOCKET client);

    void processingClientSock(SOCKET client);
    void acceptClientConnection();
    void sendFile(SOCKET client);

public:
    Server();
    ~Server();
    void startServer();

};

