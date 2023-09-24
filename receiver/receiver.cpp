#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include "opencv2/opencv.hpp"

int main() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return -1;
    }

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock == INVALID_SOCKET) {
        std::cerr << "Error: Could not create socket." << std::endl;
        WSACleanup();
        return -1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345); // Use the same port as the server
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Listen on all available network interfaces

    if (bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed." << std::endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    char buffer[32000];

    while (true) {
        int bytesReceived = recvfrom(sock, buffer, sizeof(buffer), 0, nullptr, nullptr);
        std::cout << "received" << std::endl;

        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "Error in recvfrom." << std::endl;
            break;
        }

        if (bytesReceived > 0) {
            std::vector<uchar> imageBuffer(buffer, buffer + bytesReceived);
            cv::Mat img = cv::imdecode(imageBuffer, cv::IMREAD_COLOR);
            if (!img.empty()) {
                cv::imshow("Received Image", img);
                cv::waitKey(1); // Adjust the delay as needed
            } else {
                std::cout << "Error decoding image." << std::endl;
            }
            // Print the received data to the terminal
            std::cout.write(buffer, bytesReceived);
            std::cout << std::endl;
            //Sleep(3000); // Send 'a' every 1 second
        }

        
    }

    closesocket(sock);
    WSACleanup();

    return 0;
}