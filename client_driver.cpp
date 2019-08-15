#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include "client.h"
#pragma comment (lib, "ws2_32.lib")

using namespace std;

int main()
{
	initialize_winsock();

	//Create socket
	socket_info server_socket;

	//Connect to server
	connect_to_server(server_socket);
	client_requests(server_socket);

	cout << "Closing client." << endl;
	closesocket(server_socket.sock);
	WSACleanup();

	return 0;
}