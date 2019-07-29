#include <iostream>
#include <string>
#include "client.h"
#include <WS2tcpip.h>

using namespace std;

socket_info create_client_socket()
{
	cout << "Initializing client socket" << endl;
	socket_info client_socket;
	client_socket.sock = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket.sock == INVALID_SOCKET)
	{
		exit_with_err_msg("Socket creation failed. Error #" + to_string(WSAGetLastError()) + ". Exiting.");
	}

	string IP_address;
	cout << "Please enter the IP address of the server: ";
	while (getline(cin, IP_address))
	{
		if (IP_address.empty()) { cout << "IP address is empty. Please re-enter: "; }
		else { break; }
	}

	int port_num = get_port_number();
	client_socket.sock_addr.sin_family = AF_INET;
	client_socket.sock_addr.sin_port = htons(port_num);
	inet_pton(AF_INET, IP_address.c_str(), &client_socket.sock_addr.sin_addr);

	cout << "client socket initialized." << endl;
	return client_socket;
}

void temporary_client_action(socket_info &client_socket)
{
	//While loop to send and receive data
	char buffer[4096];
	string user_input;

	do
	{
		//Prompt the user for some text
		cout << "Enter message: ";
		getline(cin, user_input);

		if (user_input.size() > 0)
		{
			//Send the text
			int send_result = send(client_socket.sock, user_input.c_str(), int(user_input.size() + 1), 0);
			if (send_result != SOCKET_ERROR)
			{
				//Wait for response
				ZeroMemory(buffer, 4096);
				int bytes_received = recv(client_socket.sock, buffer, 4096, 0);
				if (bytes_received > 0)
				{
					//Echo response to console
					cout << "SERVER: " << string(buffer, 0, bytes_received) << endl;
				}
			}
		}

	} while (user_input.size() > 0);
}