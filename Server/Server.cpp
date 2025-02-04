#include "Server.h"

Server::Server() {
    //WSAStartup инициирует использование winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSAStartup failed" << endl;
        return;
    }

    //инициализация сокета
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        cout << "Socket creation failed" << endl;
        return;
    }

    //Присваиваем параметры сокета
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    addrlen = sizeof(address);

    // Привязка сокета к адресу и порту
    if (bind(server_sock, (struct sockaddr*)&address, addrlen) == SOCKET_ERROR) {
        cout << "Bind failed" << endl;
        closesocket(server_sock);
        return;
    }

}

Server::~Server() {
    // Закрытие сокетов и очистка Winsock
    closesocket(client_sock);
    closesocket(server_sock);
    WSACleanup();
}

void Server::startServer() {

    work = true;

    // Ожидание подключений
    if (listen(server_sock, 3) == SOCKET_ERROR) {
        cout << "Listen failed" << endl;
        closesocket(server_sock);
        return;
    }
    cout << "Server is listening on port " << PORT << endl;

    acceptClientConnection();
}

void Server::acceptClientConnection() {
    while (work) {
        //принимаем соединения с клиентами
        if (client_sock = accept(server_sock, (struct sockaddr*)&address, &addrlen) == INVALID_SOCKET) {
            cout << "Accept failed" << endl;
            closesocket(server_sock);
            return;
        }
        clients.push_back(thread(&Server::pricessingClientSock, this, client_sock));
        cout << "new Client connected" << endl;
    }

}

void Server::pricessingClientSock(SOCKET client) {
    cout << "get/recieve msg" << endl;
}