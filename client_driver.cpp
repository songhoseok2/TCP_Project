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
	socket_info server_socket = create_server_socket();

	//Connect to server
	int connection_result = connect(server_socket.sock, (sockaddr*)& server_socket.sock_addr, sizeof(server_socket.sock_addr));
	if (connection_result == SOCKET_ERROR)
	{
		exit_with_err_msg("Connection to server failed. Error #" + to_string(WSAGetLastError()) + ". Exiting.");
	}

	client_requests(server_socket);

	cout << "Closing client." << endl;

	closesocket(server_socket.sock);
	WSACleanup();

	return 0;
}