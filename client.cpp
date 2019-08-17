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
	cout << "Server socket initialized.";
	return server_socket;
}

void connect_to_server(socket_info& server_socket)
{
	server_socket = create_server_socket();
	cout << " Connecting...";
	int connection_result = connect(server_socket.sock, (sockaddr*)& server_socket.sock_addr, sizeof(server_socket.sock_addr));
	if (connection_result == SOCKET_ERROR)
	{
		exit_with_err_msg("Connection to server failed. Error #" + to_string(WSAGetLastError()) + ". Exiting.");
	}
	cout << "Success." << endl;
}

void server_disconnection(socket_info &server_socket, const char IP_address[INET_ADDRSTRLEN], const int port_num)
{
	cout << "Server " << IP_address << " has disconnected from port " << port_num << ". Reconnect?" << endl;
	cout << "Enter y to reconnnect, and any other key to exit: ";
	char answer;
	cin >> answer;
	if (answer == 'y')
	{
		connect_to_server(server_socket);
	}
	else
	{
		cout << "Closing client." << endl;
		closesocket(server_socket.sock);
		WSACleanup();
		exit(0);
	}
}

int get_account_number()
{
	string account_number_str;
	cout << "Enter the number of the account you wish to access. Enter -1 to abort: ";
	while (getline(cin, account_number_str))
	{
		if (account_number_str == "-1") { break; }
		bool invalid_input = any_of(account_number_str.begin(), account_number_str.end(), is_char());
		if (invalid_input) { cout << "ERROR: Please enter numbers only: "; }
		if (account_number_str.empty()) { cout << "ERROR: Account number is empty. Please re-enter: "; }
		else { break; }
	}
	return stoi(account_number_str);
}

bool is_valid_balance(const string input)
{
	int num_of_point = count(input.begin(), input.end(), '.');

	if (num_of_point == 0)
	{
		string::const_iterator it = find_if(input.begin(), input.end(), is_char());
		return it == input.end();
	}
	else if (num_of_point == 1)
	{
		string::const_iterator dec_point_it = find(input.begin(), input.end(), '.');
		if (dec_point_it + 1 == input.end()) { return false; }
		string::const_iterator it = find_if(input.begin(), dec_point_it, is_char());
		if (it != dec_point_it) { return false; }
		else
		{
			if (dec_point_it + 2 == input.end())
			{
				return isdigit(input.back());
			}
			else if (dec_point_it + 3 == input.end())
			{
				return isdigit(input[(int)input.size() - 2]) && isdigit(input.back());
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}

}

double get_new_balance()
{
	string new_balance;

	cout << "Please enter the amount of the new balance: $";
	while (getline(cin, new_balance))
	{
		if (is_valid_balance(new_balance)) { break; }
		else
		{
			cout << new_balance << " isn't a correct currency. Please re-enter: $";
		}
	}
	return stod(new_balance);
}

bool send_request(socket_info& server_socket, const char request)
{
	char request_buff[2];
	request_buff[0]= request;
	request_buff[1] = '\0';
	int bytes_sent = send(server_socket.sock, request_buff, sizeof(request_buff) , 0);
	if (bytes_sent == SOCKET_ERROR)
	{
		exit_with_err_msg("Error in sending request to server. Error #" + to_string(WSAGetLastError()) + ". Exiting.");
		return false;
	}
	else if (bytes_sent == 0)
	{
		server_disconnection(server_socket, server_socket.IP_address, server_socket.port_num);
	}
	else
	{
		//Wait for request_acceptance_result from server
		char request_acceptance_result_buff[2];
		ZeroMemory(request_acceptance_result_buff, 2);
		int bytes_received = recv(server_socket.sock, request_acceptance_result_buff, sizeof(request_acceptance_result_buff), 0);
		if (bytes_received == SOCKET_ERROR)
		{
			exit_with_err_msg("Error in receiving request acceptance result from server. Error #" + to_string(WSAGetLastError()) + ". Exiting.");
		}
		else if (bytes_received == 0)
		{
			server_disconnection(server_socket, server_socket.IP_address, server_socket.port_num);
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

bool send_message(socket_info &server_socket)
{
	char msg_buff[4096];
	string user_msg = get_message();
	strcpy_s(msg_buff, user_msg.c_str());

	//Send the message
	int bytes_sent = send(server_socket.sock, msg_buff, sizeof(msg_buff), 0);
	if (bytes_sent == SOCKET_ERROR)
	{
		exit_with_err_msg("Error in sending message to server. Error #" + to_string(WSAGetLastError()) + ". Exiting.");
	}
	else if (bytes_sent == 0)
	{
		server_disconnection(server_socket, server_socket.IP_address, server_socket.port_num);
	}
	else
	{
		user_msg == "ABORT" ? cout << "Message transmission aborted." << endl :
			cout << "Message successfully sent to server." << endl;
	}

	return user_msg != "ABORT";
}

bool receive_process_result(socket_info& server_socket, const char current_request)
{
	//Wait for response
	char request_result_buff[2];
	ZeroMemory(request_result_buff, 2);
	int bytes_received = recv(server_socket.sock, request_result_buff, sizeof(request_result_buff), 0);
	if (bytes_received == SOCKET_ERROR)
	{
		exit_with_err_msg("Error in receiving request result from server. Error #" + to_string(WSAGetLastError()) + ". Exiting.");
	}
	else if (bytes_received == 0)
	{
		server_disconnection(server_socket, server_socket.IP_address, server_socket.port_num);
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
		case 'u':
			cout << "Server successfully updated the account." << endl;
			break;
		}
		
		return true;
	}
	else if (string(request_result_buff, 0, bytes_received) == "n")
	{
		cout << "Requested account already has the amount of new balance entered. No update has occured." << endl;
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
		case 'u':
			cout << "Server failed to update the account." << endl;
			break;
		}
	}

	return false;
}

void receive_account_balance(socket_info& server_socket)
{
	double balance_buff;

	//integer / double to and from network byte order isn't handled yet

	int bytes_received = recv(server_socket.sock, (char*) &balance_buff, sizeof(balance_buff), 0);
	if (bytes_received == SOCKET_ERROR)
	{
		cout << "ERROR in receiving requested balance from server " << server_socket.IP_address << ". Exiting." << endl;
		cout << "ERROR number: " << WSAGetLastError() << endl;
		return;
	}
	else if (bytes_received == 0)
	{
		server_disconnection(server_socket, server_socket.IP_address, server_socket.port_num);
		return;
	}

	cout << "Requested balance: $" << fixed << setprecision(2) << balance_buff << endl;
}

bool send_account_number(socket_info& server_socket)
{
	int account_number = get_account_number();
	int bytes_sent = send(server_socket.sock, (char*)& account_number, sizeof(account_number), 0);
	if (bytes_sent == SOCKET_ERROR)
	{
		exit_with_err_msg("Error in sending account number to server. Error #" + to_string(WSAGetLastError()) + ". Exiting.");
	}
	else if (bytes_sent == 0)
	{
		server_disconnection(server_socket, server_socket.IP_address, server_socket.port_num);
	}

	return account_number != -1;
}

void send_new_balance(socket_info& server_socket)
{
	double new_balance = get_new_balance();

	int bytes_sent = send(server_socket.sock, (char*)& new_balance, sizeof(new_balance), 0);
	if (bytes_sent == SOCKET_ERROR)
	{
		exit_with_err_msg("Error in sending new_balance to server. Error #" + to_string(WSAGetLastError()) + ". Exiting.");
	}
	else if (bytes_sent == 0)
	{
		server_disconnection(server_socket, server_socket.IP_address, server_socket.port_num);
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
				if (send_message(server_socket))
				{
					receive_process_result(server_socket, 'm');
				}				
			}
			else if (request == 'r')
			{
				if (send_account_number(server_socket) &&
					receive_process_result(server_socket, 'r'))
				{
					receive_account_balance(server_socket);
				}	
			}
			else if (request == 'u')
			{
				if (send_account_number(server_socket))
				{
					send_new_balance(server_socket);
					receive_process_result(server_socket, 'u');
				}
			}
			else
			{
				cout << "ERROR: Request " << request << " isn't a valid request. Please re-enter." << endl;
			}
		}
	}
}