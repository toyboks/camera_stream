#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include "opencv2/opencv.hpp"

int main() {

    cv::VideoCapture cap(0);

    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open camera." << std::endl;
        return -1;
    }

    cv::Mat frame;


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
    serverAddr.sin_port = htons(12345); // Replace with the desired destination port
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Replace with the destination IP address

    char message = 'b';

    while (true) {
        cap.read(frame);
        if (frame.empty()) {
            break;
        }

        int down_width = 100;
        int down_height = 100;
        resize(frame, frame, cv::Size(down_width, down_height), cv::INTER_LINEAR);

        std::vector<uchar> buffer;
        cv::imencode(".jpg", frame, buffer);
        if (!frame.empty()) {
            cv::imshow("Received Image", frame);
            cv::waitKey(1); // Adjust the delay as needed
        } else {
            std::cout << "Error decoding image." << std::endl;
        }
        
        //message = buffer[0];
        //sendto(sock, &message, sizeof(message), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        sendto(sock, (const char*)buffer.data(), buffer.size(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        std::cout << buffer.size() << std::endl;

        Sleep(1000); // Send 'a' every 1 second
    }

    closesocket(sock);
    WSACleanup();
    cap.release();

    return 0;
}