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
	int connection_result = connect(sock, (sockaddr*)& hint, sizeof(hint));
	if (connection_result == SOCKET_ERROR)
	{
		cout << "Connection to server failed. Error #" << WSAGetLastError() << ". Exiting." << endl;
		WSACleanup();
		return 0;
	}

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
			int send_result = send(sock, user_input.c_str(), int(user_input.size() + 1), 0);
			if (send_result != SOCKET_ERROR)
			{
				//Wait for response
				ZeroMemory(buffer, 4096);
				int bytes_received = recv(sock, buffer, 4096, 0);
				if (bytes_received > 0)
				{
					//Echo response to console
					cout << "SERVER: " << string(buffer, 0, bytes_received) << endl;
				}
			}
		}

	} while (user_input.size() > 0);

	//Close
	closesocket(sock);
	WSACleanup();

	return 0;
}