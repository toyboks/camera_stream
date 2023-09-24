#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include "opencv2/opencv.hpp"


using namespace cv;
using namespace std;


int main() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0) {
        cerr << "WSAStartup failed: " << result << endl;
        return -1;
    }

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock == INVALID_SOCKET) {
        cerr << "Error: Could not create socket." << endl;
        WSACleanup();
        return -1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (::bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Bind failed." << endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    const int buffer_size = 10000;
    char buffer[buffer_size];

    while (true) {
        int bytesReceived = recvfrom(sock, buffer, buffer_size, 0, nullptr, nullptr);
        cout << "Received" << endl;

        if (bytesReceived == SOCKET_ERROR) {
            cerr << "Error in recvfrom." << endl;
            continue;
        }

        if (bytesReceived > 0) {
            vector<uchar> imageBuffer(buffer, buffer + bytesReceived);
            Mat img = imdecode(imageBuffer, IMREAD_COLOR);
            if (!img.empty()) {
                imshow("Received Image", img);
                waitKey(1);
            } else
                cerr << "Error decoding image." << endl;   
        }
    }

    closesocket(sock);
    WSACleanup();

    return 0;
}