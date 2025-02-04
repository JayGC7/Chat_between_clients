
#include "Client.h"

Client::Client() {
    //WSAStartup инициирует использование winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSAStartup failed" << endl;
        return;
    }

    // Создание сокета
    if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        cout << "Socket creation failed" << endl;
        return;
    }
    //Присваиваем параметры сокета
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, ip, &serv_addr.sin_addr);
}

Client::~Client() {
    // Закрытие сокета и очистка Winsock
    closesocket(client_sock);
    WSACleanup();
}

void Client::startClient() {
    // Подключение к серверу
    if (connect(client_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        cout << "Connection Failed" << endl;
        return;
    }
    cout << "Connected to server" << endl;
    work = true;
    startChat();
}

void Client::startChat() {
    //while(work){...}
    cout << "send/recieve msg" << endl;
}