#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include "opencv2/opencv.hpp"


using namespace std;
using namespace cv;


Size find_size(const int max_len, Mat sample_frame);


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

    Mat sample_frame;
    cap.read(sample_frame);
    const int max_bytes = 10000;
    Size frame_size = find_size(max_bytes, sample_frame);
    

    while (true) {
        Mat frame;
        cap.read(frame);
        if (frame.empty()) {
            cerr << "Error getting frame" << endl;
            continue;
        }

        resize(frame, frame, frame_size, INTER_LINEAR);

        vector<uchar> buffer;
        imencode(".jpg", frame, buffer);

        imshow("Camera preview", frame);
        waitKey(1); // Adjust the delay as needed
        
        sendto(sock, (const char*)buffer.data(), buffer.size(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    }

    closesocket(sock);
    WSACleanup();
    cap.release();

    return 0;
}


// finds best dimensions to downsize image keeping aspect ratio
Size find_size(const int max_len, Mat sample_frame) {
    int cols = sample_frame.cols;
    int rows = sample_frame.rows;
    
    while(cols * rows > max_len) {
        (int)rows = cols * (cols-1) / rows;
        cols --;
    }
    return Size(cols, rows);
}