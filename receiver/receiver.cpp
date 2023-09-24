#include <iostream>
#include <chrono>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include "opencv2/opencv.hpp"


using namespace std::chrono;
using namespace cv;
using namespace std;


int main() {

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        cerr << "WSAStartup failed: " << endl;
        return -1;
    }

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "Failed to create socket." << endl;
        WSACleanup();
        return -1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (::bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Bind failed" << endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    int measurement_period = 3000; // in millisecond
    int frames_counter = 0;
    auto time_start = high_resolution_clock::now();
    while (true) {

        const int buffer_size = 100000;
        char buffer[buffer_size];

        int bytesReceived = recvfrom(sock, buffer, buffer_size, 0, nullptr, nullptr);

        if (bytesReceived == SOCKET_ERROR) {
            cerr << "Error in recvfrom." << endl;
            continue;
        }

        if (bytesReceived > 0) {
            vector<uchar> imageBuffer(buffer, buffer + bytesReceived);
            Mat img = imdecode(imageBuffer, IMREAD_COLOR);
            if (!img.empty()) {
                imshow("Received Image", img);
                waitKey(5);
            } else
                cerr << "Error decoding image." << endl;
        }

        frames_counter ++;
        auto time_stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(time_stop - time_start);
        if (duration.count() >= measurement_period) {
            time_start = high_resolution_clock::now();
            long long fps_rate = (1000 * frames_counter)  / duration.count();
            system("cls");
            cout << "FPS rate: " << fps_rate << endl;
            frames_counter = 0;
        }
    }

    closesocket(sock);
    WSACleanup();

    return 0;
}