#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <ctime>
#include <cstdlib>
#include "json.hpp"  

#pragma comment(lib, "ws2_32.lib")

using json = nlohmann::json;
using namespace std;

const string SERVER_IP = "127.0.0.1";  
const int SERVER_PORT = 12345;
const string ID_TANGKI = "TANGKI_AIR_CIHUYY";  

float baca_level_air() {
    return static_cast<float>(rand() % 101);  
}

string get_timestamp() {
    time_t now = time(0);
    tm gmtm;
    gmtime_s(&gmtm, &now);
    char buf[30];
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &gmtm);
    return string(buf);
}

string buat_pesan() {
    json data;
    data["id_tangki"] = ID_TANGKI;
    data["timestamp"] = get_timestamp();
    data["level"] = baca_level_air();
    return data.dump();
}

int main() {
    srand(static_cast<unsigned>(time(NULL)));

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup gagal.\n";
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Gagal membuat socket.\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP.c_str(), &serverAddr.sin_addr);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Gagal terhubung ke server.\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    cout << "? Terhubung ke server\n";

    while (true) {
        string pesan = buat_pesan();
        int sent = send(clientSocket, pesan.c_str(), pesan.length(), 0);
        if (sent == SOCKET_ERROR) {
            cerr << "? Gagal mengirim data\n";
            break;
        }
        cout << "?? Terkirim: " << pesan << endl;
        Sleep(5000);  // delay 5 detik
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}

