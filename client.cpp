#include <iostream>
#include <string>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

int main()
{
	string IP_address = "127.0.0.1";	//IP address of the server
	int port_num = 54000;				//Listening port number of the server

	//Initialize Winsock
	WSADATA data;
	WORD ver = MAKEWORD(2, 2);
	int ws_result = WSAStartup(ver, &data);
	if (ws_result != 0)
	{
		cout << "Winsock initialization failed. Error #" << ws_result << ". Exiting." << endl;
		return 0;
	}

	//Create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cout << "Socket creation failed. Error #" << WSAGetLastError() << ". Exiting." << endl;
		return 0;
	}

	//Fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port_num);
	inet_pton(AF_INET, IP_address.c_str(), &hint.sin_addr);

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