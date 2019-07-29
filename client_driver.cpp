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
	socket_info client_socket = create_client_socket();

	//Connect to server
	int connection_result = connect(client_socket.sock, (sockaddr*)& client_socket.sock_addr, sizeof(client_socket.sock_addr));
	if (connection_result == SOCKET_ERROR)
	{
		exit_with_err_msg("Connection to server failed. Error #" + to_string(WSAGetLastError()) + ". Exiting.");
	}

	temporary_client_action(client_socket);

	closesocket(client_socket.sock);
	WSACleanup();

	return 0;
}