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
    // Ожидание подключений
    if (listen(server_sock, 3) == SOCKET_ERROR) {
        cout << "Listen failed" << endl;
        closesocket(server_sock);
        return;
    }
    cout << "Server is listening on port " << PORT << endl;
    work = true;
    acceptClientConnection();
}

void Server::acceptClientConnection() {
    while (work) {
        //принимаем соединения с клиентами
        client_sock = accept(server_sock, (struct sockaddr*)&address, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            cout << "Accept failed" << endl;
            closesocket(server_sock);
            return;
        }
        //Запуск потоков отправки\получения
        clients.push_back(thread(&Server::processingClientSock, this, client_sock));
        thread sendback(&Server::sendMsg, this, client_sock);
        clients.back().join();
        sendback.join();
    }

}

void Server::processingClientSock(SOCKET client) {
    //Обработка сообщений от клиента
    cout << "new Client connected" << client<<endl;
    char buffer[1024];
    int data;
    while (work) {
        data = recv(client, buffer, 1024, 0);
        if (data <= 0) {
            cout << client << " disconnected" << endl;
            break;
        }
        else {
            string message(buffer, data);
            cout << "Received from " << client << ": " << message << endl;
            que.push_back(message);            
        }


        //cout << "get/recieve msg" << endl;
    }
}

void Server::sendMsg(SOCKET client) {
    //отправка сообщений клиенту
    string message;
    while (work) {
        if (!que.empty()) {
            message = que.back();
            cout << "send   " << message<<endl;
            que.pop_back();
            if (send(client, message.c_str(), message.size(), 0) == SOCKET_ERROR) {
                cout << "Send failed" << std::endl;
                work = false;
                break;
            }
        }
    }
}