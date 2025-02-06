
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

    N_try = 0;
}

Client::~Client() {
    // Закрытие сокета и очистка Winsock
    cout << "Client OFF";
    closesocket(client_sock);
    WSACleanup();
}

void Client::startClient() {
    //Подключение к серверу
    while (connect(client_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR && N_try < 5) {
        cout << "Server Is Not Availible. Trying To Reconnect" << endl;
        Sleep(1000);
        //N_try++; if(N_try==5)cout<<"Can't access Server.";return;"Failed to connect to server after " << MAX_RETRIES << " attempts\n"
    }
 
    cout << "Client Connected to server" << endl;
    work = true;

    //Запуск потоков отправки\получения
    sendThread = thread(&Client::sendMsg, this);
    recvThread = thread(&Client::recieveMsg, this);
    sendThread.join();
    recvThread.join();
    
}

void Client::sendMsg() {
    //Отпавка сообщения на сервер
    string message;
    char buffer[1024] = { 0 };
    cout << "To send a message to certain Client enter: <Client's Number>.<Your message>" << endl;
    cout << "To send a file enter: FILE" << endl;
    while (work) {
        cout << ">>";
        getline(cin, message);

        if (message == "") { continue; }

        if (message == "!STOP!") {//Завершение работы чата/клиента
            work = false;
            closesocket(client_sock);
            break;
        }else if (message.find("FILE")!=-1) {//Отправка файла
            send(client_sock, message.c_str(), message.size(), 0);
            sendFile();
        }else if (send(client_sock, message.c_str(), message.size(), 0) == SOCKET_ERROR) {//Отправка сообщения
            cout << "Send failed" << endl;
            work = false;
            break;
        }
    }
}

void Client::recieveMsg() {
    //Прием сообщений от сервера
    char buffer[1024] = { 0 };
    int data, sender;
    string message;
    while (work) {
        memset(buffer, 0, 1024);
        data = recv(client_sock, buffer, 1024, 0);
        if ( data == -1) {
            cout << "Server and Client are disconnected. Client will be turned off." << endl;
            work=false;
            closesocket(client_sock);
            return;
        }
        message = string(buffer, data);
        sender = stoi(message.substr(0, message.find(".")));
        message = message.substr(message.find(".") + 1);
        if (message == "FILE") {
            cout << sender << " send You a file." << endl;
            recieveFile();
        }else cout<<"From "<< sender << ": " << message << endl;
    }
}

void Client::sendFile() {
    string message;
    int file_size;
    char* buffer;
    //считываем название файла и путь
    cout << "Enter name of file" << endl;//+path to
    getline(cin, message);
    ifstream file(message.c_str(), ios::binary);

    //определяем размер файла
    file.seekg(0,ios::end);
    file_size = file.tellg();
    buffer = (char*)malloc(file_size);

    //отправляем название файла и размер
    message += ":" + to_string(file_size);
    send(client_sock, message.c_str(), message.size(), 0);
    // Отправляем содержимое файла
    file.read(buffer, file_size);
    send(client_sock, buffer, file_size, 0);
    free(buffer);
}

void Client::recieveFile() {
    char buffer[1024];
    char* file_ptr;
    int file_size, data;
    string file_name;
    int bytesReceived = 0;
    int all_data = 0;

    //получаем строку [название_файла:размер_файла]
    data = recv(client_sock, buffer, 1024, 0);
    file_name = string(buffer, data);
    //парсим строку
    file_size = stoi(file_name.substr(file_name.find(":") + 1));
    file_name = file_name.substr(0, file_name.find(":"));
    //Выделяем память для записи получаемого файла
    file_ptr = (char*)malloc(file_size);

    //cout << "Enter path to save file : " << endl;
    //getline(cin, file_name);
    
    //создаем файл и считываем файл из сокета
    ofstream file(file_name, ios::binary);
    
    recv(client_sock, file_ptr, file_size, 0);
    file.write(buffer, file_size);
    
    cout << "File Recived" << endl;
    free(file_ptr);
}