#include <iostream>
#include <string>
#include "client.h"
#include <WS2tcpip.h>

using namespace std;

socket_info create_client_socket()
{
	socket_info client_socket;
	client_socket.sock = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket.sock == INVALID_SOCKET)
	{
		exit_with_err_msg("Socket creation failed. Error #" + to_string(WSAGetLastError()) + ". Exiting.");
	}

	string IP_address;
	string port_num_str;

	while (getline(cin, IP_address))
	{
		if (IP_address.empty()) { cout << "IP address is empty. Please re-enter: "; }
		else { break; }
	}
	while (getline(cin, port_num_str))
	{

	}

	client_socket.sock_addr.sin_family = AF_INET;
	client_socket.sock_addr.sin_port = htons(port_num);
	inet_pton(AF_INET, IP_address.c_str(), &client_socket.sock_addr.sin_addr);

	return client_socket;
}