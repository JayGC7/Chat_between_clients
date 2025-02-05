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
    for(int client:clients_sock_list){
    closesocket(client);
    }
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
    string new_conn = "Connected new client: ";
    while (work) {
        //принимаем соединения с клиентами
        client_sock = accept(server_sock, (struct sockaddr*)&address, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            cout << "Accept failed" << endl;
            closesocket(server_sock);
            return;
        }
        clients_sock_list.push_back(client_sock);

        //Оповещение подключенных клиентов о новом клиете в сети
        for (int client : clients_sock_list) {
            if (client != client_sock) {
                new_conn = to_string(server_sock) +"."+ new_conn + to_string(client_sock);
                if (send(client, new_conn.c_str(), new_conn.size(), 0) == SOCKET_ERROR) {
                    cout << "Send failed" << std::endl;
                }
            }
        }
        //Создание потоков отправки\получения и очередей для подключенного сокета
        clients_in_stream.push_back(thread(&Server::processingClientSock, this, client_sock));
        clients_out_stream.push_back(thread(&Server::sendMsg, this, client_sock));
        ques[client_sock] = que;
    }
    //Запуск потоков отправки\получения
    for (int i = 0; i < clients_in_stream.size(); i++) {
        clients_in_stream.back().join();
        clients_out_stream.back().join();
        clients_in_stream.pop_back();
        clients_out_stream.pop_back();
    }
    

}

void Server::processingClientSock(SOCKET client) {
    //Обработка сообщений от клиента
    cout << "new Client connected: " << client<<endl;
    char buffer[1024];
    int recipient;//получатель
    string message;
    while (work) {
        recipient = recv(client, buffer, 1024, 0);
        if (recipient <= 0) {
            cout << client << " disconnected" << endl;
            break;
        }
        else {
            message =string(buffer, recipient);
            //Проверяем наличие номера сокета получателя...
            if(message.find(".")!=-1)  recipient = stoi(message.substr(0, message.find(".")));
            else{//...в случае отсутствия, отправляем всем подключенным клиентам
                for (int client : clients_sock_list) {
                    if (client != client_sock) {
                        message = to_string(client_sock) + "." + message;
                        send(client, message.c_str(), message.size(), 0);
                    }
                }
                continue;
            }
            
            cout << "Get from " << client << ": " << message << endl;
            message = to_string(client)+"."+message.substr(message.find(".") + 1);//Добавляем номер сокета отправителя
            ques[recipient].push_back(message);
        }
        //cout << "get/recieve msg" << endl;
    }
}

void Server::sendMsg(SOCKET client) {
    //отправка сообщений клиенту
    vector <string> *q;
    string message;
    while (work) {
        m.lock();
        q = &ques[client];
        if (!q->empty()) {
            message = q->back();
            cout << "Send to" << client <<": " << message << endl;
            q->pop_back();

            if (send(client, message.c_str(), message.size(), 0) == SOCKET_ERROR) {
                cout << "Send failed" << std::endl;
                work = false;
                //break;
            }
        }
        m.unlock();
    }
}