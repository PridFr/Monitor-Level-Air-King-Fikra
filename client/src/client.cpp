#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <cmath>
#include <chrono>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT "1234"  // Sesuaikan dengan config.h
#define PI 3.14159265

int main() {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct addrinfo* result = nullptr, hints;
    int iResult;

    // Inisialisasi Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve alamat server dan port
    iResult = getaddrinfo(SERVER_IP, SERVER_PORT, &hints, &result);
    if (iResult != 0) {
        std::cerr << "getaddrinfo failed: " << iResult << std::endl;
        WSACleanup();
        return 1;
    }

    // Membuat socket
    sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket failed: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Koneksi ke server
    iResult = connect(sock, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Connect failed: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    std::string tank_id = "t1";
    double time = 0.0;

    while (true) {
        // Fungsi osilasi (sinus), output antara 20–80
        double level = 50.0 + 30.0 * std::sin(time);
        std::string level_str = std::to_string(level);

        std::string data = tank_id + "," + level_str;
        std::cout << "Mengirim data: " << data << std::endl;

        // Kirim ke server
        iResult = send(sock, data.c_str(), (int)data.length(), 0);
        if (iResult == SOCKET_ERROR) {
            std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
            break;
        }

        // Terima respon dari server
        char recvbuf[512];
        int recvResult = recv(sock, recvbuf, sizeof(recvbuf), 0);
        if (recvResult > 0) {
            std::cout << "Server: " << std::string(recvbuf, recvResult) << std::endl;
        } else {
            std::cerr << "Receive failed or connection closed." << std::endl;
            break;
        }

        // Tunggu 2 detik sebelum kirim lagi
        std::this_thread::sleep_for(std::chrono::seconds(2));
        time += 0.5;  // Menambah fase untuk osilasi
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
