#include <iostream>
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
    SOCKET server_sock, client_sock; // �����
    sockaddr_in address;
    int addrlen;
    mutex m;
    //int data;
    void sendMsg(SOCKET client);

    bool work;

    void processingClientSock(SOCKET client);
    void acceptClientConnection();

public:
    Server();
    ~Server();
    void startServer();

};

