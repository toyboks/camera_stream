/*#include "main.h"

using namespace std;
using namespace cv;

int main( int argc, char ** argv )
{

	namedWindow( "test", WINDOW_NORMAL);

    	VideoCapture cap;

	if ( argc == 1 ){

		cap.open(0);   // To open the first camera
	}
	else
	{
		cap.open( argv[1] );
	}

 	if ( ! cap.isOpened( ) ) {
		cerr << "couldn't open capture."<<endl;
		return -1;
	}

	Mat frame;

	while( 1 ) {

		cap>>frame;

		if( frame.empty( ) ) 
			break;

		imshow ( "test", frame );

		if ( waitKey ( 30 ) >= 0 )
			break;

	}

	return 0;

}*/

#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

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
    serverAddr.sin_port = htons(12345); // Replace with the desired destination port
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Replace with the destination IP address

    char message = 'b';

    while (true) {
        sendto(sock, &message, sizeof(message), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        Sleep(1000); // Send 'a' every 1 second
    }

    closesocket(sock);
    WSACleanup();

    return 0;
}