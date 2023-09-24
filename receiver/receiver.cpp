#include <iostream>
#include <chrono>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include "opencv2/opencv.hpp"


using namespace std::chrono;
using namespace cv;
using namespace std;


int main() {

    // udp socket setup

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

    // variables for measurement of video stream fps rate, described below
    const int measurement_period = 3000; // in millisecond
    int frames_counter = 0;
    auto time_start = high_resolution_clock::now();
    while (true) {

        const int buffer_size = 100000;
        char buffer[buffer_size];

        // udp packet is received and bytes are placed in buffer
        int bytesReceived = recvfrom(sock, buffer, buffer_size, 0, nullptr, nullptr);
        if (bytesReceived == SOCKET_ERROR) {
            cerr << "Error in recvfrom." << endl;
            continue;
        }

        // each video frame is displayed as separate .jpg in opencv window
        if (bytesReceived > 0) {
            vector<uchar> imageBuffer(buffer, buffer + bytesReceived);
            Mat img = imdecode(imageBuffer, IMREAD_COLOR);
            if (!img.empty()) {
                imshow("Received Image", img);
                waitKey(5);
            } else
                cerr << "Error decoding image." << endl;
        }

        // frames are counted in about measurement_period periods
        // in each period fps rate is refreshed in terminal
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

    // cleanup
    closesocket(sock);
    WSACleanup();

    return 0;
}