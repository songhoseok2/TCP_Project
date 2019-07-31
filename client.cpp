#include <iostream>
#include <string>
#include "client.h"
#include <WS2tcpip.h>

using namespace std;

socket_info create_server_socket()
{
	cout << "Initializing server socket." << endl;
	socket_info server_socket;
	server_socket.sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket.sock == INVALID_SOCKET)
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
	server_socket.sock_addr.sin_family = AF_INET;
	server_socket.sock_addr.sin_port = htons(port_num);
	inet_pton(AF_INET, IP_address.c_str(), &server_socket.sock_addr.sin_addr);
	inet_pton(AF_INET, IP_address.c_str(),server_socket.IP_address);
	server_socket.port_num = port_num;
	cout << "Server socket initialized." << endl;
	return server_socket;
}

bool send_request(socket_info& server_socket, const char request)
{
	char request_buff[1];
	request_buff[0] = request;
	int send_result = send(server_socket.sock, request_buff, 2, 0); //2 cuz one char + terminating null
	if (send_result == SOCKET_ERROR)
	{
		exit_with_err_msg("Error in sending request to server. Error #" + to_string(WSAGetLastError()) + ". Exiting.");
		return false;
	}
	else
	{
		//Wait for request_acceptance_result from server
		char request_acceptance_result_buff[1];
		ZeroMemory(request_acceptance_result_buff, 1);
		int bytes_received = recv(server_socket.sock, request_acceptance_result_buff, 1, 0);

		if (bytes_received < 0)
		{
			exit_with_err_msg("Error in receiving request acceptance result from server. Error #" + to_string(WSAGetLastError()) + ". Exiting.");
		}
		else
		{
			cout << "DEBUG: bytes_received: " << bytes_received << endl;
			cout << "DEBUG: request_acceptance_result_buff: " << request_acceptance_result_buff << endl;
			string request_acceptance_result = string(request_acceptance_result_buff, 0, bytes_received);
			cout << "DEBUG: request_acceptance_result: " << request_acceptance_result << endl;
			if (request_acceptance_result == "y")
			{
				cout << "Server accepted the request of " << request << "." << endl;
				return true;
			}
			else
			{
				cout << "Server denied the request of " << request << "." << endl;
				return false;
			}		
		}
	}

	exit_with_err_msg("Error in send_request function. Exiting.");
	return false;
}

void send_message(socket_info &server_socket)
{
	char msg_buff[4096];
	string user_msg = get_message();
	strcpy_s(msg_buff, user_msg.c_str());
	if (user_msg == "ABORT")
	{
		return;
	}
	else
	{
		//Send the message
		int send_result = send(server_socket.sock, msg_buff, 4096 + 1, 0);
		if (send_result == SOCKET_ERROR)
		{
			exit_with_err_msg("Error in sending message to server. Error #" + to_string(WSAGetLastError()) + ". Exiting.");
		}
		else
		{
			cout << "Message successfully sent to server." << endl;
		}
	}
}

void receive_request_result(socket_info& server_socket, const char current_request)
{
	//Wait for response
	char request_result_buff[1];
	ZeroMemory(request_result_buff, 1);
	int bytes_received = recv(server_socket.sock, request_result_buff, 1, 0);

	if (bytes_received < 0)
	{
		exit_with_err_msg("Error in receiving request result from server. Error #" + to_string(WSAGetLastError()) + ". Exiting.");
	}
	else
	{
		//Echo response to console
		cout << "SERVER: " << string(request_result_buff, 0, bytes_received) << endl;
	}
}

void client_requests(socket_info& server_socket)
{
	for (char request = get_request(); request != 'q'; request = get_request())
	{
		bool request_acceptance_result = send_request(server_socket, request);
		if (request_acceptance_result)
		{
			if (request == 'm')
			{
				send_message(server_socket);
				receive_request_result(server_socket, 'm');
			}
		}
	}
}