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
	char request_buff[2];
	request_buff[0]= request;
	request_buff[1] = '\0';
	int send_result = send(server_socket.sock, request_buff, sizeof(request_buff) , 0);
	if (send_result == SOCKET_ERROR)
	{
		exit_with_err_msg("Error in sending request to server. Error #" + to_string(WSAGetLastError()) + ". Exiting.");
		return false;
	}
	else
	{
		//Wait for request_acceptance_result from server
		char request_acceptance_result_buff[2];
		ZeroMemory(request_acceptance_result_buff, 2);
		int bytes_received = recv(server_socket.sock, request_acceptance_result_buff, sizeof(request_acceptance_result_buff), 0);
		if (bytes_received < 0)
		{
			exit_with_err_msg("Error in receiving request acceptance result from server. Error #" + to_string(WSAGetLastError()) + ". Exiting.");
		}
		else
		{
			string request_acceptance_result = string(request_acceptance_result_buff, 0, bytes_received);
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
		int send_msg_result = send(server_socket.sock, msg_buff, sizeof(msg_buff), 0);
		if (send_msg_result == SOCKET_ERROR)
		{
			exit_with_err_msg("Error in sending message to server. Error #" + to_string(WSAGetLastError()) + ". Exiting.");
		}
		else
		{
			cout << "Message successfully sent to server." << endl;
		}
	}
}

bool receive_process_result(socket_info& server_socket, const char current_request)
{
	//Wait for response
	char request_result_buff[2];
	ZeroMemory(request_result_buff, 2);
	int bytes_received = recv(server_socket.sock, request_result_buff, sizeof(request_result_buff) + 2, 0);
	if (bytes_received < 0)
	{
		exit_with_err_msg("Error in receiving request result from server. Error #" + to_string(WSAGetLastError()) + ". Exiting.");
	}
	else if(string(request_result_buff, 0, bytes_received) == "s")
	{
		switch (current_request)
		{
		case 'm':
			cout << "Server successfully received message." << endl;
			break;
		case 'r':
			cout << "Server successfully accessed the account." << endl;
			break;
		}
		
		return true;
	}
	else if (string(request_result_buff, 0, bytes_received) == "f")
	{
		switch (current_request)
		{
		case 'm':
			cout << "Server failed to receive the message." << endl;
			break;
		case 'r':
			cout << "Server failed to access the account." << endl;
			break;
		}

		return false;
	}

	assert(false);//shouldn't reach here
	return false; //compiling purpose
}

void receive_account_balance(socket_info& server_socket)
{
	double balance_buff;

	//integer / double to and from network byte order isn't handled yet

	//receive message sent from the client
	int bytes_received = recv(server_socket.sock, (char*) &balance_buff, sizeof(balance_buff), 0);
	if (bytes_received == SOCKET_ERROR)
	{
		cout << "ERROR in receiving requested balance from server " << server_socket.IP_address << ". Exiting." << endl;
		cout << "ERROR number: " << WSAGetLastError() << endl;
		return;
	}
	else if (bytes_received == 0)
	{
		cout << "Server " << server_socket.IP_address << " has disconnected." << endl;
		return;
	}

	cout << "Requested balance: $" << balance_buff << endl;
}

int get_account_number()
{
	string account_number_str;
	cout << "Enter the number of the account you wish to read: ";
	while (getline(cin, account_number_str))
	{
		bool invalid_input = any_of(account_number_str.begin(), account_number_str.end(), is_char());
		if (invalid_input) { cout << "ERROR: Please enter numbers only: "; }
		if (account_number_str.empty()) { cout << "ERROR: Account number is empty. Please re-enter: "; }
		else { break; }
	}
	return stoi(account_number_str);
}

void send_account_number(socket_info& server_socket)
{
	int account_number = get_account_number();

	int send_result = send(server_socket.sock, (char*)& account_number, sizeof(account_number), 0);
	if (send_result == SOCKET_ERROR)
	{
		exit_with_err_msg("Error in sending account number to server. Error #" + to_string(WSAGetLastError()) + ". Exiting.");
	}
}

void client_requests(socket_info& server_socket)
{
	for (char request = get_request(); request != 'q'; request = get_request())
	{
		if (send_request(server_socket, request)) //if request is accepted by server
		{
			if (request == 'm')
			{
				send_message(server_socket);
				receive_process_result(server_socket, 'm');
			}
			else if (request == 'r')
			{
				send_account_number(server_socket);
				if (receive_process_result(server_socket, 'r'))
				{
					receive_account_balance(server_socket);
				}	
			}
			else if (request == 'u')
			{

			}
			else
			{
				cout << "ERROR: Request " << request << " isn't a valid request. Please re-enter." << endl;
			}
		}
	}
}