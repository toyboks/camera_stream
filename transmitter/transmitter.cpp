#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include "opencv2/opencv.hpp"


using namespace std;
using namespace cv;


// finds best dimensions to downsize image keeping aspect ratio
Size find_size(const int max_len, Mat sample_img) {
    int cols = sample_img.cols;
    int rows = sample_img.rows;
    
    while(cols * rows > max_len) {
        (int)rows = cols * (cols-1) / rows;
        cols --;
    }
    return Size(cols, rows);
}


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

    const char receiver_addr[] = "127.0.0.1";
    const int receiver_port = 12345;
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(receiver_port);
    serverAddr.sin_addr.s_addr = inet_addr(receiver_addr);
    
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "Error: Could not open camera." << endl;
        return -1;
    }

    Mat sample_img;
    cap.read(sample_img);
    const int max_bytes = 100000;
    Size img_size = find_size(max_bytes, sample_img);
    
    while (true) {
        Mat img;
        cap.read(img);
        if (img.empty()) {
            cerr << "Error getting img" << endl;
            continue;
        }

        resize(img, img, img_size, INTER_LINEAR);

        vector<uchar> buffer;
        imencode(".jpg", img, buffer);

        imshow("Camera preview", img);
        waitKey(5);
        
        sendto(sock, (const char*)buffer.data(), (int)buffer.size(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    }

    closesocket(sock);
    WSACleanup();
    cap.release();

    return 0;
}