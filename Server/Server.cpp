#include "Server.h"

Server::Server(const char* IP, int port) {
    ip = IP;
    PORT = port;
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

    //Присваиваем параметры сокета сервера
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    addrlen = sizeof(address);


    // Привязка сокета к адресу и порту
    if (bind(server_sock, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        cout << "Bind failed" << endl;
        closesocket(server_sock);
        return;
    }

}

Server::~Server() {
    // Закрытие сокетов и очистка Winsock
    for (int client : clients_sock_list) {
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
    cout << "Server is listening on port - " << PORT << " ip - " << ip << endl;
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
        //+Сообщение с информацией о уже подключенных клиентах для нового
        for (int client : clients_sock_list) {
            if (client != client_sock) {
                new_conn = to_string(server_sock) + "." + new_conn + to_string(client_sock);
                if (send(client, new_conn.c_str(), new_conn.size(), 0) == SOCKET_ERROR) {
                    cout << "Send failed" << std::endl;
                }
            }
        }

        //Создание потоков отправки\получения и очередей для подключенного сокета
        clients_in_stream.push_back(thread(&Server::receiveData, this, client_sock));
        clients_out_stream.push_back(thread(&Server::sendMsg, this, client_sock));
        ques[client_sock] = que;
    }

}

void Server::receiveData(SOCKET client) {
    //Обработка сообщений от клиента
    cout << "new Client connected: " << client << endl;
    char buffer[1024];
    int recipient;//получатель
    int data;
    string message;
    while (work) {
        data = recv(client, buffer, 1024, 0);
        if (data <= 0) {
            cout << client << " disconnected. " << endl;
            break;
        }
        else {
            message = string(buffer, data);

            //Если только один подключенный клиент
            if (clients_sock_list.size() == 1) {
                message = to_string(server_sock) + ".There is no availible Clients";
                ques[client].push_back(message);
            }//Проверяем наличие номера сокета получателя...
            else if (message.find(".") != -1) {
                recipient = stoi(message.substr(0, message.find(".")));
                message = message.substr(message.find(".") + 1);
                if (message == "FILE") {
                    cout << "Client " << client << " send file to " << recipient << endl;
                    sendFile(client, recipient);
                }
                else {
                    cout << "Get from " << client << ": " << message << endl;
                }
                ques[recipient].push_back(message);
            }//...в случае отсутствия, отправляем всем подключенным клиентам
            else {
                cout << "Client send file to all connected clients" << endl;
                for (int recipient_client : clients_sock_list) {
                    if (recipient_client != client)
                        ques[recipient_client].push_back(message);
                }
                if (message == "FILE")
                    sendFile(client);
            }
        }
    }
}

void Server::sendMsg(SOCKET client) {
    //отправка сообщений клиенту
    vector <string>* q;
    string message;
    while (work) {
        m.lock();
        q = &ques[client];
        if (!q->empty()) {
            message = q->back();
            cout << "Send to " << client << ": " << message << endl;
            message = to_string(client) + "." + message;
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

void Server::sendFile(SOCKET from_client, SOCKET to_client) {
    char buffer[1024] = { 0 };
    char* file_ptr;
    int file_size, data;
    string file_name;

    //получаем строку [название_файла:размер_файла]
    data = recv(from_client, buffer, 1024, 0);
    file_name = string(buffer, data);

    //парсим строку
    file_size = stoi(file_name.substr(file_name.find(":") + 1));
    if (file_size == -1) {
        send(to_client, file_name.c_str(), sizeof(file_name), 0);
        return;
    }
    //Выделяем память для временного хранения данных
    file_ptr = (char*)malloc(file_size);
    //считываем файл из сокета
    recv(from_client, file_ptr, file_size, 0);

    //Отправляем файл клиенту
    send(to_client, file_name.c_str(), sizeof(file_name), 0);
    send(to_client, file_ptr, file_size, 0);
    free(file_ptr);
}

void Server::sendFile(SOCKET from_client) {
    char buffer[1024] = { 0 };
    char* file_ptr;
    int file_size, data;
    string file_name;
    //получаем строку [название_файла:размер_файла]
    data = recv(from_client, buffer, 1024, 0);

    //парсим строку
    file_name = string(buffer, data);
    file_size = stoi(file_name.substr(file_name.find(":") + 1));
    if (file_size != -1) {
        file_ptr = (char*)malloc(file_size);
        //Получаем файл
        recv(from_client, file_ptr, file_size, 0);
    }
    else {
        file_ptr = (char*)malloc(sizeof(char*));
    }
    //отправляем файл всем подключенным клиентам
    for (int recipient_client : clients_sock_list) {
        if (recipient_client != from_client) {
            send(recipient_client, file_name.c_str(), sizeof(file_name), 0);
            if (file_size != -1) send(recipient_client, file_ptr, file_size, 0);
        }
    }
    free(file_ptr);

}

