#include <iostream>
#include <fstream>
#include <ostream>
#include <thread>
#include <vector>
#include <map>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <mutex>
#pragma comment(lib, "ws2_32.lib")


using namespace std;

class Server {
private:
    int PORT;
    const char* ip;
    SOCKET server_sock, client_sock;
    sockaddr_in address;
    vector<thread> clients_in_stream;
    vector<thread> clients_out_stream;
    vector<SOCKET> clients_sock_list;
    vector<string>que;
    map<SOCKET, vector<string>>ques;
    int addrlen;
    mutex m;
    bool work;
    //int data;
    void sendMsg(SOCKET client);
    void sendFile(SOCKET from_client, SOCKET to_client);
    void sendFile(SOCKET from_client);
    void receiveData(SOCKET client);
    void acceptClientConnection();

public:
    Server(const char* IP, int port);
    ~Server();
    void startServer();

};

