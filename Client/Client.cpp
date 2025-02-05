
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
    cout << "Client Connected to server" << endl;
    work = true;
    //Запуск потоков отправки\получения
    sendThread = thread(&Client::sendMsg, this, client_sock);
    recvThread = thread(&Client::recieveMsg, this, client_sock);
    sendThread.join();
    recvThread.join();
    
}

void Client::sendMsg(SOCKET client) {
    //Отпавка сообщения на сервер
    string message;
    while (work) {
        cout << "To send a message to certain Client enter: <Client's Number>.<Your message>" << endl;
        cout << ">>";
        getline(cin, message);
        if (send(client_sock, message.c_str(), message.size(), 0) == SOCKET_ERROR) {
            cout << "Send failed" << endl;
            work = false;
            break;
        }
    }
}
void Client::recieveMsg(SOCKET client) {
    //Прием сообщений от сервера
        char buffer[1024] = { 0 };
        int data;
        string message;
        while (work) {
            memset(buffer, 0, 1024);
            data = recv(client, buffer, 1024, 0);
            if ( data<= 0) {
                cout << "Server disconnected" << endl;
                break;
            }
            message = string(buffer, data);
            data = stoi(message.substr(0, message.find(".")));
            message = message.substr(message.find(".") + 1);
            cout<<"From "<<data << ": " << message << endl;
        }
}