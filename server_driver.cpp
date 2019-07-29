#include <iostream>
#include <string>
#include "server.h"
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

int main()
{
	initialize_winsock();
	socket_info listening_socket = create_listening_socket();

	//Bind ip address and port to a socket
	if (bind(listening_socket.sock, (sockaddr*)& listening_socket.sock_addr, sizeof(listening_socket.sock_addr)) < 0)
	{
		exit_with_err_msg("Binding listening socket failed.");
	}

	//listen() tells the socket to listen to the incoming connections.
	//The listen() function places all incoming connection into a backlog queue
	//until accept() call accepts the connection.
	//Setting the maximum size for the backlog queue to a defined number.
	listen(listening_socket.sock, SOMAXCONN);

	socket_info client_socket = connect_to_a_client(listening_socket);
	temporary_action(client_socket);

	closesocket(client_socket.sock);
	WSACleanup();

	return 0;
}